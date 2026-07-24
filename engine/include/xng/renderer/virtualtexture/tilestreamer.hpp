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

#ifndef XENGINE_TILESTREAMER_HPP
#define XENGINE_TILESTREAMER_HPP

#include <utility>
#include <vector>
#include <cstdint>
#include <cstring>

#include "xng/assets/image.hpp"
#include "xng/async/task.hpp"
#include "xng/async/threadpool.hpp"
#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

#include "xng/renderer/stream/streambuffer.hpp"
#include "xng/renderer/virtualtexture/textureatlas.hpp"
#include "xng/renderer/virtualtexture/tileloader.hpp"

namespace xng {
    /**
     * The tile streamer manages the mapping buffers and streams tiles into the atlas texture.
     */
    class TileStreamer final : public TextureAtlas::UploadCallbackHandler {
    public:
        typedef unsigned int TextureID;

        static Vec2u getTiles(const Vec2u &imageRes, const unsigned int tileSize) {
            return {ceildiv(imageRes.x, tileSize), ceildiv(imageRes.y, tileSize)};
        }

        static unsigned int tileToIndex(const Vec2u &tile, const Vec2u &tileCount) {
            return tile.y * tileCount.x + tile.x;
        }

        static Vec2u indexToTile(const unsigned int index, const Vec2u &tileCount) {
            return {index % tileCount.x, index / tileCount.x};
        }

        enum TileState {
            TILE_EVICTED = 0,
            TILE_PENDING,
            TILE_RESIDENT
        };

        struct VirtualTextureState {
            Vec2u size;
            Vec2u tileCount;

            std::vector<TileState> tileStates;
            std::vector<TextureAtlas::Slot> atlasSlots;

            size_t tileMapOffset{}; // For Readback

            VirtualTextureState(Vec2u size, const Vec2u &tileCount, const size_t tileMapOffset)
                : size(std::move(size)),
                  tileCount(tileCount),
                  tileStates(tileCount.x * tileCount.y, TILE_EVICTED),
                  atlasSlots(tileCount.x * tileCount.y),
                  tileMapOffset(tileMapOffset) {
            }

            void setTileState(const Vec2u &tile, const TileState state) {
                const auto index = tileToIndex(tile, tileCount);
                assert(!(tileStates.at(index) == TILE_RESIDENT && state == TILE_PENDING));
                assert(!(tileStates.at(index) == TILE_EVICTED && state == TILE_RESIDENT));
                tileStates.at(index) = state;
            }

            [[nodiscard]] TileState getTileState(const Vec2u &tile) const {
                return tileStates.at(tileToIndex(tile, tileCount));
            }

            [[nodiscard]] TextureAtlas::Slot getAtlasSlot(const Vec2u &tile) const {
                return atlasSlots.at(tileToIndex(tile, tileCount));
            }

            void setAtlasSlot(const Vec2u &tile, const TextureAtlas::Slot slot) {
                this->atlasSlots.at(tileToIndex(tile, tileCount)) = slot;
            }
        };

        TileStreamer(rg::Runtime &runtime,
                     ChunkStreamer &chunkStreamer,
                     TextureAtlas &atlas,
                     ThreadPool &pool,
                     const unsigned int tileSize)
            : runtime(runtime),
              atlas(atlas),
              pool(pool),
              tileSize(tileSize),
              tileMapOffsetsBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              tileMapBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              residencyMapOffsetsBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              residencyMapBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE) {
            readbackBuffer = runtime.getResourceHeap().allocateBuffer(rg::Buffer(1,
                rg::Buffer::CAPABILITY_STORAGE,
                rg::Buffer::MEMORY_GPU_ONLY));
            readbackHostBuffer = runtime.getResourceHeap().allocateBuffer(rg::Buffer(1,
                rg::Buffer::CAPABILITY_STORAGE,
                rg::Buffer::MEMORY_GPU_TO_CPU));
            readbackClearBuffer = runtime.getResourceHeap().allocateBuffer(rg::Buffer(1,
                rg::Buffer::CAPABILITY_STORAGE,
                rg::Buffer::MEMORY_CPU_TO_GPU));
        }

