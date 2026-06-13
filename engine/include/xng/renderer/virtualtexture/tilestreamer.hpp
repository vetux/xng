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
     * Load tiles via TileLoader asynchronously and upload the tiles to the atlas texture on the render thread and update the residency buffer.
     */
    class TileStreamer {
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

        struct TextureState {
            Vec2u size;
            Vec2u tileCount;

            std::vector<bool> resident;
            std::vector<TextureAtlas::Slot> atlasSlots;

            TextureState(Vec2u size, const Vec2u &tileCount)
                : size(std::move(size)),
                  tileCount(tileCount),
                  resident(tileCount.x * tileCount.y),
                  atlasSlots(tileCount.x * tileCount.y) {
            }

            [[nodiscard]] bool isResident(const Vec2u &tile) const {
                return resident.at(tileToIndex(tile, tileCount));
            }

            void setResident(const Vec2u &tile, const bool isResident) {
                this->resident.at(tileToIndex(tile, tileCount)) = isResident;
            }

            [[nodiscard]] TextureAtlas::Slot getAtlasSlot(const Vec2u &tile) const {
                return atlasSlots.at(tileToIndex(tile, tileCount));
            }

            void setAtlasSlot(const Vec2u &tile, const TextureAtlas::Slot slot) {
                this->atlasSlots.at(tileToIndex(tile, tileCount)) = slot;
            }
        };

        TileStreamer(rg::Heap &heap,
                     ChunkStreamer &chunkStreamer,
                     TextureAtlas &atlas,
                     ThreadPool &pool,
                     const unsigned int tileSize)
            : heap(heap),
              atlas(atlas),
              pool(pool),
              tileSize(tileSize),
              tileMapOffsetsBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              tileMapBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              residencyMapOffsetsBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
              residencyMapBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE) {
        }

        ~TileStreamer() = default;

        TileStreamer(const TileStreamer &) = delete;

        TileStreamer &operator=(const TileStreamer &) = delete;

        TileStreamer(TileStreamer &&) = default;

        TileStreamer &operator=(TileStreamer &&) = delete;

        TextureID create(const std::shared_ptr<TileLoader> &tileLoader) {
            const auto imageSize = tileLoader->getSize();
            const auto mipLevels = tileLoader->getMipLevels();

            if (mipLevels == 0) {
                throw std::runtime_error("Mip levels must be greater than 0.");
            }

            const TextureID ret = textureIDAllocator.allocate(mipLevels);

            VirtualTexture tex;

            tex.loader = tileLoader;

            for (auto mip = 0; mip < mipLevels; mip++) {
                const auto mipSize = rg::Texture::getMipLevelSize(imageSize, mip);
                const auto mipTiles = getTiles(mipSize, tileSize);
                const auto tileMapSize = mipTiles.x * mipTiles.y;
                const auto offset = static_cast<unsigned int>(tileMapAllocator.allocate(tileMapSize));
                tex.tileMapOffsets.emplace_back(offset);
                tex.tileMapSizes.emplace_back(tileMapSize);

                const auto tileMapOffsetsUpload = tileMapOffsetsBuffer.upload(
                    reinterpret_cast<const uint8_t *>(&offset),
                    sizeof(unsigned int),
                    (ret + mip) * sizeof(unsigned int));
                tex.tileMapOffsetsUploadHandles.emplace_back(tileMapOffsetsUpload);

                const auto tileMapBufferUpload = tileMapBuffer.upload(
                    std::vector<uint8_t>(mipTiles.x * mipTiles.y * sizeof(unsigned int), 0),
                    offset * sizeof(unsigned int));
                tex.tileMapUploadHandles.emplace_back(tileMapBufferUpload);

                textureStates[ret].emplace_back(mipSize, mipTiles);
            }

            const Vec2u mip0Tiles = getTiles(imageSize, tileSize);
            const auto residencyMapSize = mip0Tiles.x * mip0Tiles.y;
            const auto residencyMapOffset = static_cast<unsigned int>(residencyMapAllocator.allocate(residencyMapSize));

            tex.residencyMapOffset = residencyMapOffset;
            tex.residencyMapSize = residencyMapSize;

            tex.residencyMapOffsetUploadHandle = residencyMapOffsetsBuffer.upload(
                reinterpret_cast<const uint8_t *>(&residencyMapOffset),
                sizeof(unsigned int),
                ret * sizeof(unsigned int));

            const std::vector residencyMapSeed(residencyMapSize, mipLevels - 1u);
            tex.residencyMapUploadHandle = residencyMapBuffer.upload(
                reinterpret_cast<const uint8_t *>(residencyMapSeed.data()),
                sizeof(unsigned int) * residencyMapSize,
                residencyMapOffset * sizeof(unsigned int));

            textures[ret] = std::move(tex);

            return ret;
        }

        void destroy(const TextureID tex) {
            textureIDAllocator.free(tex, textures.at(tex).tileMapOffsets.size());

            const auto &texture = textures.at(tex);

            // Free allocators
            for (auto i = 0; i < texture.tileMapOffsets.size(); i++) {
                tileMapAllocator.free(texture.tileMapOffsets.at(i), texture.tileMapSizes.at(i));
            }
            residencyMapAllocator.free(texture.residencyMapOffset, texture.residencyMapSize);

            // Release stream buffer uploads
            for (const auto &uploadHandle: texture.tileMapOffsetsUploadHandles) {
                tileMapOffsetsBuffer.release(uploadHandle);
            }
            for (const auto &uploadHandle: texture.tileMapUploadHandles) {
                tileMapBuffer.release(uploadHandle);
            }
            residencyMapOffsetsBuffer.release(texture.residencyMapOffsetUploadHandle);
            residencyMapBuffer.release(texture.residencyMapUploadHandle);

            textures.erase(tex);
            textureStates.erase(tex);
            pendingUploads.erase(tex);
            updatedMips.erase(tex);
        }

        const std::unordered_map<TextureID, std::vector<TextureState> > &getTextureStates() const {
            return textureStates;
        }

        void loadTile(const TextureID tex, unsigned int mip, const Vec2u &tile) {
            const auto texSize = textures.at(tex).loader->getSize();
            const auto mipSize = rg::Texture::getMipLevelSize(texSize, mip);
            const auto mipTiles = getTiles(mipSize, tileSize);
            pendingUploads[tex].emplace_back(tex,
                                             mip,
                                             tile,
                                             mipTiles,
                                             pool,
                                             textures.at(tex).loader);
        }

        void evictTile(const TextureID tex, const unsigned int mip, const Vec2u &tile) {
            std::vector<PendingUpload> nPendingUploads;
            for (auto &pendingUpload: pendingUploads[tex]) {
                if (pendingUpload.mip == mip && pendingUpload.tile == tile) {
                    if (pendingUpload.startedUpload) {
                        atlas.destroy(pendingUpload.atlasSlot);
                    }
                    continue;
                }
                nPendingUploads.emplace_back(std::move(pendingUpload));
            }
            pendingUploads[tex] = std::move(nPendingUploads);
            auto &state = textureStates.at(tex).at(mip);
            if (state.isResident(tile)) {
                atlas.destroy(state.getAtlasSlot(tile));
                state.setResident(tile, false);
                updatedMips[tex].insert(mip);
            }
        }

        void flush(const TextureID tex) {
            for (auto &pendingUpload: pendingUploads[tex]) {
                pendingUpload.flushed = true;
            }
        }

        bool isUploadComplete(const TextureID tex) {
            for (auto &pendingUpload: pendingUploads[tex]) {
                if (!pendingUpload.flushed) {
                    if (pendingUpload.startedUpload) {
                        if (!atlas.isUploadComplete(pendingUpload.uploadHandle)) {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            std::vector<rg::TransferPass> ret;

            std::unordered_map<TextureID, std::vector<PendingUpload> > framePendingUploads;
            for (auto &pair: pendingUploads) {
                for (auto &pendingUpload: pair.second) {
                    if (pendingUpload.flushed) {
                        if (!pendingUpload.startedUpload) {
                            pendingUpload.tileTask->join();
                            pendingUpload.startedUpload = true;
                            pendingUpload.atlasSlot = atlas.create();
                            pendingUpload.uploadHandle = atlas.upload(pendingUpload.atlasSlot,
                                                                      std::move(*pendingUpload.tileData));
                        }
                        atlas.flush(pendingUpload.uploadHandle);
                        auto &state = textureStates.at(pair.first).at(pendingUpload.mip);
                        state.setResident(pendingUpload.tile, true);
                        state.setAtlasSlot(pendingUpload.tile, pendingUpload.atlasSlot);
                        updatedMips[pendingUpload.tex].insert(pendingUpload.mip);
                        continue;
                    }

                    if (pendingUpload.startedUpload) {
                        if (atlas.isUploadComplete(pendingUpload.uploadHandle)) {
                            auto &state = textureStates.at(pair.first).at(pendingUpload.mip);
                            state.setResident(pendingUpload.tile, true);
                            state.setAtlasSlot(pendingUpload.tile, pendingUpload.atlasSlot);
                            updatedMips[pendingUpload.tex].insert(pendingUpload.mip);
                            continue;
                        }
                    } else if (pendingUpload.tileTask->isDone()) {
                        pendingUpload.startedUpload = true;
                        pendingUpload.atlasSlot = atlas.create();
                        pendingUpload.uploadHandle = atlas.upload(pendingUpload.atlasSlot,
                                                                  std::move(*pendingUpload.tileData));
                    }
                    framePendingUploads[pair.first].emplace_back(std::move(pendingUpload));
                }
            }
            pendingUploads = std::move(framePendingUploads);

            for (auto &pair: updatedMips) {
                auto &texture = textures.at(pair.first);
                // Update TileMap
                for (auto &mip: pair.second) {
                    const auto tileMapOffset = texture.tileMapOffsets.at(mip);
                    auto &state = textureStates.at(pair.first).at(mip);
                    tileMapBuffer.release(texture.tileMapUploadHandles.at(mip));

                    const auto &atlasSlots = state.atlasSlots;
                    const auto tileMapUploadHandle = tileMapBuffer.upload(
                        reinterpret_cast<const uint8_t *>(atlasSlots.data()),
                        sizeof(TextureAtlas::Slot) * atlasSlots.size(),
                        tileMapOffset * sizeof(unsigned int));
                    tileMapBuffer.flush(tileMapUploadHandle);
                    texture.tileMapUploadHandles.at(mip) = tileMapUploadHandle;
                }

                // Update Residency Map
                const auto residencyMap = getResidencyMap(textureStates.at(pair.first), tileSize);
                const auto residencyMapOffset = texture.residencyMapOffset;
                residencyMapBuffer.release(texture.residencyMapUploadHandle);
                const auto residencyMapUploadHandle = residencyMapBuffer.upload(
                    reinterpret_cast<const uint8_t *>(residencyMap.data()),
                    sizeof(unsigned int) * residencyMap.size(),
                    residencyMapOffset * sizeof(unsigned int));
                residencyMapBuffer.flush(residencyMapUploadHandle);
                texture.residencyMapUploadHandle = residencyMapUploadHandle;
            }
            updatedMips.clear();

            auto passes = tileMapOffsetsBuffer.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());

            passes = tileMapBuffer.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());

            passes = residencyMapOffsetsBuffer.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());

            passes = residencyMapBuffer.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());

            return ret;
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

    private:
        static constexpr unsigned int ceildiv(const unsigned int a, const unsigned int b) {
            return (a + b - 1) / b;
        }

        static std::vector<unsigned int> getResidencyMap(const std::vector<TextureState> &textureStates,
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
                                if (!state.isResident(t)) {
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

        struct PendingUpload {
            TextureID tex{};
            unsigned int mip{};
            Vec2u tile;
            unsigned int tileIndex{};

            std::shared_ptr<Task> tileTask;
            std::shared_ptr<std::vector<uint8_t> > tileData; // The tile data including borders

            TextureAtlas::Slot atlasSlot = {};

            bool startedUpload = false;
            TextureAtlas::UploadHandle uploadHandle{};

            bool flushed = false;

            PendingUpload() = default;

            PendingUpload(const TextureID tex,
                          const unsigned int mip,
                          Vec2u _tile,
                          const Vec2u &mipTiles,
                          ThreadPool &pool,
                          const std::shared_ptr<TileLoader> &loader)
                : tex(tex),
                  mip(mip),
                  tile(std::move(_tile)),
                  tileIndex(tileToIndex(tile, mipTiles)) {
                tileData = std::make_shared<std::vector<uint8_t> >();
                auto tileDataCapture = tileData;
                auto tileCapture = tile;
                tileTask = pool.addTask([tileDataCapture, loader, mip, tileCapture]() {
                    *tileDataCapture = std::move(loader->getTile(mip, tileCapture));
                });
                tileTask->start();
            }
        };

        struct VirtualTexture {
            std::shared_ptr<TileLoader> loader;

            std::vector<unsigned int> tileMapOffsets;
            std::vector<size_t> tileMapSizes;

            std::vector<StreamBuffer::Handle> tileMapOffsetsUploadHandles;
            std::vector<StreamBuffer::Handle> tileMapUploadHandles;

            unsigned int residencyMapOffset{};
            size_t residencyMapSize{};
            StreamBuffer::Handle residencyMapOffsetUploadHandle{};

            StreamBuffer::Handle residencyMapUploadHandle{};
        };

        rg::Heap &heap;
        TextureAtlas &atlas;
        ThreadPool &pool;

        const unsigned int tileSize;

        RangeAllocator textureIDAllocator;

        std::unordered_map<TextureID, VirtualTexture> textures;
        std::unordered_map<TextureID, std::vector<TextureState> > textureStates;
        std::unordered_map<TextureID, std::vector<PendingUpload> > pendingUploads;

        std::unordered_map<TextureID, std::unordered_set<unsigned int> > updatedMips;

        StreamBuffer tileMapOffsetsBuffer;
        StreamBuffer tileMapBuffer;
        RangeAllocator tileMapAllocator;

        StreamBuffer residencyMapOffsetsBuffer;
        StreamBuffer residencyMapBuffer;
        RangeAllocator residencyMapAllocator;
    };
}

#endif //XENGINE_TILESTREAMER_HPP
