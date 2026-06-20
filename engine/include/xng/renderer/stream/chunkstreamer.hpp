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

#ifndef XENGINE_UPLOADBUFFER_HPP
#define XENGINE_UPLOADBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <utility>

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"
#include "xng/rendergraph/resource/buffer.hpp"
#include "xng/util/rangeallocator.hpp"

namespace xng {
    /**
     * There should only be one instance of a chunk streamer shared among all streamers.
     *
     * chunkSize * pinnedChunks is the expected streaming budget.
     * The budget is never exceeded without flushing.
     *
     * Flushed uploads can cause the chunk streamer to allocate new chunks and exceed the streaming budget.
     *
     * This allows hard flushes at the cost of dynamic allocation of staging buffers on budget overflow.
     *
     * A large chunkSize means bigger staging buffer allocations if a flushed upload requires new chunks beyond pinnedChunks.
     * A small chunkSize means more copy commands per upload but more granular streaming priority because the
     * non⁻flushed transfers from previous frames may complete faster and become available for flushed transfers.
     *
     * Upload works by splitting the uploaded data into fixed size chunks in ram and then uploading individual chunks
     * by copying into fixed size (chunkSize) staging buffers and from staging buffers to the target buffer.
     *
     * TODO: Implement ChunkStreamer upload priorities
     * TODO: Replace pinnedChunks with a single fixed size staging buffer + chunk sized dynamically allocated overflow buffers
     */
    class ChunkStreamer {
    public:
        typedef size_t Handle;

        ChunkStreamer(const ChunkStreamer &other) = delete;
        ChunkStreamer &operator=(const ChunkStreamer &other) = delete;

        /**
         * @param heap The heap to use for streaming
         * @param chunkSize The size of one streaming chunk.
         * @param pinnedChunks The minimum number of chunks that are kept pinned.
         */
        ChunkStreamer(rg::Heap &heap, const size_t chunkSize, const size_t pinnedChunks)
            : heap(heap),
              chunkSize(chunkSize),
              pinnedChunks(pinnedChunks) {
            for (auto i = 0; i < pinnedChunks; i++) {
                stagingBuffers.emplace_back(heap, chunkSize);
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
                        uploadChunks[ret].emplace_back(UploadChunk{
                            ret, chunkOffset, chunkSize, targetOffset + chunkOffset
                        });
                    }
                    const auto chunkOffset = (nChunks - 1) * chunkSize;
                    uploadChunks[ret].emplace_back(UploadChunk{
                        ret, chunkOffset, remainder, targetOffset + chunkOffset
                    });
                } else {
                    for (auto i = 0; i < nChunks; i++) {
                        const auto chunkOffset = i * chunkSize;
                        uploadChunks[ret].emplace_back(UploadChunk{
                            ret, chunkOffset, chunkSize, targetOffset + chunkOffset
                        });
                    }
                }
            } else {
                uploadChunks[ret].emplace_back(UploadChunk{ret, 0, dataSize, targetOffset});
            }

            uploadData[ret] = std::vector(data, data + dataSize);

            targetBuffers[ret] = targetBuffer;

