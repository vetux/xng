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

        /**
         * The ChunkStreamer streaming budget defines the PCIe bandwidth limit.
         * Additionally, the max tiles in flight represent a VRAM usage limit for bounding the tile staging buffer and
         * the gpu side copy overhead.
         *
         * @param runtime
         * @param chunkStreamer
         * @param tileSize
         * @param tileBorder
         * @param maxAnisotropy
         * @param maxTilesInFlight The max tiles in flight per frame (Flushed tiles can exceed this value)
         */
        TextureAtlas(rg::Runtime &runtime,
                     ChunkStreamer &chunkStreamer,
                     const unsigned int tileSize,
                     const unsigned int tileBorder,
                     const float maxAnisotropy,
                     const unsigned int maxTilesInFlight = 128)
            : runtime(runtime),
              buffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_TRANSFER_SRC),
              tileSize(tileSize),
              tileBorder(tileBorder),
              atlasTileSize(tileSize + 2 * tileBorder),
              atlasTileBytes((atlasTileSize * atlasTileSize) * 4),
              maxTilesInFlight(maxTilesInFlight) {
            buffer.setTargetSize(maxTilesInFlight * atlasTileBytes);

            const auto limits = runtime.getTextureFormatLimits(rg::TEXTURE_2D_ARRAY,
                                                               rg::RGBA8,
                                                               rg::Texture::CAPABILITY_SAMPLED |
                                                               rg::Texture::CAPABILITY_TRANSFER_SRC |
                                                               rg::Texture::CAPABILITY_TRANSFER_DST);

            const auto remainder = Vec2u(limits.maxExtent.x % atlasTileSize, limits.maxExtent.y % atlasTileSize);
            const auto textureSize = limits.maxExtent - remainder;

            atlasTiles = textureSize / Vec2u(atlasTileSize, atlasTileSize);
            tilesPerLayer = atlasTiles.x * atlasTiles.y;

            rg::Texture desc;
            desc.capabilities = rg::Texture::CAPABILITY_SAMPLED
                                | rg::Texture::CAPABILITY_TRANSFER_SRC
                                | rg::Texture::CAPABILITY_TRANSFER_DST;
            desc.size = textureSize;
            desc.textureType = rg::TEXTURE_2D_ARRAY;
            desc.format = rg::RGBA8;
            desc.filterMin = rg::LINEAR;
            desc.filterMag = rg::LINEAR;
            desc.arrayLayers = 1;
            desc.maxAnisotropy = maxAnisotropy;

            texture = runtime.getResourceHeap().allocateTexture(desc);
        }

        Slot create(const std::shared_ptr<std::vector<uint8_t> > &texels, const int priority) {
            const auto slot = slotAllocator.allocate(1);
            pendingUploads.emplace(slot, PendingUpload(slot, getOffset(slot), texels));
            pendingUploadPriorities.emplace(slot, priority);
            pendingUploadQueues[priority].insert(slot);
            return slot;
        }

        void destroy(const Slot slot) {
            slotAllocator.free(slot, 1);
            auto it = pendingUploads.find(slot);
            if (it != pendingUploads.end()) {
                if (it->second.startedUpload) {
                    buffer.release(it->second.bufferHandle);
                    freeBufferSlot(it->second.bufferSlot);
                    tilesInFlight--;
                }
                pendingUploadQueues[pendingUploadPriorities.at(slot)].erase(slot);
                copiedUploadQueues[pendingUploadPriorities.at(slot)].erase(slot);
                pendingUploadPriorities.erase(slot);
            }
            pendingUploads.erase(slot);
            flushedUploads.erase(slot);
        }

        void flush(const Slot handle) {
            if (pendingUploads.find(handle) != pendingUploads.end()) {
                flushedUploads.insert(handle);
            }
        }

        bool isUploadComplete(const Slot handle) {
            auto it = pendingUploads.find(handle);
            if (it != pendingUploads.end()) {
                if (it->second.startedUpload) {
                    return buffer.isUploadComplete(it->second.bufferHandle);
                }
                return false;
            }
            return true;
        }

        void commit(rg::GraphBuilder &graph, StreamerQueue &streamerQueue) {
            staleTexture = {};

            const auto totalOffset = getOffset(slotAllocator.getSize());
            if (texture.getDescription().arrayLayers <= totalOffset.z) {
                staleTexture = texture;
                auto desc = texture.getDescription();
                desc.arrayLayers = totalOffset.z + 1;
                texture = runtime.getResourceHeap().allocateTexture(desc);
                graph.addPass(rg::GraphicsPassBuilder("TextureAtlas/Copy")
                    .transferRead(staleTexture,
                                  rg::TextureBinding::Range(0, 1, 0, staleTexture.getDescription().arrayLayers))
                    .transferWrite(
                        texture, rg::TextureBinding::Range(0, 1, 0, staleTexture.getDescription().arrayLayers))
                    .execute([this](rg::RasterContext &,
                                    rg::TransferContext &ctx,
                                    rg::ComputeContext &) {
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

            std::unordered_set<Slot> frameCopies;
            std::unordered_set<Slot> finishedUploads;

            for (auto &pair: copiedUploadQueues) {
                for (auto slot: pair.second) {
                    auto &upload = pendingUploads.at(slot);
                    freeBufferSlot(upload.bufferSlot);
                    buffer.release(upload.bufferHandle);
                    finishedUploads.insert(slot);
                }
            }

            for (auto slot: finishedUploads) {
                pendingUploads.erase(slot);
                flushedUploads.erase(slot);
                copiedUploadQueues.at(pendingUploadPriorities.at(slot)).erase(slot);
                pendingUploadPriorities.erase(slot);
                tilesInFlight--;
            }

            for (auto slot: flushedUploads) {
                auto &upload = pendingUploads.at(slot);
                if (upload.flushed)
                    continue;
                if (!upload.startedUpload) {
                    upload.bufferSlot = allocateBufferSlot();
                    upload.bufferHandle = buffer.upload(*upload.texels, upload.bufferSlot * atlasTileBytes);
                    upload.startedUpload = true;
                    buffer.flush(upload.bufferHandle);
                    tilesInFlight++;
                } else {
                    buffer.flush(upload.bufferHandle);
                }
                upload.flushed = true;
            }

            for (auto &pair: pendingUploadQueues) {
                for (auto slot: pair.second) {
                    auto &upload = pendingUploads.at(slot);
                    if (!upload.startedUpload && tilesInFlight < maxTilesInFlight) {
                        upload.bufferSlot = allocateBufferSlot();
                        assert(upload.texels->size() == atlasTileBytes);
                        upload.bufferHandle = buffer.upload(*upload.texels,
                                                            upload.bufferSlot * atlasTileBytes);
                        upload.startedUpload = true;
                        tilesInFlight++;
                    }

                    if ((upload.startedUpload && buffer.isUploadComplete(upload.bufferHandle))
                        || flushedUploads.find(slot) != flushedUploads.end()) {
                        frameCopies.insert(slot);
                    }
                }
            }

            buffer.commit(streamerQueue);

            auto pass = rg::GraphicsPassBuilder("TextureAtlas/Copy");

            for (auto slot: frameCopies) {
                const auto &upload = pendingUploads.at(slot);
                pass.transferRead(buffer.getBuffer(),
                                  upload.bufferSlot * atlasTileBytes,
                                  atlasTileBytes);
                pass.transferWrite(texture, rg::TextureBinding::Range(0, 1, upload.offset.z, 1));

                copiedUploadQueues[pendingUploadPriorities.at(slot)].insert(slot);
                pendingUploadQueues.at(pendingUploadPriorities.at(slot)).erase(slot);
            }

            graph.addPass(pass.execute([this, frameCopies](rg::RasterContext &,
                                                           rg::TransferContext &ctx,
                                                           rg::ComputeContext &) {
                for (auto slot: frameCopies) {
                    const auto &upload = pendingUploads.at(slot);
                    ctx.copyBufferToTexture(texture,
                                            buffer.getBuffer(),
                                            rg::Texture::SubResource(0, upload.offset.z, rg::FACE_UNDEFINED),
                                            upload.bufferSlot * atlasTileBytes,
                                            Rectu(Vec2u(upload.offset.x, upload.offset.y),
                                                  Vec2u(atlasTileSize, atlasTileSize)),
                                            rg::RGBA8);
                }
            }));
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
        struct PendingUpload {
            Slot slot;
            Vec3u offset;

            std::shared_ptr<std::vector<uint8_t> > texels;

            StreamBuffer::Handle bufferHandle{};
            size_t bufferSlot{};

            bool startedUpload = false;
            bool flushed = false;

            PendingUpload(const Slot slot,
                          Vec3u offset,
                          std::shared_ptr<std::vector<uint8_t> > texels)
                : slot(slot),
                  offset(std::move(offset)),
                  texels(std::move(texels)) {
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

        size_t allocateBufferSlot() {
            if (freeBufferSlots.empty()) {
                return currentBufferSlot++;
            }
            const auto ret = freeBufferSlots.back();
            freeBufferSlots.pop_back();
            return ret;
        }

        void freeBufferSlot(const size_t slot) {
            if (slot >= maxTilesInFlight)
                return;
            freeBufferSlots.push_back(slot);
        }

        rg::Runtime &runtime;

        StreamBuffer buffer;
        size_t currentBufferSlot = 0;
        std::vector<size_t> freeBufferSlots;

        const unsigned int tileSize;
        const unsigned int tileBorder;
        const unsigned int atlasTileSize;

        const unsigned int atlasTileBytes;

        const unsigned int maxTilesInFlight;
        unsigned int tilesInFlight = 0;

        Vec2u atlasTiles;
        unsigned int tilesPerLayer;

        rg::HeapResource<rg::Texture> staleTexture;
        rg::HeapResource<rg::Texture> texture;

        std::map<int, std::unordered_set<Slot> > pendingUploadQueues;
        std::map<int, std::unordered_set<Slot> > copiedUploadQueues;

        std::unordered_map<Slot, PendingUpload> pendingUploads;
        std::unordered_map<Slot, int> pendingUploadPriorities;
        std::unordered_set<Slot> flushedUploads;

        RangeAllocator slotAllocator;
    };
}
#endif //XENGINE_TEXTUREATLAS_HPP
