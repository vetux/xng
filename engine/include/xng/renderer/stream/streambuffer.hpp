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

#include "xng/renderer/stream/chunkstreamer.hpp"

namespace xng {
    /**
     * The stream buffer is double-buffered to avoid queue resource ownership transfer stalls.
     *
     * The stream buffer dynamically grows in size if flushed uploads exceed targetSize and dynamically shrinks back to targetSize.
     *
     * TODO: Implement dynamic double buffer size (Instead of full size double buffer)
     */
    class StreamBuffer {
    public:
        typedef ChunkStreamer::Handle Handle;

        StreamBuffer(rg::Heap &heap,
                     ChunkStreamer &chunkStreamer,
                     const rg::Buffer::Capability capabilities,
                     const size_t targetSize = 1)
            : heap(heap),
              chunkStreamer(chunkStreamer),
              buffer(heap.allocateBuffer(rg::Buffer(targetSize,
                                                    capabilities
                                                    | rg::Buffer::CAPABILITY_TRANSFER_SRC
                                                    | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                    rg::Buffer::MEMORY_GPU_ONLY))),
              backBuffer(heap.allocateBuffer(rg::Buffer(targetSize,
                                                        rg::Buffer::CAPABILITY_TRANSFER_SRC
                                                        | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                        rg::Buffer::MEMORY_GPU_ONLY))),
              targetSize(targetSize) {
        }

        ~StreamBuffer() = default;

        Handle upload(const std::vector<uint8_t> &data, const size_t offset) {
            return upload(data.data(), data.size(), offset);
        }

        /**
         * Uploads are not ordered.
         *
         * Concurrent overlapping uploads are forbidden.
         *
         * @param data The pointer to the start of the data to upload
         * @param dataSize The size of the data to upload
         * @param offset The offset into the stable buffer to upload to.
         * @return The offset of the data in the stable buffer.
         */
        Handle upload(const uint8_t *data, const size_t dataSize, const size_t offset) {
            const auto ret = chunkStreamer.upload(data, dataSize, backBuffer, offset);
            uploads.emplace(ret, PendingUpload{offset, dataSize});
            return ret;
        }

        /**
         *
         * @param handle
         * @return True if the passed upload has finished.
         */
        bool isUploadComplete(const Handle handle) const {
            return chunkStreamer.isUploadComplete(handle);
        }

        /**
         * Forces a flush of the passed upload handle.
         *
         * @param handle The handle of the upload to flush
         */
        void flush(const Handle handle) {
            if (finishedUploads.find(handle) == finishedUploads.end()) {
                flushedUploads.insert(handle);
                chunkStreamer.flush(handle);
            }
        }

        /**
         * Release the passed handle.
         * Pending uploads for the given handle are canceled.
         *
         * @param handle The handle to release
         */
        void release(const Handle handle) {
            chunkStreamer.release(handle);
            uploads.erase(handle);
            flushedUploads.erase(handle);
            finishedUploads.erase(handle);
        }

        /**
         * Commit the pending uploads.
         *
         * Allocates and initializes a new stable buffer on resize
         *
         * @param graph
         * @return The handle to the buffer.
         */
        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            std::vector<rg::TransferPass> ret;

            // On Resize the chunk streamer might have chunks in flight writing to the stale buffer
            // The copy of staleBackBuffer -> new backBuffer synchronizes the in flight chunks
            // By setting a new target buffer on the chunk streamer the ChunkStreamer::commit then
            // submits new chunks to the new target buffer, and the copy syncs in flight transfers.
            // Chunk streamer commit must be called after all stream buffer commits.
            const auto bufferSize = getBufferSize();

