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

#ifndef XENGINE_STREAMTEXTURE_HPP
#define XENGINE_STREAMTEXTURE_HPP

#include <compare>
#include <cstddef>
#include <format>
#include <utility>

#include "xng/assets/image.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng {
    /**
     * Fixed resolution slab-based allocation using Array Texture.
     *
     * CPU Buffer -> StreamBuffer -> Texture
     *
     * Texture slots can also be allocated and bound to shaders for writing instead of streaming from cpu. (Shadow Mapping, IBL)
     */
    class StreamTexture {
    public:
        /**
         * The index into the array texture.
         */
        typedef unsigned int Slot;

        explicit StreamTexture(rg::Heap &heap, ChunkStreamer &chunkStreamer, rg::Texture desc)
            : heap(heap),
              chunkStreamer(chunkStreamer),
              buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_TRANSFER_SRC) {
            switch (desc.textureType) {
                case rg::TEXTURE_2D:
                    desc.textureType = rg::TEXTURE_2D_ARRAY;
                    break;
                case rg::TEXTURE_2D_MULTISAMPLE:
                    desc.textureType = rg::TEXTURE_2D_MULTISAMPLE_ARRAY;
                    break;
                case rg::TEXTURE_CUBE_MAP:
                    desc.textureType = rg::TEXTURE_CUBE_MAP_ARRAY;
                    break;
                default:
                    throw std::runtime_error("Unsupported texture type for stream texture");
            }
            desc.capabilities = desc.capabilities | rg::Texture::CAPABILITY_TRANSFER_DST;
            texture = heap.allocateTexture(desc);
        }

        ~StreamTexture() = default;

        Slot create() {
            if (!freeSlots.empty()) {
                const auto ret = freeSlots.back();
                freeSlots.pop_back();
                return ret;
            }
            return nextSlot++;
        }

        void destroy(const Slot &slot) {
            auto it = pendingUploads.find(slot);
            if (it != pendingUploads.end()) {
                buffer.release(it->second.bufferHandle);
            }
            pendingUploads.erase(slot);
            freeSlots.push_back(slot);
        }

        void upload(const Slot &slot,
                    const ImageRGBA &image,
                    const int mipLevel = 0) {
            auto it = pendingUploads.find(slot);
            if (it != pendingUploads.end()) {
                buffer.release(it->second.bufferHandle);
                bufferAllocator.free(it->second.bufferOffset, it->second.bufferSize);
                pendingUploads.erase(slot);
            }

            const auto offset = bufferAllocator.allocate(sizeof(ColorRGBA) * image.getBuffer().size());
            const auto size = sizeof(ColorRGBA) * image.getBuffer().size();

            const auto bufferHandle = buffer.upload(reinterpret_cast<const uint8_t *>(image.getBuffer().data()),
                                                    size,
                                                    offset);

            pendingUploads.insert(std::pair(slot, PendingUpload(bufferHandle, rg::SRGB8_ALPHA8, offset, size, mipLevel)));
        }

        bool isUploadComplete(const Slot &slot) {
            const auto it = pendingUploads.find(slot);
            if (it == pendingUploads.end()) return true;
            return buffer.isUploadComplete(it->second.bufferHandle);
        }

        void flush(const Slot &handle) {
            const auto it = pendingUploads.find(handle);
            if (it == pendingUploads.end()) return;
            auto &pendingUpload = it->second;
            if (!pendingUpload.flushed) {
                buffer.flush(pendingUpload.bufferHandle);
                pendingUpload.flushed = true;
            }
        }

        const rg::HeapResource<rg::Texture> &commit(rg::GraphBuilder &graph) {
            // Resize / copy texture
            if (nextSlot > texture.getDescription().arrayLayers) {
                const auto staleTexture = texture;
                auto desc = staleTexture.getDescription();
                desc.arrayLayers = nextSlot;
                texture = heap.allocateTexture(desc);
                const auto pass = rg::TransferPassBuilder("StreamTexture/Resize")
                        .read(staleTexture, rg::TextureBinding::Range(0,
                                                                      staleTexture.getDescription().mipLevels,
                                                                      0,
                                                                      staleTexture.getDescription().arrayLayers))
                        .write(texture, rg::TextureBinding::Range(0,
                                                                  staleTexture.getDescription().mipLevels,
                                                                  0,
                                                                  staleTexture.getDescription().arrayLayers
                               ))
                        .execute([this, staleTexture](rg::TransferContext &ctx) {
                            std::vector<rg::TransferContext::TextureCopyRegion> regions;
                            for (auto i = 0; i < staleTexture.getDescription().mipLevels; i++) {
                                rg::TransferContext::TextureCopyRegion region;
                                region.src = rg::Texture::SubResource(i, 0, {});
                                region.dst = rg::Texture::SubResource(i, 0, {});
                                region.size = staleTexture.getDescription().getMipLevelSize(i);
                                if (texture.getDescription().textureType == rg::TEXTURE_CUBE_MAP_ARRAY) {
                                    region.depth = staleTexture.getDescription().arrayLayers * 6;
                                } else {
                                    region.depth = staleTexture.getDescription().arrayLayers;
                                }
                                regions.push_back(region);
                            }
                            ctx.copyTexture(texture, staleTexture, regions);
                        });
                graph.addPass(pass);
            }

            const auto stableBuffer = buffer.commit(graph);

            // Copy flushed / Completed uploads
            std::unordered_set<Slot> evictedHandles;
            for (auto &pair: pendingUploads) {
                auto &pendingUpload = pair.second;
                if (pendingUpload.flushed || buffer.isUploadComplete(pendingUpload.bufferHandle)) {
                    // Execute copy from stream buffer to texture
                    const auto slot = pair.first;
                    const auto upload = std::move(pendingUpload);

                    const auto pass = rg::TransferPassBuilder("StreamTexture/Commit")
                            .read(stableBuffer, upload.bufferOffset, upload.bufferSize)
                            .write(texture, rg::TextureBinding::Range(upload.mipLevel, 1, pair.first, 1))
                            .execute([this, slot, upload, stableBuffer](rg::TransferContext &ctx) {
                                const auto mipSize = texture.getDescription().getMipLevelSize(upload.mipLevel);
                                ctx.copyBufferToTexture(texture,
                                                        stableBuffer,
                                                        rg::Texture::SubResource(upload.mipLevel, slot, {}),
                                                        upload.bufferOffset,
                                                        Recti({}, mipSize),
                                                        upload.bufferFormat);
                            });
                    graph.addPass(pass);

                    evictedHandles.insert(pair.first);
                }
            }

            for (auto &handle: evictedHandles) {
                buffer.release(pendingUploads.at(handle).bufferHandle);
                auto &p = pendingUploads.at(handle);
                bufferAllocator.free(p.bufferOffset, p.bufferSize);
                pendingUploads.erase(handle);
            }

            return texture;
        }

    private:
        struct PendingUpload {
            StreamBuffer::Handle bufferHandle;
            rg::ColorFormat bufferFormat;

            size_t bufferOffset;
            size_t bufferSize;

            int mipLevel;
            bool flushed = false;

            PendingUpload(const StreamBuffer::Handle bufferHandle,
                          const rg::ColorFormat bufferFormat,
                          size_t bufferOffset,
                          size_t bufferSize,
                          const int mipLevel)
                : bufferHandle(bufferHandle),
                  bufferFormat(bufferFormat),
                  bufferOffset(bufferOffset),
                  bufferSize(bufferSize),
                  mipLevel(mipLevel) {
            }
        };

        rg::Heap &heap;
        ChunkStreamer &chunkStreamer;

        StreamBuffer buffer;
        RangeAllocator bufferAllocator;

        rg::HeapResource<rg::Texture> texture;

        std::unordered_map<Slot, PendingUpload> pendingUploads;

        Slot nextSlot = 0;
        std::vector<Slot> freeSlots;
    };
}

#endif //XENGINE_STREAMTEXTURE_HPP
