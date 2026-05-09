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
     * This enables asynchronous buffer streaming by not blocking on the slow PCIe copy from the Staging Buffers.
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

        /**
         * Upload data to the stream buffer.
         * The copy to the staging buffer is performed on the calling thread.
         *
         * Uploads are not ordered.
         *
         * @param offset The offset into the stable buffer to upload to.
         * @param data
         * @return The offset of the data in the stable buffer.
         */
        Handle upload(const size_t offset, const std::vector<uint8_t> &data) {
            //TODO: Upload batching (Single staging / temp buffer)
            if (offset + data.size() > bufferSize) {
                bufferSize = offset + data.size();
            }

            const auto ret = createUploadHandle();

            const auto stagingBuffer = heap.allocateBuffer(rg::Buffer(data.size(),
                                                                      rg::Buffer::CAPABILITY_TRANSFER_SRC,
                                                                      rg::Buffer::MEMORY_CPU_TO_GPU));

            // Perform copy to staging buffer
            {
                const auto mapping = heap.mapBuffer(stagingBuffer);
                mapping.copyFrom(data, 0, 0, data.size());
            }

            const auto tempBuffer = heap.allocateBuffer(rg::Buffer(data.size(),
                                                                   rg::Buffer::CAPABILITY_TRANSFER_SRC
                                                                   | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                                   rg::Buffer::MEMORY_GPU_ONLY));

            // Queue copy of staging -> temp buffer
            heap.getTransferContext().copyBuffer(stagingBuffer, tempBuffer, 0, 0, data.size());

            pendingUploads.emplace(ret, PendingUpload{offset, tempBuffer});

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
            } else {
                return !heap.hasPendingTransfers(it->second.buffer);
            }
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
         * @param builder
         * @return The handle to the buffer.
         */
        const rg::HeapResource<rg::Buffer> &commit(rg::GraphBuilder &builder) {
            auto passBuilder = builder.addTransferPass("StreamBufferTransfer");

            rg::HeapResource<rg::Buffer> staleBuffer;
            if (buffer.getDescription().size != bufferSize) {
                staleBuffer = buffer;
                buffer = heap.allocateBuffer(rg::Buffer(bufferSize,
                                                        buffer.getDescription().capabilityFlags,
                                                        rg::Buffer::MEMORY_GPU_ONLY));

                passBuilder.read(staleBuffer);
                passBuilder.write(buffer);
            }

            const auto pendingUploadsCopy = pendingUploads;
            std::unordered_map<Handle, PendingUpload> frameUploads;
            for (auto &upload: pendingUploadsCopy) {
                if (!heap.hasPendingTransfers(upload.second.buffer)
                    || flushedUploads.find(upload.first) != flushedUploads.end()) {
                    passBuilder.read(upload.second.buffer);

                    frameUploads.emplace(upload);
                    pendingUploads.erase(upload.first);
                    flushedUploads.erase(upload.first);
                    freeUploadHandle(upload.first);
                }
            }

            if (!staleBuffer.isAssigned() && !frameUploads.empty()) {
                passBuilder.write(buffer);
            }

            if (staleBuffer.isAssigned() || !frameUploads.empty()) {
                passBuilder.execute([this, staleBuffer, frameUploads](rg::TransferContext &ctx) {
                    // Copy stale buffer if reallocated
                    if (staleBuffer.isAssigned()) {
                        ctx.copyBuffer(buffer, staleBuffer, 0, 0, staleBuffer.getDescription().size);
                    }

                    // Copy flushed create buffers into stable.
                    for (auto &upload: frameUploads) {
                        ctx.copyBuffer(buffer,
                                       upload.second.buffer,
                                       upload.second.offset,
                                       0,
                                       upload.second.buffer.getDescription().size);
                    }
                });
            }

            return buffer;
        }

    private:
        struct PendingUpload {
            size_t offset;
            rg::HeapResource<rg::Buffer> buffer;
        };

        Handle createUploadHandle() {
            if (pendingUploadHandles.empty()) {
                return currentUploadHandle++;
            } else {
                const auto ret = pendingUploadHandles.back();
                pendingUploadHandles.pop_back();
                return ret;
            }
        }

        void freeUploadHandle(const Handle handle) {
            pendingUploadHandles.push_back(handle);
        }

        Handle currentUploadHandle = 0;
        std::vector<Handle> pendingUploadHandles;

        rg::Heap &heap;

        rg::HeapResource<rg::Buffer> buffer;

        std::unordered_map<Handle, PendingUpload> pendingUploads;
        std::unordered_set<Handle> flushedUploads;

        size_t bufferSize = 0;
    };
}

#endif //XENGINE_STREAMBUFFER_HPP