            if (buffer.getDescription().size != bufferSize) {
                // This will stall the graph execution on all in flight chunk uploads on the backBuffer
                // and may issue a queue ownership transfer if the runtime executes the graph transfer passes
                // on the graphics queue.
                const auto staleBuffer = buffer;
                buffer = heap.allocateBuffer(rg::Buffer(bufferSize,
                                                        buffer.getDescription().capabilityFlags,
                                                        buffer.getDescription().memoryType));

                const auto staleBackBuffer = backBuffer;
                backBuffer = heap.allocateBuffer(rg::Buffer(bufferSize,
                                                            backBuffer.getDescription().capabilityFlags,
                                                            backBuffer.getDescription().memoryType));

                const auto copySize = std::min(staleBuffer.getDescription().size, buffer.getDescription().size);

                const auto pass = rg::TransferPassBuilder("StreamBuffer/Resize")
                        .read(staleBuffer, 0, staleBuffer.getDescription().size)
                        .write(buffer, 0, staleBuffer.getDescription().size)
                        .read(staleBackBuffer, 0, staleBackBuffer.getDescription().size)
                        .write(backBuffer, 0, staleBackBuffer.getDescription().size)
                        .execute([this, staleBuffer, staleBackBuffer, copySize](rg::TransferContext &ctx) {
                            ctx.copyBuffer(buffer, staleBuffer, 0, 0, copySize);
                            ctx.copyBuffer(backBuffer, staleBackBuffer, 0, 0, copySize);
                        });

                //TODO: Find clean performant solution to resize.
                if (!heap.transfer({pass})->wait(timeOut)) {
                    throw std::runtime_error("Failed to resize stream buffer");
                }

                for (auto &upload: uploads) {
                    chunkStreamer.setTargetBuffer(upload.first, backBuffer);
                }
            }

            std::unordered_map<Handle, PendingUpload> frameUploads;
            for (auto &upload: uploads) {
                if (finishedUploads.find(upload.first) == finishedUploads.end()) {
                    if (flushedUploads.find(upload.first) != flushedUploads.end()
                        || chunkStreamer.isUploadComplete(upload.first)) {
                        frameUploads.emplace(upload.first, upload.second);
                        flushedUploads.erase(upload.first);
                        finishedUploads.insert(upload.first);
                    }
                }
            }

            // Perform copies of the finished uploads in the heap context.
            // This means subsequent RasterPasses stall only on the ownership transfer of the buffer after the copy is finished
            // while in the copy pass the queue synchronizes on the range granular copies from staging -> backBuffer -> buffer
            if (!frameUploads.empty()) {
                auto builder = rg::TransferPassBuilder("StreamBuffer/Copy");
                for (auto &upload: frameUploads) {
                    builder.read(backBuffer, upload.second.offset, upload.second.size);
                    builder.write(buffer, upload.second.offset, upload.second.size);
                }
                auto pass = builder.execute([this, frameUploads](rg::TransferContext &ctx) {
                    for (auto &upload: frameUploads) {
                        ctx.copyBuffer(buffer,
                                       backBuffer,
                                       upload.second.offset,
                                       upload.second.offset,
                                       upload.second.size);
                    }
                });
                /**
                 * This is a WAR hazard if the chunk streamer commit passes are not submitted before this pass
                 * because the flushed uploads to the backBuffer must complete before copying them to the stable buffer.
                 * To solve this the heap transfer dispatch is deferred to the caller.
                 */
                ret.emplace_back(std::move(pass));
            }
            return ret;
        }

        rg::HeapResource<rg::Buffer> getBuffer() const {
            return buffer;
        }

        void setTargetSize(const size_t size) {
            targetSize = size;
        }

    private:
        static constexpr size_t timeOut = 10'000'000'000ULL;

        size_t getBufferSize() const {
            size_t ret = targetSize;
            for (auto &upload: uploads) {
                ret = std::max(ret, upload.second.offset + upload.second.size);
            }
            return ret;
        }

        struct PendingUpload {
            size_t offset;
            size_t size;
        };

        rg::Heap &heap;
        ChunkStreamer &chunkStreamer;

        rg::HeapResource<rg::Buffer> buffer;
        rg::HeapResource<rg::Buffer> backBuffer;

        size_t targetSize = 0;

        std::unordered_map<Handle, PendingUpload> uploads;
        std::unordered_set<Handle> flushedUploads;
        std::unordered_set<Handle> finishedUploads;
    };
}

#endif //XENGINE_STREAMBUFFER_HPP
