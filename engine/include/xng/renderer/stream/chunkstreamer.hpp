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

#ifndef XENGINE_CHUNKSTREAMER_HPP
#define XENGINE_CHUNKSTREAMER_HPP

#include <cstddef>
#include <cstdint>
#include <utility>

#include "xng/renderer/streamerqueue.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"
#include "xng/rendergraph/resource/buffer.hpp"
#include "xng/util/rangeallocator.hpp"

namespace xng {
    /**
     * There should only be one instance of a chunk streamer shared among all streamers.
     *
     * chunkSize * chunkCount is the expected streaming budget.
     * The budget is never exceeded without flushing.
     *
     * Flushed uploads can cause the chunk streamer to allocate new chunks and exceed the streaming budget.
     *
     * This allows hard flushes at the cost of dynamic allocation of staging buffers on budget overflow.
     *
     * A large chunkSize means bigger chunk buffer allocations if a flushed upload requires new chunks beyond pinnedChunks.
     * A small chunkSize means more copy commands per upload but more granular streaming priority because the
     * non⁻flushed transfers from previous frames may complete faster and become available for flushed transfers.
     *
     * Upload works by splitting the uploaded data into fixed size chunks in ram and then uploading individual chunks
     * by copying into fixed size (chunkSize) staging buffers and from staging buffers to chunk sized back buffers
     * and from back buffers to the target buffer.
     *
     * A chunk flows like so:
     *
     * RAM -> Staging Buffer
     *  - Cpu Copy into driver managed memory
     *
     * Staging Buffer -> Back Buffer
     *  - Hardware Copy in transfer context (Dedicated Transfer Queue on vulkan)
     *
     * Back Buffer -> Target Buffer
     *  - Hardware Copy in render context (Dedicated Graphics Queue on vulkan)
     *      The Graphics queue submission will here wait on a semaphore signaled by the Staging -> BackBuffer copy and
     *      perform the ownership transfer. As this copy is only ever started once the Staging -> BackBuffer copy has
     *      finished (Except Flushed uploads) there is no stall on the graphics queue.
     *
     * Each chunk gets their own back buffer and thus allows the graphics queue to copy the streamed data without stalling on
     * in flight uploads.
     *
     * TODO: Implement ChunkStreamer upload priorities
     * TODO: (Maybe) Sub Chunk allocation
     */
    class ChunkStreamer {
    public:
        typedef size_t Handle;

        ChunkStreamer(const ChunkStreamer &other) = delete;

        ChunkStreamer &operator=(const ChunkStreamer &other) = delete;

        /**
         * @param heap The heap to use for streaming
         * @param chunkSize The size of one streaming chunk.
         * @param chunkCount The minimum number of chunk buffers to keep allocated. (Flushing can allocate new chunks)
         */
        ChunkStreamer(rg::Heap &heap,
                      const size_t chunkSize,
                      const size_t chunkCount)
            : heap(heap),
              chunkSize(chunkSize),
              pinnedChunkBuffers(chunkCount) {
            for (auto i = 0; i < pinnedChunkBuffers; i++) {
                freeChunkBuffers.emplace_back(heap, chunkSize);
            }
        }

        Handle upload(const uint8_t *data,
                      const size_t dataSize,
                      const rg::HeapResource<rg::Buffer> &targetBuffer,
                      const size_t targetOffset) {
            if (!(targetBuffer.getDescription().capabilityFlags & rg::Buffer::CAPABILITY_TRANSFER_DST)) {
                throw std::runtime_error("Target buffer must have CAPABILITY_TRANSFER_DST capability");
            }

            Handle ret;
            if (freeHandles.empty()) {
                ret = nextHandle++;
            } else {
                ret = freeHandles.back();
                freeHandles.pop_back();
            }
            if (dataSize > chunkSize) {
                const auto nChunks = (dataSize + chunkSize - 1) / chunkSize;
                const auto remainder = dataSize % chunkSize;
                if (remainder > 0) {
                    for (auto i = 0; i < nChunks - 1; i++) {
                        const auto chunkOffset = i * chunkSize;
                        pendingChunks[ret].insert({
                            pendingChunks[ret].size(),
                            UploadChunk{
                                targetOffset + chunkOffset,
                                chunkOffset,
                                chunkSize
                            }
                        });
                    }
                    const auto chunkOffset = (nChunks - 1) * chunkSize;
                    pendingChunks[ret].insert({
                        pendingChunks[ret].size(),
                        UploadChunk{
                            targetOffset + chunkOffset,
                            chunkOffset,
                            remainder
                        }
                    });
                } else {
                    for (auto i = 0; i < nChunks; i++) {
                        const auto chunkOffset = i * chunkSize;
                        pendingChunks[ret].insert({
                            pendingChunks[ret].size(),
                            UploadChunk{
                                targetOffset + chunkOffset,
                                chunkOffset,
                                chunkSize
                            }
                        });
                    }
                }
            } else {
                pendingChunks[ret].insert({pendingChunks[ret].size(), UploadChunk{targetOffset, 0, dataSize}});
            }

            uploadData[ret] = std::vector(data, data + dataSize);

            targetBuffers[ret] = targetBuffer;

            return ret;
        }

