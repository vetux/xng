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

#ifndef XENGINE_TEXTUREATLAS_HPP
#define XENGINE_TEXTUREATLAS_HPP

#include <utility>

#include "xng/rendergraph/runtime.hpp"

#include "xng/renderer/stream/streambuffer.hpp"

namespace xng {
    class TextureAtlas {
    public:
        typedef unsigned int Slot;

        TextureAtlas(rg::Runtime &runtime,
                     ChunkStreamer &chunkStreamer,
                     const unsigned int tileSize,
                     const unsigned int tileBorder,
                     const float maxAnisotropy)
            : runtime(runtime),
              chunkStreamer(chunkStreamer),
              tileSize(tileSize),
              tileBorder(tileBorder),
              atlasTileSize(tileSize + 2 * tileBorder) {
            const auto limits = runtime.getTextureFormatLimits(rg::TEXTURE_2D,
                                                               rg::RGBA8,
                                                               rg::Texture::CAPABILITY_SAMPLED |
                                                               rg::Texture::CAPABILITY_TRANSFER_DST);

            const auto remainder = Vec2u(limits.maxExtent.x % atlasTileSize, limits.maxExtent.y % atlasTileSize);
            const auto textureSize = limits.maxExtent - remainder;

            atlasTiles = textureSize / Vec2u(atlasTileSize, atlasTileSize);
            tilesPerLayer = atlasTiles.x * atlasTiles.y;

            rg::Texture desc;
            desc.capabilities = rg::Texture::CAPABILITY_SAMPLED | rg::Texture::CAPABILITY_TRANSFER_DST;
            desc.size = textureSize;
            desc.textureType = rg::TEXTURE_2D_ARRAY;
            desc.format = rg::RGBA8;
            desc.filterMin = rg::LINEAR;
            desc.filterMag = rg::LINEAR;
            desc.arrayLayers = 1;
            desc.maxAnisotropy = maxAnisotropy;

            texture = runtime.getResourceHeap().allocateTexture(desc);
        }

        Slot create(const std::vector<uint8_t> &texels) {
            const auto slot = slotAllocator.allocate(1);
            pendingUploads.emplace(slot, PendingUpload(slot,
                                                       getOffset(slot),
                                                       runtime.getResourceHeap(),
                                                       chunkStreamer,
                                                       texels));
            return slot;
        }

        void destroy(const Slot slot) {
            slotAllocator.free(slot, 1);
            pendingUploads.erase(slot);
        }

        void flush(const Slot handle) {
            auto it = pendingUploads.find(handle);
            if (it != pendingUploads.end()) {
                it->second.buffer.flush(it->second.handle);
            }
        }

        bool isUploadComplete(const Slot handle) {
            auto it = pendingUploads.find(handle);
            if (it != pendingUploads.end()) {
                return it->second.buffer.isUploadComplete(it->second.handle);
            }
            return true;
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            std::vector<rg::TransferPass> ret;

            const auto totalOffset = getOffset(slotAllocator.getSize());
            if (texture.getDescription().arrayLayers <= totalOffset.z) {
                auto staleTexture = texture;
                auto desc = texture.getDescription();
                desc.arrayLayers = totalOffset.z + 1;
                texture = runtime.getResourceHeap().allocateTexture(desc);
                ret.emplace_back(rg::TransferPassBuilder("TextureAtlas/Copy")
                    .read(staleTexture, rg::TextureBinding::Range(0, 1, 0, staleTexture.getDescription().arrayLayers))
                    .write(texture, rg::TextureBinding::Range(0, 1, 0, staleTexture.getDescription().arrayLayers))
                    .execute([this, staleTexture](rg::TransferContext &ctx) {
                        rg::TransferContext::TextureCopyRegion region;
                        region.src = rg::Texture::SubResource(0);
                        region.dst = rg::Texture::SubResource(0);
                        region.size = staleTexture.getDescription().size;
                        region.srcOffset = {};
                        region.dstOffset = {};
                        region.depth = staleTexture.getDescription().arrayLayers;
                        ctx.copyTexture(texture, staleTexture, {region});
                    }));
            }

            std::unordered_set<Slot> prunedUploads;
            for (auto &pair: pendingUploads) {
                if (pair.second.submitted) {
                    prunedUploads.insert(pair.first);
                    continue;
                }

                auto passes = pair.second.buffer.commit(graph);
                ret.insert(ret.end(), passes.begin(), passes.end());
                if (pair.second.buffer.isUploadComplete(pair.second.handle)) {
                    const auto &upload = pair.second;
                    ret.emplace_back(rg::TransferPassBuilder("TextureAtlas/Copy")
                        .read(upload.buffer.getBuffer(), 0, upload.buffer.getBuffer().getDescription().size)
                        .write(texture, rg::TextureBinding::Range(0, 1, upload.offset.z, 1))
                        .execute([this, upload](rg::TransferContext &ctx) {
                            ctx.copyBufferToTexture(texture, upload.buffer.getBuffer(),
                                                    rg::Texture::SubResource(upload.offset.z),
                                                    0,
                                                    Rectu(Vec2u(upload.offset.x, upload.offset.y),
                                                          Vec2u(atlasTileSize, atlasTileSize)),
                                                    rg::RGBA8);
                        }));
                    pair.second.submitted = true;
                }
            }

            for (auto slot: prunedUploads) {
                pendingUploads.erase(slot);
            }

            return ret;
        }

        rg::HeapResource<rg::Texture> getTexture() const {
            return texture;
        }

        unsigned int getTileSize() const {
            return tileSize;
        }

        unsigned int getTileBorder() const {
            return tileBorder;
        }

    private:
        //TODO: Use single stream buffer for all uploads
        struct PendingUpload {
            Slot slot;
            Vec3u offset;
            StreamBuffer buffer;
            StreamBuffer::Handle handle;
            bool submitted = false;

            PendingUpload(const Slot slot,
                          Vec3u offset,
                          rg::Heap &heap,
                          ChunkStreamer &chunkStreamer,
                          const std::vector<uint8_t> &texels)
                : slot(slot), offset(std::move(offset)),
                  buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_TRANSFER_SRC) {
                handle = buffer.upload(texels, 0);
            }
        };

        Vec3u getOffset(const Slot slot) const {
            Vec3u ret;
            ret.z = slot / tilesPerLayer;
            const auto localSlot = slot - ret.z * tilesPerLayer;
            ret.x = localSlot % atlasTiles.x * atlasTileSize;
            ret.y = localSlot / atlasTiles.x * atlasTileSize;
            return ret;
        }

        rg::Runtime &runtime;

        ChunkStreamer &chunkStreamer;

        const unsigned int tileSize;
        const unsigned int tileBorder;
        const unsigned int atlasTileSize;

        Vec2u atlasTiles;
        unsigned int tilesPerLayer;

        rg::HeapResource<rg::Texture> texture;

        std::unordered_map<Slot, PendingUpload> pendingUploads;

        RangeAllocator slotAllocator;
    };
}
#endif //XENGINE_TEXTUREATLAS_HPP