        ~TileStreamer() override = default;

        TileStreamer(const TileStreamer &) = delete;

        TileStreamer &operator=(const TileStreamer &) = delete;

        TileStreamer(TileStreamer &&) = default;

        TileStreamer &operator=(TileStreamer &&) = delete;

        TextureID create(const Vec2u &imageSize, const unsigned int mipLevels) {
            if (mipLevels == 0) {
                throw std::runtime_error("Mip levels must be greater than 0.");
            }

            const TextureID ret = textureIDAllocator.allocate(mipLevels);

            std::vector<VirtualTextureState> states;
            states.reserve(mipLevels);

            std::vector<unsigned int> tileMapOffsets;
            tileMapOffsets.reserve(mipLevels);

            std::vector<size_t> tileMapSizes;
            tileMapSizes.reserve(mipLevels);

            for (auto mip = 0; mip < mipLevels; mip++) {
                const auto mipSize = rg::Texture::getMipLevelSize(imageSize, mip);
                const auto mipTiles = getTiles(mipSize, tileSize);
                const auto tileMapSize = mipTiles.x * mipTiles.y;
                const auto tileMapOffset = static_cast<unsigned int>(tileMapAllocator.allocate(tileMapSize));
                tileMapOffsets.emplace_back(tileMapOffset);
                tileMapSizes.emplace_back(tileMapSize);
                states.emplace_back(mipSize, mipTiles, tileMapOffset);
            }

            const Vec2u mip0Tiles = getTiles(imageSize, tileSize);
            const auto residencyMapSize = mip0Tiles.x * mip0Tiles.y;
            const auto residencyMapOffset = static_cast<unsigned int>(residencyMapAllocator.allocate(residencyMapSize));

            textures.emplace(ret, std::move(VirtualTexture(ret,
                                                           imageSize,
                                                           mipLevels,
                                                           states,
                                                           tileSize,
                                                           tileMapBuffer,
                                                           tileMapOffsetsBuffer,
                                                           tileMapOffsets,
                                                           tileMapSizes,
                                                           residencyMapBuffer,
                                                           residencyMapOffsetsBuffer,
                                                           residencyMapOffset,
                                                           residencyMapSize)));

            return ret;
        }

