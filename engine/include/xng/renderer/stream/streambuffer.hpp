/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_STREAMBUFFER_HPP
#define XENGINE_STREAMBUFFER_HPP

#include <stack>
#include <cstring>

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/pass.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

namespace xng {
    /**
     * A generic buffer into which data can be streamed asynchronously via
     *
     * Copy to a mapped staging buffer
     * Copy staging buffer -> Temp Buffer asynchronously on the heap transfer context
     * Copy Temp Buffer -> Stable Buffer in a graph transfer pass
     *
     * This enables asynchronous buffer streaming by not blocking on the slow PCIe copy from the Staging Buffers
     * and not blocking the graph queues on the copy from temp to stable buffer by double buffering the temp buffer.
     *
     * The stream buffer dynamically resizes on create and can be manually resized via resize().
     */
    class StreamBuffer {
    public:
        typedef size_t Handle;

        StreamBuffer(rg::Heap &heap,
                     const rg::Buffer::Capability capabilities,
                     const size_t initialSize = 1024)
            : heap(heap), bufferSize(initialSize) {
            buffer = heap.allocateBuffer(rg::Buffer(initialSize,
                                                    capabilities
                                                    | rg::Buffer::CAPABILITY_TRANSFER_SRC
                                                    | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                    rg::Buffer::MEMORY_GPU_ONLY));
        }

        ~StreamBuffer() = default;

        Handle upload(const std::vector<uint8_t> &data, const size_t offset) {
            return upload(data.data(), data.size(), offset);
        }

        /**
         * Upload data to the stream buffer.
         * The copy to the staging buffer is performed on the calling thread.
         *
         * Uploads are not ordered.
         *
         * Concurrent overlapping uploads are forbidden.
         *
         * @param data The pointer to the start of the data to upload
         * @param size The size of the data to upload
         * @param offset The offset into the stable buffer to upload to.
         * @return The offset of the data in the stable buffer.
         */
        Handle upload(const uint8_t *data, const size_t size, const size_t offset) {
            //TODO: Upload batching (Single staging / temp buffer) + Double Buffered temp buffer
            for (auto &pendingUpload: pendingUploads) {
                const auto start = pendingUpload.second.offset;
                const auto end = start + pendingUpload.second.buffer.getDescription().size;
                if (offset < end && start < offset + size) {
                    // Overlapping upload
                    throw std::runtime_error("Overlapping upload");
                }
            }

            if (offset + size > bufferSize) {
                bufferSize = offset + size;
            }

            const auto ret = createUploadHandle();

            const auto stagingBuffer = heap.allocateBuffer(rg::Buffer(size,
                                                                      rg::Buffer::CAPABILITY_TRANSFER_SRC,
                                                                      rg::Buffer::MEMORY_CPU_TO_GPU));

            // Perform copy to staging buffer
            {
                const auto mapping = heap.map(stagingBuffer);
                assert(stagingBuffer.getDescription().size >= size);
                std::memcpy(mapping->begin(), data, size);
            }

            const auto tempBuffer = heap.allocateBuffer(rg::Buffer(size,
                                                                   rg::Buffer::CAPABILITY_TRANSFER_SRC
                                                                   | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                                   rg::Buffer::MEMORY_GPU_ONLY));

            // Queue copy of staging -> temp buffer
            const auto pass = rg::TransferPassBuilder("StreamBuffer/Upload")
                    .read(stagingBuffer, 0, 0)
                    .write(tempBuffer, 0, 0)
                    .execute([stagingBuffer, tempBuffer, size](rg::TransferContext &ctx) {
                        ctx.copyBuffer(tempBuffer, stagingBuffer, 0, 0, size);
                    });

            auto transfer = heap.transfer(pass);

            pendingUploads.emplace(ret, PendingUpload(offset, tempBuffer, std::move(transfer)));

            return ret;
        }

        /**
         *
         * @param handle
         * @return True if the passed upload has finished.
         */
        bool isUploadComplete(const Handle handle) const {
            const auto it = pendingUploads.find(handle);
            if (it == pendingUploads.end()) {
                return true;
            }
            return it->second.transferHandle->isFinished();
        }

        void cancel(const Handle handle) {
            const auto it = pendingUploads.find(handle);
            if (it == pendingUploads.end()) {
                return;
            }
            flushedUploads.erase(handle);
            pendingUploads.erase(it);
            freeUploadHandle(handle);
        }

