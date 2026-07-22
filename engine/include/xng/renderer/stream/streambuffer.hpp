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
     * The stream buffer dynamically grows in size if flushed uploads exceed targetSize and dynamically shrinks back to targetSize.
     *
     * TODO: Efficient stream buffer shrinking
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
              targetSize(targetSize),
              bufferSize(targetSize) {
        }

        ~StreamBuffer() = default;

        StreamBuffer(const StreamBuffer &) = delete;

        StreamBuffer &operator=(const StreamBuffer &) = delete;

        StreamBuffer(StreamBuffer &&other) noexcept
            : heap(other.heap),
              chunkStreamer(other.chunkStreamer),
              buffer(std::move(other.buffer)),
              targetSize(other.targetSize),
              bufferSize(other.bufferSize),
              uploads(std::move(other.uploads)),
              pendingUploads(std::move(other.pendingUploads)),
              flushedUploads(std::move(other.flushedUploads)),
              finishedUploads(std::move(other.finishedUploads)) {
        }

        StreamBuffer &operator=(StreamBuffer &&other) noexcept {
            if (&other == this) return *this;
            buffer = std::move(other.buffer);
            targetSize = other.targetSize;
            bufferSize = other.bufferSize;
            uploads = std::move(other.uploads);
            pendingUploads = std::move(other.pendingUploads);
            flushedUploads = std::move(other.flushedUploads);
            finishedUploads = std::move(other.finishedUploads);
            return *this;
        }

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
            bufferSize = std::max(bufferSize, offset + dataSize);
            bufferSize = std::max(bufferSize, targetSize);
            const auto ret = chunkStreamer.upload(data, dataSize, buffer, offset);
            uploads.emplace(ret, PendingUpload{offset, dataSize});
            pendingUploads.insert(ret);
            return ret;
        }

        /**
         *
         * @param handle
         * @return True if the passed upload has finished.
         */
        bool isUploadComplete(const Handle handle) const {
            return finishedUploads.find(handle) != finishedUploads.end()
                   || flushedUploads.find(handle) != flushedUploads.end();
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
            pendingUploads.erase(handle);
            flushedUploads.erase(handle);
            finishedUploads.erase(handle);
            uploads.erase(handle);
        }

        /**
         * Commit the pending uploads.
         *
         * Allocates and initializes a new stable buffer on resize
         *
         * @param queue
         * @return The handle to the buffer.
         */
        void commit(StreamerQueue &queue) {
            // On resize all previous chunk transfers have already been submitted on the graphics queue because
            // the copies from backBuffer -> buffer happen in the main graph in RenderPasses and thus on the graphics queue.

            // This means the resize only stalls on previously submitted copies on the graphics queue
            // which are already considered "finished" but not on any other concurrent transfers.

            // A streaming transfer is considered "finished" when it has passed the boundary of staging -> backBuffer
            // and the copy from backBuffer -> buffer is considered frame overhead because it cannot be overlapped
            // with both queues if exclusive queue ownership is assumed.

            // With this model the graphics queue will never wait on the transfer queue at the cost of the overhead
            // of copying the finished chunks from backBuffer to buffer. However, this copy can be overlapped
            // with other operations on the graphics queue because the runtime can use range granular pipeline barriers
            // as the copy happens completely intra queue.

            if (buffer.getDescription().size != bufferSize) {
                const auto staleBuffer = buffer;
                buffer = heap.allocateBuffer(rg::Buffer(bufferSize,
                                                        buffer.getDescription().capabilityFlags,
                                                        buffer.getDescription().memoryType));

                const auto copySize = std::min(staleBuffer.getDescription().size, buffer.getDescription().size);

                auto pass = rg::GraphicsPassBuilder("StreamBuffer/Resize")
                        .transferRead(staleBuffer, 0, staleBuffer.getDescription().size)
                        .transferWrite(buffer, 0, staleBuffer.getDescription().size)
                        .execute([this, staleBuffer, copySize](rg::RasterContext &,
                                                               rg::TransferContext &ctx,
                                                               rg::ComputeContext &) {
                            // The stale buffers are pinned via HeapResource references in the lambda and
                            // the runtime will pin the stale buffers additionally until the graph finished execution.
                            ctx.copyBuffer(buffer, staleBuffer, 0, 0, copySize);
                        });

                queue.addPass(std::move(pass));

                // Chunk Streamer inserts copies from chunk buffers to target after stream buffer commit
                // and all previous copies from chunk buffers to target buffer are synchronized on the graphics queue
                // which means all copies from chunk buffers to target happening this frame will write to the newly
                // allocated buffer and the copy from stale to new happens before the chunk buffer copy.
                for (auto &upload: pendingUploads) {
                    chunkStreamer.setTargetBuffer(upload, buffer);
                }
            }

            std::unordered_set<Handle> frameUploads;
            for (auto &upload: flushedUploads) {
                pendingUploads.erase(upload);
                finishedUploads.insert(upload);
                frameUploads.insert(upload);
            }

            for (auto &upload: pendingUploads) {
                if (chunkStreamer.isUploadComplete(upload)) {
                    flushedUploads.erase(upload);
                    finishedUploads.insert(upload);
                    frameUploads.insert(upload);
                }
            }

            for (auto &upload: frameUploads) {
                pendingUploads.erase(upload);
                flushedUploads.erase(upload);
            }
        }

        rg::HeapResource<rg::Buffer> getBuffer() const {
            return buffer;
        }

        void setTargetSize(const size_t size) {
            targetSize = size;
        }

    private:
        static constexpr size_t timeOut = 10'000'000'000ULL;

        struct PendingUpload {
            size_t offset;
            size_t size;
        };

        rg::Heap &heap;
        ChunkStreamer &chunkStreamer;

        rg::HeapResource<rg::Buffer> buffer;

        size_t targetSize = 0;
        size_t bufferSize = 0;

        std::unordered_map<Handle, PendingUpload> uploads;
        std::unordered_set<Handle> pendingUploads;
        std::unordered_set<Handle> flushedUploads;
        std::unordered_set<Handle> finishedUploads;
    };
}

#endif //XENGINE_STREAMBUFFER_HPP