        void destroy(const TextureID textureID) {
            const auto &texture = textures.at(textureID);

            // Free allocators
            textureIDAllocator.free(textureID, texture.states.size());
            for (auto i = 0; i < texture.tileMapOffsetsValues.size(); i++) {
                tileMapAllocator.free(texture.tileMapOffsetsValues.at(i), texture.tileMapSizes.at(i));
            }
            residencyMapAllocator.free(texture.residencyMapOffsetValue, texture.residencyMapSize);

            // Destroy allocation
            for (auto &state: texture.states) {
                for (auto i = 0; i < state.tileStates.size(); i++) {
                    switch (state.tileStates.at(i)) {
                        case TILE_PENDING: {
                            const auto slot = state.atlasSlots.at(i);
                            atlas.destroy(slot);
                            pendingTiles.erase(slot);
                            break;
                        }
                        case TILE_RESIDENT: {
                            atlas.destroy(state.atlasSlots.at(i));
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            textures.erase(textureID);
            tileAllocations.erase(textureID);
        }

        void uploadTile(const TextureID textureID,
                        const unsigned int mip,
                        const Vec2u &tile,
                        std::vector<uint8_t> texels,
                        const int priority) {
            if (mip >= textures.at(textureID).tileMapOffsetsValues.size()) {
                throw std::runtime_error("Mip level out of range.");
            }

            auto &state = textures.at(textureID).states.at(mip);
            if (state.getTileState(tile) != TILE_EVICTED) {
                throw std::runtime_error("Tile already in flight");
            }

            const auto texSize = textures.at(textureID).imageSize;
            const auto mipSize = rg::Texture::getMipLevelSize(texSize, mip);
            const auto mipTiles = getTiles(mipSize, tileSize);

            const auto atlasSlot = atlas.create(std::move(texels),
                                                priority,
                                                *this);

            state.setTileState(tile, TILE_PENDING);
            state.setAtlasSlot(tile, atlasSlot);

            tileAllocations[textureID][mip].emplace(tile,
                                                    TileAllocation(atlas,
                                                                   atlasSlot));
            pendingTiles[atlasSlot] = TileID{textureID, mip, tile};
        }

        void evictTile(const TextureID textureID,
                       const unsigned int mip,
                       const Vec2u &tile) {
            auto &state = textures.at(textureID).states.at(mip);
            if (state.getTileState(tile) == TILE_EVICTED) {
                throw std::runtime_error("Tile already evicted");
            }
            switch (state.getTileState(tile)) {
                case TILE_PENDING: {
                    const auto slot = state.getAtlasSlot(tile);
                    atlas.destroy(slot);
                    pendingTiles.erase(slot);
                    break;
                }
                case TILE_RESIDENT: {
                    atlas.destroy(state.getAtlasSlot(tile));
                    updatedMips[textureID].insert(mip);
                    break;
                }
                default:
                    break;
            }
            state.setTileState(tile, TILE_EVICTED);
            state.setAtlasSlot(tile, TextureAtlas::Slot());
            tileAllocations[textureID][mip].erase(tile);
        }

        void flushTile(const TextureID textureID,
                       const unsigned int mip,
                       const Vec2u &tile) const {
            auto &state = textures.at(textureID).states.at(mip);
            if (state.getTileState(tile) == TILE_PENDING) {
                tileAllocations.at(textureID).at(mip).at(tile).flush();
            }
        }

        const std::vector<VirtualTextureState> &getTextureState(const TextureID textureID) {
            return textures.at(textureID).states;
        }

        size_t getTilesInFlight() const {
            return atlas.getTilesInFlight();
        }

        void commit(RenderQueue &queue) {
            for (const auto &pair: updatedMips) {
                auto &texture = textures.at(pair.first);
                // Update TileMap
                for (const auto mip: pair.second) {
                    texture.updateTileMap(mip);
                }
                texture.updateResidencyMap();
            }
            updatedMips.clear();

            tileMapOffsetsBuffer.commit(queue);
            tileMapBuffer.commit(queue);
            residencyMapOffsetsBuffer.commit(queue);
            residencyMapBuffer.commit(queue);

            if (readbackBuffer.getDescription().size != tileMapBuffer.getBuffer().getDescription().size) {
                auto desc = readbackBuffer.getDescription();
                desc.size = tileMapBuffer.getBuffer().getDescription().size;
                readbackBuffer = runtime.getResourceHeap().allocateBuffer(desc);

                desc = readbackHostBuffer.getDescription();
                desc.size = tileMapBuffer.getBuffer().getDescription().size;
                readbackHostBuffer = runtime.getResourceHeap().allocateBuffer(desc);

                desc = readbackClearBuffer.getDescription();
                desc.size = tileMapBuffer.getBuffer().getDescription().size;
                readbackClearBuffer = runtime.getResourceHeap().allocateBuffer(desc);
                {
                    const auto mapping = runtime.getResourceHeap().map(readbackClearBuffer);
                    std::memset(mapping->data(), 0, mapping->size());
                }
            }

            queue.addPreFrame(rg::GraphicsPassBuilder("TileStreamer/ClearReadback")
                .storageWrite(readbackBuffer, {})
                .storageRead(readbackClearBuffer, {})
                .execute([this](rg::RasterContext &, rg::TransferContext &ctx, rg::ComputeContext &) {
                    ctx.copyBuffer(readbackBuffer, readbackClearBuffer, 0, 0, readbackBuffer.getDescription().size);
                }));
        }

        rg::HeapResource<rg::Buffer> getTileMapOffsetsBuffer() const {
            return tileMapOffsetsBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getTileMapBuffer() const {
            return tileMapBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getResidencyMapOffsetsBuffer() const {
            return residencyMapOffsetsBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getResidencyMapBuffer() const {
            return residencyMapBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getReadbackBuffer() const {
            return readbackBuffer;
        }

        rg::HeapResource<rg::Buffer> getReadbackHostBuffer() const {
            return readbackHostBuffer;
        }

    private:
        struct TileID {
            TextureID texture{};
            unsigned int mip{};
            Vec2u tile{};
        };

        static constexpr unsigned int ceildiv(const unsigned int a, const unsigned int b) {
            return (a + b - 1) / b;
        }

        static std::vector<unsigned int> getResidencyMap(const std::vector<VirtualTextureState> &textureStates,
                                                         const unsigned int tileSize) {
            const auto mipLevels = static_cast<unsigned int>(textureStates.size());
            const auto tiles0 = textureStates.at(0).tileCount;
            const auto size0 = textureStates.at(0).size;

            std::vector ret(tiles0.x * tiles0.y, mipLevels - 1);

            for (unsigned int ty = 0; ty < tiles0.y; ++ty) {
                for (unsigned int tx = 0; tx < tiles0.x; ++tx) {
                    // mip0 texel span of this tile, clamped to the image (last tile is partial)
                    const unsigned int x0 = tx * tileSize;
                    const unsigned int y0 = ty * tileSize;
                    const unsigned int x1 = std::min(x0 + tileSize, size0.x) - 1; // inclusive
                    const unsigned int y1 = std::min(y0 + tileSize, size0.y) - 1;

                    unsigned int finest = mipLevels - 1;
                    for (int mip = static_cast<int>(mipLevels) - 1; mip >= 0; --mip) {
                        const auto &state = textureStates.at(mip);

                        // mip0 texels -> mip-m tile range (a range, because of straddle)
                        const unsigned int mtx0 = (x0 >> mip) / tileSize;
                        const unsigned int mtx1 = (x1 >> mip) / tileSize;
                        const unsigned int mty0 = (y0 >> mip) / tileSize;
                        const unsigned int mty1 = (y1 >> mip) / tileSize;

                        bool allResident = true;
                        for (unsigned int my = mty0; my <= mty1 && allResident; ++my) {
                            for (unsigned int mx = mtx0; mx <= mtx1; ++mx) {
                                const Vec2u t{
                                    std::min(mx, state.tileCount.x - 1),
                                    std::min(my, state.tileCount.y - 1)
                                };
                                if (state.getTileState(t) != TILE_RESIDENT) {
                                    allResident = false;
                                    break;
                                }
                            }
                        }
                        if (!allResident) break; // chain broken here, can't go finer
                        finest = static_cast<unsigned int>(mip);
                    }
                    ret[tileToIndex({tx, ty}, tiles0)] = finest;
                }
            }
            return ret;
        }

        struct TileAllocation {
            TextureAtlas &atlas;
            TextureAtlas::Slot atlasSlot;

            TileAllocation(TextureAtlas &atlas, const TextureAtlas::Slot atlasSlot)
                : atlas(atlas), atlasSlot(atlasSlot) {
            }

            TileAllocation(const TileAllocation &) = delete;

            TileAllocation &operator=(const TileAllocation &) = delete;

            TileAllocation(TileAllocation &&) = default;

            [[nodiscard]] bool isUploadComplete() const {
                return atlas.isUploadComplete(atlasSlot);
            }

            void flush() const {
                atlas.flush(atlasSlot);
            }
        };

        struct VirtualTexture {
            TextureID textureID{};
            Vec2u imageSize;
            unsigned int mipLevels;

            std::vector<VirtualTextureState> states;
            const unsigned int tileSize;

            StreamBuffer &tileMap;
            std::vector<StreamBuffer::Handle> tileMapUploadHandles{};

            StreamBuffer &tileMapOffsets;
            StreamBuffer::Handle tileMapOffsetsUploadHandle{};

            std::vector<unsigned int> tileMapOffsetsValues;
            std::vector<size_t> tileMapSizes;

            StreamBuffer &residencyMap;
            StreamBuffer::Handle residencyMapUploadHandle{};

            StreamBuffer &residencyMapOffsets;
            StreamBuffer::Handle residencyMapOffsetUploadHandle{};

            unsigned int residencyMapOffsetValue{};
            size_t residencyMapSize{};

            VirtualTexture(const TextureID textureID,
                           Vec2u imageSize,
                           const unsigned int mipLevels,
                           std::vector<VirtualTextureState> _states,
                           const unsigned int tileSize,
                           StreamBuffer &tileMap,
                           StreamBuffer &tileMapOffsets,
                           std::vector<unsigned int> _tileMapOffsetsValues,
                           std::vector<size_t> _tileMapSizes,
                           StreamBuffer &residencyMap,
                           StreamBuffer &residencyMapOffsets,
                           const unsigned int residencyMapOffsetValue,
                           const size_t residencyMapSize)
                : textureID(textureID),
                  imageSize(std::move(imageSize)),
                  mipLevels(mipLevels),
                  states(std::move(_states)),
                  tileSize(tileSize),
                  tileMap(tileMap),
                  tileMapUploadHandles(states.size()),
                  tileMapOffsets(tileMapOffsets),
                  tileMapOffsetsValues(std::move(_tileMapOffsetsValues)),
                  tileMapSizes(std::move(_tileMapSizes)),
                  residencyMap(residencyMap),
                  residencyMapOffsets(residencyMapOffsets),
                  residencyMapOffsetValue(residencyMapOffsetValue),
                  residencyMapSize(residencyMapSize) {
                tileMapOffsetsUploadHandle = tileMapOffsets.upload(
                    reinterpret_cast<const uint8_t *>(tileMapOffsetsValues.data()),
                    tileMapOffsetsValues.size() * sizeof(unsigned int),
                    textureID * sizeof(unsigned int));
                tileMapOffsets.flush(tileMapOffsetsUploadHandle);

                residencyMapOffsetUploadHandle = residencyMapOffsets.upload(
                    reinterpret_cast<const uint8_t *>(&residencyMapOffsetValue),
                    sizeof(unsigned int),
                    textureID * sizeof(unsigned int));
                residencyMapOffsets.flush(residencyMapOffsetUploadHandle);
            }

            ~VirtualTexture() {
                for (const auto &handle: tileMapUploadHandles) {
                    if (handle != StreamBuffer::INVALID_HANDLE) {
                        tileMap.release(handle);
                    }
                }
                if (tileMapOffsetsUploadHandle != StreamBuffer::INVALID_HANDLE) {
                    tileMapOffsets.release(tileMapOffsetsUploadHandle);
                }
                if (residencyMapUploadHandle != StreamBuffer::INVALID_HANDLE) {
                    residencyMap.release(residencyMapUploadHandle);
                }
                if (residencyMapOffsetUploadHandle != StreamBuffer::INVALID_HANDLE) {
                    residencyMapOffsets.release(residencyMapOffsetUploadHandle);
                }
            }

            VirtualTexture(const VirtualTexture &) = delete;

            VirtualTexture &operator=(const VirtualTexture &) = delete;

            VirtualTexture(VirtualTexture &&other) noexcept
                : textureID(other.textureID),
                  imageSize(std::move(other.imageSize)),
                  mipLevels(other.mipLevels),
                  states(std::move(other.states)),
                  tileSize(other.tileSize),
                  tileMap(other.tileMap),
                  tileMapUploadHandles(std::move(other.tileMapUploadHandles)),
                  tileMapOffsets(other.tileMapOffsets),
                  tileMapOffsetsUploadHandle(other.tileMapOffsetsUploadHandle),
                  tileMapOffsetsValues(std::move(other.tileMapOffsetsValues)),
                  tileMapSizes(std::move(other.tileMapSizes)),
                  residencyMap(other.residencyMap),
                  residencyMapUploadHandle(other.residencyMapUploadHandle),
                  residencyMapOffsets(other.residencyMapOffsets),
                  residencyMapOffsetUploadHandle(other.residencyMapOffsetUploadHandle),
                  residencyMapOffsetValue(other.residencyMapOffsetValue),
                  residencyMapSize(other.residencyMapSize) {
                other.tileMapOffsetsUploadHandle = StreamBuffer::INVALID_HANDLE;
                other.residencyMapOffsetUploadHandle = StreamBuffer::INVALID_HANDLE;
            }

            void updateTileMap(const unsigned int mip) {
                const auto tileMapOffset = tileMapOffsetsValues.at(mip);
                const auto &state = states.at(mip);

                if (tileMapUploadHandles.at(mip) != StreamBuffer::INVALID_HANDLE) {
                    tileMap.release(tileMapUploadHandles.at(mip));
                }

                const auto &atlasSlots = state.atlasSlots;
                const auto tileMapUploadHandle = tileMap.upload(
                    reinterpret_cast<const uint8_t *>(atlasSlots.data()),
                    sizeof(TextureAtlas::Slot) * atlasSlots.size(),
                    tileMapOffset * sizeof(unsigned int));
                tileMap.flush(tileMapUploadHandle);
                tileMapUploadHandles.at(mip) = tileMapUploadHandle;
            }

            void updateResidencyMap() {
                const auto residencyMapValues = getResidencyMap(states, tileSize);

                if (residencyMapUploadHandle != StreamBuffer::INVALID_HANDLE) {
                    residencyMap.release(residencyMapUploadHandle);
                }

                residencyMapUploadHandle = residencyMap.upload(
                    reinterpret_cast<const uint8_t *>(residencyMapValues.data()),
                    sizeof(unsigned int) * residencyMapSize,
                    residencyMapOffsetValue * sizeof(unsigned int));
                residencyMap.flush(residencyMapUploadHandle);
            }
        };

        void onUploadComplete(const TextureAtlas::Slot slot) override {
            const auto tileID = pendingTiles.at(slot);
            auto &state = textures.at(tileID.texture).states.at(tileID.mip);
            assert(state.getTileState(tileID.tile) == TILE_PENDING);
            state.setTileState(tileID.tile, TILE_RESIDENT);
            updatedMips[tileID.texture].insert(tileID.mip);
            pendingTiles.erase(slot);
        }

        rg::Runtime &runtime;
        TextureAtlas &atlas;
        ThreadPool &pool;

        const unsigned int tileSize;

        RangeAllocator textureIDAllocator;

        std::unordered_map<TextureID, VirtualTexture> textures;

        std::unordered_map<TextureID,
            std::unordered_map<unsigned int, std::unordered_map<Vec2u, TileAllocation> > > tileAllocations;

        std::unordered_map<TextureAtlas::Slot, TileID> pendingTiles;

        std::unordered_map<TextureID, std::unordered_set<unsigned int> > updatedMips;

        StreamBuffer tileMapOffsetsBuffer;
        StreamBuffer tileMapBuffer;
        RangeAllocator tileMapAllocator;

        StreamBuffer residencyMapOffsetsBuffer;
        StreamBuffer residencyMapBuffer;
        RangeAllocator residencyMapAllocator;

        rg::HeapResource<rg::Buffer> readbackBuffer;
        rg::HeapResource<rg::Buffer> readbackHostBuffer;
        rg::HeapResource<rg::Buffer> readbackClearBuffer;
    };
}

#endif //XENGINE_TILESTREAMER_HPP
