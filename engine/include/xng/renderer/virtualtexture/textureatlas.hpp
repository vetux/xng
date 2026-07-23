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

        class UploadCallbackHandler {
        public:
            virtual ~UploadCallbackHandler() = default;

            virtual void onUploadComplete(Slot slot) = 0;
        };

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
         * @param maxTileCopies The maximum number of tiles to copy per frame (Flushed tiles can exceed this value)
         * @param maxTilesInFlight The maximum expected number of tile uploads in flight.
         */
        TextureAtlas(rg::Runtime &runtime,
                     ChunkStreamer &chunkStreamer,
                     const unsigned int tileSize,
                     const unsigned int tileBorder,
                     const float maxAnisotropy,
                     const unsigned int maxTileCopies = 128,
                     const unsigned int maxTilesInFlight = 128)
            : runtime(runtime),
              tileSize(tileSize),
              tileBorder(tileBorder),
              atlasTileSize(tileSize + 2 * tileBorder),
              atlasTileBytes((atlasTileSize * atlasTileSize) * 4),
              maxTileCopies(maxTileCopies),
              uploadBuffer(runtime.getResourceHeap(), chunkStreamer, maxTilesInFlight, atlasTileBytes) {
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

        Slot create(std::vector<uint8_t> texels,
                    const int priority,
                    UploadCallbackHandler &callback) {
            const auto slot = slotAllocator.allocate(1);
            pendingUploads.emplace(slot, TileUpload(slot,
                                                    atlasTileBytes,
                                                    uploadBuffer,
                                                    getOffset(slot),
                                                    std::move(texels),
                                                    callback));
            pendingUploadPriorities.emplace(slot, priority);
            pendingUploadQueues[priority].insert(slot);
            tilesInFlight++;
            return slot;
        }

        void destroy(const Slot slot) {
            if (pendingUploads.find(slot) != pendingUploads.end()) {
                tilesInFlight--;
            }
            slotAllocator.free(slot, 1);
            pendingUploads.erase(slot);
            pendingUploadPriorities.erase(slot);
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
                return it->second.isUploadComplete();
            }
            return true;
        }

        void commit(RenderQueue &queue) {
            staleTexture = {};

            const auto totalOffset = getOffset(slotAllocator.getSize());
            if (texture.getDescription().arrayLayers <= totalOffset.z) {
                staleTexture = texture;
                auto desc = texture.getDescription();
                desc.arrayLayers = totalOffset.z + 1;
                texture = runtime.getResourceHeap().allocateTexture(desc);
                queue.addPreFrame(rg::GraphicsPassBuilder("TextureAtlas/Resize")
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

            for (auto &slot: copiedUploads) {
                pendingUploads.erase(slot);
                flushedUploads.erase(slot);
                pendingUploadQueues.at(pendingUploadPriorities.at(slot)).erase(slot);
                pendingUploadPriorities.erase(slot);
                tilesInFlight--;
            }
            copiedUploads.clear();

            std::unordered_set<Slot> frameCopies;
            for (auto slot: flushedUploads) {
                auto &upload = pendingUploads.at(slot);
                upload.flush();
                frameCopies.insert(slot);
            }

            for (auto &pair: pendingUploadQueues) {
                for (auto slot: pair.second) {
                    if (frameCopies.size() >= maxTileCopies) {
                        break;
                    }
                    const auto &upload = pendingUploads.at(slot);
                    if (upload.isUploadComplete()) {
                        frameCopies.insert(slot);
                    }
                }
                if (frameCopies.size() >= maxTileCopies) {
                    break;
                }
            }

            uploadBuffer.buffer.commit(queue);

            auto pass = rg::GraphicsPassBuilder("TextureAtlas/Copy");

            for (auto slot: frameCopies) {
                const auto &upload = pendingUploads.at(slot);
                pass.transferRead(uploadBuffer.buffer.getBuffer(),
                                  upload.uploadBufferSlot * atlasTileBytes,
                                  atlasTileBytes);
                pass.transferWrite(texture, rg::TextureBinding::Range(0, 1, upload.offset.z, 1));

                upload.callback.onUploadComplete(slot);

                copiedUploads.insert(slot);
                pendingUploadQueues.at(pendingUploadPriorities.at(slot)).erase(slot);
            }

            queue.addFrame(pass.execute([this, frameCopies](rg::RasterContext &,
                                                            rg::TransferContext &ctx,
                                                            rg::ComputeContext &) {
                for (auto slot: frameCopies) {
                    const auto &upload = pendingUploads.at(slot);
                    ctx.copyBufferToTexture(texture,
                                            uploadBuffer.buffer.getBuffer(),
                                            rg::Texture::SubResource(0, upload.offset.z, rg::FACE_UNDEFINED),
                                            upload.uploadBufferSlot * atlasTileBytes,
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

        size_t getTilesInFlight() const {
            return tilesInFlight;
        }

    private:
        // TODO: Handle upload buffer allocator fragmentation
        struct UploadBuffer {
            StreamBuffer buffer;
            RangeAllocator allocator;

            UploadBuffer(rg::Heap &heap,
                         ChunkStreamer &chunkStreamer,
                         const unsigned int maxTilesInFlight,
                         const size_t atlasTileBytes)
                : buffer(heap,
                         chunkStreamer,
                         rg::Buffer::CAPABILITY_TRANSFER_SRC,
                         maxTilesInFlight * atlasTileBytes) {
            }

            UploadBuffer(const UploadBuffer &) = delete;

            UploadBuffer &operator=(const UploadBuffer &) = delete;
        };

        struct TileUpload {
            Slot slot;
            Vec3u offset;

            UploadBuffer &uploadBuffer;
            StreamBuffer::Handle uploadBufferHandle{};
            size_t uploadBufferSlot{};

            UploadCallbackHandler &callback;

            TileUpload(const Slot slot,
                       const size_t atlasTileBytes,
                       UploadBuffer &uploadBuffer,
                       Vec3u offset,
                       std::vector<uint8_t> texels,
                       UploadCallbackHandler &callback)
                : slot(slot),
                  offset(std::move(offset)),
                  uploadBuffer(uploadBuffer),
                  callback(callback) {
                uploadBufferSlot = uploadBuffer.allocator.allocate(1);
                uploadBufferHandle = uploadBuffer.buffer.upload(std::move(texels), uploadBufferSlot * atlasTileBytes);
            }

            ~TileUpload() {
                if (uploadBufferHandle != StreamBuffer::INVALID_HANDLE) {
                    uploadBuffer.buffer.release(uploadBufferHandle);
                    uploadBuffer.allocator.free(uploadBufferSlot, 1);
                }
            }

            TileUpload(const TileUpload &) = delete;

            TileUpload &operator=(const TileUpload &) = delete;

            TileUpload(TileUpload &&other) noexcept
                : slot(other.slot),
                  offset(std::move(other.offset)),
                  uploadBuffer(other.uploadBuffer),
                  uploadBufferHandle(other.uploadBufferHandle),
                  uploadBufferSlot(other.uploadBufferSlot),
                  callback(other.callback) {
                other.uploadBufferHandle = StreamBuffer::INVALID_HANDLE;
            }

            bool isUploadComplete() const {
                return uploadBuffer.buffer.isUploadComplete(uploadBufferHandle);
            }

            void flush() {
                uploadBuffer.buffer.flush(uploadBufferHandle);
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

        const unsigned int tileSize;
        const unsigned int tileBorder;
        const unsigned int atlasTileSize;

        const unsigned int atlasTileBytes;

        const unsigned int maxTileCopies;

        UploadBuffer uploadBuffer;

        unsigned int tilesInFlight = 0;

        Vec2u atlasTiles;
        unsigned int tilesPerLayer;

        rg::HeapResource<rg::Texture> staleTexture;
        rg::HeapResource<rg::Texture> texture;

        std::unordered_map<Slot, TileUpload> pendingUploads;
        std::unordered_map<Slot, int> pendingUploadPriorities;
        std::unordered_set<Slot> flushedUploads;

        std::unordered_set<Slot> copiedUploads;

        std::unordered_map<int, std::unordered_set<Slot> > pendingUploadQueues;

        RangeAllocator slotAllocator;
    };
}
#endif //XENGINE_TEXTUREATLAS_HPP