        void release(const Handle handle) {
            targetBuffers.erase(handle);
            uploadData.erase(handle);
            pendingChunks.erase(handle);
            flushedUploads.erase(handle);

            const auto it = pendingChunkBuffers.find(handle);
            if (it != pendingChunkBuffers.end()) {
                for (auto &buffer: it->second) {
                    if (freeChunkBuffers.size() >= pinnedChunkBuffers) {
                        break;
                    }
                    buffer.pendingTransfer = nullptr;
                    freeChunkBuffers.emplace_back(std::move(buffer));
                }
                pendingChunkBuffers.erase(handle);
            }

            freeHandles.push_back(handle);
        }

        bool isUploadComplete(const Handle handle) {
            return flushedUploads.find(handle) != flushedUploads.end()
                   || pendingChunks.find(handle) == pendingChunks.end();
        }

        void flush(const Handle handle) {
            if (pendingChunks.find(handle) != pendingChunks.end()) {
                flushedUploads.insert(handle);
            }
        }

        /**
         * Set the target buffer of the given upload handle for the next commit() invocation.
         *
         * @param handle The handle for which to set the target buffer
         * @param targetBuffer The target buffer to set
         */
        void setTargetBuffer(const Handle handle, const rg::HeapResource<rg::Buffer> &targetBuffer) {
            targetBuffers[handle] = targetBuffer;
        }

        void commit(rg::GraphBuilder &graph, StreamerQueue &queue) {
            for (auto handle: flushedUploads) {
                const auto it = pendingChunks.find(handle);
                if (it != pendingChunks.end()) {
                    for (auto &pair: it->second) {
                        if (freeChunkBuffers.empty()) {
                            ChunkBuffer buffer(heap, chunkSize);
                            buffer.upload(queue,
                                          handle,
                                          uploadData.at(handle),
                                          pair.second.targetOffset,
                                          pair.second.chunkOffset,
                                          pair.second.dataSize);
                            pendingChunkBuffers[handle].emplace_back(std::move(buffer));
                        } else {
                            auto &buffer = freeChunkBuffers.back();
                            freeChunkBuffers.pop_back();
                            buffer.upload(queue,
                                          handle,
                                          uploadData.at(handle),
                                          pair.second.targetOffset,
                                          pair.second.chunkOffset,
                                          pair.second.dataSize);
                            pendingChunkBuffers[handle].emplace_back(std::move(buffer));
                        }
                    }
                    it->second.clear();
                }
            }

            auto stalePendingChunks = std::move(pendingChunks);
            pendingChunks.reserve(stalePendingChunks.size());
            for (auto &hPair: stalePendingChunks) {
                std::unordered_set<size_t> frameChunks;
                for (auto &pair: hPair.second) {
                    if (freeChunkBuffers.empty()) {
                        break;
                    }
                    auto &buffer = freeChunkBuffers.back();
                    freeChunkBuffers.pop_back();
                    buffer.upload(queue,
                                  hPair.first,
                                  uploadData.at(hPair.first),
                                  pair.second.targetOffset,
                                  pair.second.chunkOffset,
                                  pair.second.dataSize);
                    pendingChunkBuffers[hPair.first].emplace_back(std::move(buffer));
                    frameChunks.insert(pair.first);
                }
                for (auto &index: frameChunks) {
                    hPair.second.erase(index);
                }
                if (!hPair.second.empty()) {
                    pendingChunks[hPair.first] = std::move(hPair.second);
                }
            }

            // Copy finished chunks
            auto stalePendingChunkBuffers = std::move(pendingChunkBuffers);
            pendingChunkBuffers.reserve(stalePendingChunkBuffers.size());
            for (auto &pair: stalePendingChunkBuffers) {
                auto staleBuffers = std::move(pair.second);
                pair.second.reserve(staleBuffers.size());
                for (auto &buffer: staleBuffers) {
                    assert(buffer.pendingTransfer != nullptr);
                    if (flushedUploads.find(buffer.pendingTransferHandle) != flushedUploads.end()
                        || buffer.pendingTransfer->isSignaled()) {
                        // Copy to target on graph
                        const auto sourceBuffer = buffer.backBuffer;
                        const auto targetBuffer = targetBuffers.at(buffer.pendingTransferHandle);
                        const auto offset = buffer.pendingTransferOffset;
                        const auto size = buffer.pendingTransferSize;

                        // This will stall the graphics queue waiting on the transfer queue for flushed uploads.
                        auto pass = rg::GraphicsPassBuilder("ChunkStreamer/Copy")
                                .transferRead(buffer.backBuffer, 0, buffer.pendingTransferSize)
                                .transferWrite(targetBuffer, buffer.pendingTransferOffset, buffer.pendingTransferSize)
                                .execute([sourceBuffer, targetBuffer, offset, size](rg::RasterContext &,
                                rg::TransferContext &ctx,
                                rg::ComputeContext &) {
                                        ctx.copyBuffer(targetBuffer, sourceBuffer, offset, 0, size);
                                    });

                        graph.addPass(std::move(pass));

                        if (freeChunkBuffers.size() < pinnedChunkBuffers) {
                            buffer.pendingTransfer = nullptr;
                            freeChunkBuffers.emplace_back(std::move(buffer));
                        }
                    } else {
                        pair.second.emplace_back(std::move(buffer));
                    }
                }
                if (!pair.second.empty()) {
                    pendingChunkBuffers[pair.first] = std::move(pair.second);
                }
            }

            flushedUploads.clear();
        }