            return ret;
        }

        void release(const Handle handle) {
            uploadChunks.erase(handle);
            uploadData.erase(handle);
            flushedUploads.erase(handle);
            inFlightUploads.erase(handle);
            freeHandles.push_back(handle);
        }

        bool isUploadComplete(const Handle handle) {
            auto it = inFlightUploads.find(handle);
            if (it != inFlightUploads.end()) {
                auto copy = it->second;
                it->second.clear();
                for (auto &transfer: copy) {
                    if (!transfer->isSignaled()) {
                        it->second.insert(transfer);
                    }
                }
                if (!it->second.empty()) {
                    return false;
                }
            }
            return uploadChunks.find(handle) == uploadChunks.end();
        }

        void flush(const Handle handle) {
            if (uploadChunks.find(handle) != uploadChunks.end()) {
                flushedUploads.insert(handle);
            }
        }

        /**
         * Set the target buffer of the given upload handle for the next commit() invocation.
         *
         * There may be chunks in flight from previous commit invocations when calling this method.
         * Users must insert a copy from the stale to the new buffer in the graph before calling commit().
         * This ensures the in flight chunks are present on the new buffer.
         *
         * @param handle The handle for which to set the target buffer
         * @param targetBuffer The target buffer to set
         */
        void setTargetBuffer(const Handle handle, const rg::HeapResource<rg::Buffer> &targetBuffer) {
            targetBuffers[handle] = targetBuffer;
        }

        void commit(std::vector<rg::TransferPass> passes) {
            const size_t budget = chunkSize * pinnedChunks;

            // Prune finished transfers and accumulate in flight size
            size_t inFlightSize = 0;
            std::vector stagingBuffersCopy = std::move(stagingBuffers);
            stagingBuffers.reserve(stagingBuffersCopy.size());
            for (auto &stagingBuffer: stagingBuffersCopy) {
                auto pendingCopy = stagingBuffer.pendingTransfers;
                for (auto &transfer: pendingCopy) {
                    if (transfer.first->isSignaled()) {
                        for (auto &chunk: transfer.second) {
                            stagingBuffer.allocator.free(chunk.offset, chunk.size);
                        }
                        stagingBuffer.pendingTransfers.erase(transfer.first);
                    } else {
                        for (auto &chunk: transfer.second) {
                            inFlightSize += chunk.size;
                        }
                    }
                }
                if (!stagingBuffer.pendingTransfers.empty()
                    || stagingBuffers.size() < pinnedChunks) {
                    stagingBuffers.emplace_back(std::move(stagingBuffer));
                }
            }

            // Gather Frame Upload Chunks
            std::vector<UploadChunk> frameChunks;
            for (auto &handle: flushedUploads) {
                for (auto &chunk: uploadChunks.at(handle)) {
                    inFlightSize += chunk.dataSize;
                    frameChunks.emplace_back(chunk);
                }
                uploadChunks.erase(handle);
            }
            flushedUploads.clear();

            auto uploads = std::move(uploadChunks);
            uploadChunks.reserve(uploads.size());
            for (auto &pair: uploads) {
                if (inFlightSize < budget && stagingBuffers.size() <= pinnedChunks) {
                    auto chunks = std::move(pair.second);
                    pair.second.clear();
                    for (auto &chunk: chunks) {
                        // Check if the chunk fits in budget
                        if (inFlightSize + chunk.dataSize <= budget) {
                            inFlightSize += chunk.dataSize;
                            frameChunks.emplace_back(chunk);
                        } else {
                            pair.second.emplace_back(chunk);
                        }
                    }
                }
                if (!pair.second.empty()) {
                    uploadChunks.emplace(pair.first, std::move(pair.second));
                }
            }

            // Upload Chunks
            std::vector<ChunkUpload> chunkUploads;
            for (auto &chunk: frameChunks) {
                size_t frameChunkSize = chunk.dataSize;

                size_t stagingOffset = 0;
                auto stagingIndex = getStagingBuffer(frameChunkSize, stagingOffset);
                auto &stagingBuffer = stagingBuffers.at(stagingIndex);

                auto &chunkData = uploadData.at(chunk.handle);

                stagingBuffer.mapping->copyFrom(chunkData, chunk.dataOffset, stagingOffset, chunk.dataSize);

                chunkUploads.emplace_back(stagingIndex,
                                          stagingOffset,
                                          chunk.chunkOffset,
                                          chunk.dataSize,
                                          chunk.handle,
                                          targetBuffers.at(chunk.handle));
            }

            auto builder = rg::TransferPassBuilder("ChunkStreamer/Upload");

            for (auto &upload: chunkUploads) {
                builder.read(stagingBuffers.at(upload.stagingIndex).buffer, upload.stagingOffset, upload.chunkSize);
                builder.write(upload.targetBuffer, upload.chunkOffset, upload.chunkSize);
            }

            auto pass = builder.execute(
                [this, chunkUploads](rg::TransferContext &ctx) {
                    for (auto &upload: chunkUploads) {
                        ctx.copyBuffer(upload.targetBuffer,
                                       stagingBuffers.at(upload.stagingIndex).buffer,
                                       upload.chunkOffset,
                                       upload.stagingOffset,
                                       upload.chunkSize);
                    }
                });

            passes.insert(passes.begin(), std::move(pass));

            auto transferHandle = std::shared_ptr(std::move(heap.transfer(passes)));

            for (auto &upload: chunkUploads) {
                stagingBuffers.at(upload.stagingIndex).pendingTransfers[transferHandle].emplace_back(upload.stagingOffset, upload.chunkSize);
                inFlightUploads[upload.handle].insert(transferHandle);
            }
        }

    private:
        struct UploadChunk {
            Handle handle;
            size_t dataOffset;
            size_t dataSize;
            size_t chunkOffset; // The offset into buffer
        };

        struct ChunkTransfer {
            size_t offset; // The offset into the staging buffer
            size_t size; // The size of the transfer

            ChunkTransfer(const size_t offset, const size_t size)
                : offset(offset), size(size) {
            }
        };

        struct StagingBuffer {
            rg::HeapResource<rg::Buffer> buffer;
            std::unique_ptr<rg::HeapMapping> mapping;
            std::unordered_map<std::shared_ptr<rg::Semaphore>, std::vector<ChunkTransfer>> pendingTransfers;

            RangeAllocator allocator;

            StagingBuffer(rg::Heap &heap, const size_t chunkSize)
                : buffer(heap.allocateBuffer(rg::Buffer(chunkSize,
                                                        rg::Buffer::CAPABILITY_TRANSFER_SRC,
                                                        rg::Buffer::MEMORY_CPU_TO_GPU))),
                  mapping(heap.map(buffer)),
                  allocator(chunkSize) {
            }
        };

        struct ChunkUpload {
            size_t stagingIndex;
            size_t stagingOffset;
            size_t chunkOffset;
            size_t chunkSize;
            Handle handle;
            rg::HeapResource<rg::Buffer> targetBuffer;

            ChunkUpload(const size_t stagingIndex,
                        const size_t stagingOffset,
                        const size_t chunkOffset,
                        const size_t chunkSize,
                        const Handle handle,
                        rg::HeapResource<rg::Buffer> targetBuffer)
                : stagingIndex(stagingIndex),
                  stagingOffset(stagingOffset),
                  chunkOffset(chunkOffset),
                  chunkSize(chunkSize),
                  handle(handle),
                  targetBuffer(std::move(targetBuffer)) {
            }
        };

        size_t getStagingBuffer(const size_t size, size_t &offset) {
            assert(size <= chunkSize);

            for (auto i = 0; i < stagingBuffers.size(); i++) {
                auto &stagingBuffer = stagingBuffers.at(i);
                if (stagingBuffer.allocator.allocate(size, offset)) {
                    return i;
                }
            }

            StagingBuffer stagingBuffer(heap, chunkSize);

            const auto allocated = stagingBuffer.allocator.allocate(size, offset);
            assert(allocated);

            stagingBuffers.emplace_back(std::move(stagingBuffer));

            return stagingBuffers.size() - 1;
        }

        rg::Heap &heap;

        const size_t chunkSize = 0;
        const size_t pinnedChunks = 0;

        std::unordered_map<Handle, rg::HeapResource<rg::Buffer> > targetBuffers;
        std::unordered_map<Handle, std::vector<uint8_t> > uploadData;
        std::unordered_map<Handle, std::vector<UploadChunk> > uploadChunks;
        std::unordered_set<Handle> flushedUploads;

        std::unordered_map<Handle, std::unordered_set<std::shared_ptr<rg::Semaphore> > > inFlightUploads;

        std::vector<StagingBuffer> stagingBuffers; // Each staging buffer is sized chunkSize

        Handle nextHandle = 0;
        std::vector<Handle> freeHandles;
    };
}

#endif //XENGINE_UPLOADBUFFER_HPP