        /**
         * Forces a flush of the passed create.
         *
         * @param handle The handle of the upload to flush
         */
        void flush(const Handle handle) {
            if (pendingUploads.find(handle) != pendingUploads.end()) {
                flushedUploads.insert(handle);
            }
        }

        /**
         * Cancels all pending uploads that are outside the new allocated range.
         * Data up to newSize in the current buffer is copied into the new buffer.
         *
         * @param newSize The new buffer size
         */
        void resize(const size_t newSize) {
            if (newSize <= bufferSize) {
                const auto pendingUploadsCopy = pendingUploads;
                for (auto &upload: pendingUploadsCopy) {
                    if (upload.second.offset + upload.second.buffer.getDescription().size > newSize) {
                        pendingUploads.erase(upload.first);
                        flushedUploads.erase(upload.first);
                        freeUploadHandle(upload.first);
                    }
                }
            }
            bufferSize = newSize;
        }

        /**
         * Commit the pending uploads.
         *
         * Allocates and initializes a new stable buffer on resize
         *
         * Inserts the transfer pass performing the copy from the temp buffers to the stable buffer.
         *
         * Must be called every frame where the stable buffer is referenced.
         *
         * @param graph
         * @return The handle to the buffer.
         */
        const rg::HeapResource<rg::Buffer> &commit(rg::GraphBuilder &graph) {
            // Reallocate / Copy buffer on resize
            if (buffer.getDescription().size != bufferSize) {
                const auto staleBuffer = buffer;
                buffer = heap.allocateBuffer(rg::Buffer(bufferSize,
                                                        buffer.getDescription().capabilityFlags,
                                                        rg::Buffer::MEMORY_GPU_ONLY));
                const auto pass = rg::TransferPassBuilder("StreamBuffer/Commit/Resize")
                        .read(staleBuffer, 0, 0)
                        .write(buffer, 0, staleBuffer.getDescription().size)
                        .execute([this, staleBuffer](rg::TransferContext &ctx) {
                            ctx.copyBuffer(buffer, staleBuffer, 0, 0, staleBuffer.getDescription().size);
                        });
                graph.addPass(pass);
            }

            const auto pendingUploadsCopy = pendingUploads;
            std::unordered_map<Handle, PendingUpload> frameUploads;
            for (auto &upload: pendingUploadsCopy) {
                if (upload.second.transferHandle->isFinished()
                    || flushedUploads.find(upload.first) != flushedUploads.end()) {
                    frameUploads.emplace(upload);
                    pendingUploads.erase(upload.first);
                    flushedUploads.erase(upload.first);
                    freeUploadHandle(upload.first);
                }
            }

            if (!frameUploads.empty()) {
                auto builder = rg::TransferPassBuilder("StreamBuffer/Commit/Upload");
                for (auto &upload: frameUploads) {
                    builder.read(upload.second.buffer, 0, 0);
                    builder.write(buffer, upload.second.offset, upload.second.buffer.getDescription().size);
                }
                const auto pass = builder.execute([this, frameUploads](rg::TransferContext &ctx) {
                    // Copy flushed create buffers into stable.
                    for (auto &upload: frameUploads) {
                        ctx.copyBuffer(buffer,
                                       upload.second.buffer,
                                       upload.second.offset,
                                       0,
                                       upload.second.buffer.getDescription().size);
                    }
                });
                graph.addPass(pass);
            }

            return buffer;
        }

    private:
        struct PendingUpload {
            size_t offset;
            rg::HeapResource<rg::Buffer> buffer;
            std::shared_ptr<rg::HeapTransfer> transferHandle;

            PendingUpload(const size_t offset,
                          rg::HeapResource<rg::Buffer> _buffer,
                          std::unique_ptr<rg::HeapTransfer> _transferHandle)
                : offset(offset), buffer(std::move(_buffer)), transferHandle(std::move(_transferHandle)) {
            }
        };

        Handle createUploadHandle() {
            if (freeUploadHandles.empty()) {
                return nextUploadHandle++;
            }
            const auto ret = freeUploadHandles.back();
            freeUploadHandles.pop_back();
            return ret;
        }

        void freeUploadHandle(const Handle handle) {
            freeUploadHandles.push_back(handle);
        }

        Handle nextUploadHandle = 0;
        std::vector<Handle> freeUploadHandles;

        rg::Heap &heap;

        rg::HeapResource<rg::Buffer> buffer;

        std::unordered_map<Handle, PendingUpload> pendingUploads;
        std::unordered_set<Handle> flushedUploads;

        size_t bufferSize = 0;
    };
}

#endif //XENGINE_STREAMBUFFER_HPP