    private:
        struct UploadChunk {
            size_t targetOffset;
            size_t chunkOffset;
            size_t dataSize;
        };

        struct ChunkBuffer {
            rg::HeapResource<rg::Buffer> stagingBuffer;
            rg::HeapResource<rg::Buffer> backBuffer;

            std::unique_ptr<rg::HeapMapping> mapping;

            std::shared_ptr<StreamerQueue::SubmitFence> pendingTransfer{};
            Handle pendingTransferHandle{};
            size_t pendingTransferOffset{};
            size_t pendingTransferSize{};

            ChunkBuffer(rg::Heap &heap, const size_t chunkSize)
                : stagingBuffer(heap.allocateBuffer(rg::Buffer(chunkSize,
                                                               rg::Buffer::CAPABILITY_TRANSFER_SRC,
                                                               rg::Buffer::MEMORY_CPU_TO_GPU))),
                  backBuffer(heap.allocateBuffer(rg::Buffer(chunkSize,
                                                            rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                            rg::Buffer::MEMORY_GPU_ONLY))) {
                mapping = heap.map(stagingBuffer);
            }

            void upload(StreamerQueue &queue,
                        const Handle handle,
                        const std::vector<uint8_t> &data,
                        const size_t targetOffset,
                        const size_t chunkOffset,
                        const size_t size) {
                assert(pendingTransfer == nullptr);

                mapping->copyFrom(data, chunkOffset, 0, size);

                auto pass = rg::TransferPassBuilder("ChunkStreamer/Upload")
                        .read(stagingBuffer, 0, size)
                        .write(backBuffer, 0, size)
                        .execute([this, size](rg::TransferContext &ctx) {
                            ctx.copyBuffer(backBuffer, stagingBuffer, 0, 0, size);
                        });
                pendingTransfer = queue.addPass(std::move(pass));
                pendingTransferHandle = handle;
                pendingTransferOffset = targetOffset;
                pendingTransferSize = size;
            }
        };

        rg::Heap &heap;

        const size_t chunkSize = 0;
        const size_t pinnedChunkBuffers = 0;

        std::unordered_map<Handle, rg::HeapResource<rg::Buffer> > targetBuffers;
        std::unordered_map<Handle, std::vector<uint8_t> > uploadData;

        std::unordered_map<Handle, std::unordered_map<size_t, UploadChunk> > pendingChunks;

        std::unordered_map<Handle, std::vector<ChunkBuffer> > pendingChunkBuffers;

        std::unordered_set<Handle> flushedUploads;

        std::vector<ChunkBuffer> freeChunkBuffers;

        Handle nextHandle = 0;
        std::vector<Handle> freeHandles;
    };
}

#endif //XENGINE_CHUNKSTREAMER_HPP
