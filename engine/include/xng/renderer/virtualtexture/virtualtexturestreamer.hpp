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

#ifndef XENGINE_VIRTUALTEXTURESTREAMER_HPP
#define XENGINE_VIRTUALTEXTURESTREAMER_HPP

#include "tilestreamer.hpp"
#include "xng/assets/image.hpp"
#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

#include "xng/renderer/stream/streambuffer.hpp"

#include "xng/renderer/virtualtexture/textureatlas.hpp"
#include "xng/renderer/virtualtexture/tileloader.hpp"

namespace xng {
    /**
     * The VirtualTextureStreamer decides which tiles to load / evict based on readback.
     */
    class VirtualTextureStreamer {
    public:
        typedef TileStreamer::TextureID TextureID;

        VirtualTextureStreamer(rg::Runtime &runtime,
                               ChunkStreamer &chunkStreamer,
                               const unsigned int tileSize,
                               const unsigned int tileBorder,
                               const float maxAnisotropy)
            : runtime(runtime),
              atlas(runtime, chunkStreamer, tileSize, tileBorder, maxAnisotropy),
              tileStreamer(runtime, chunkStreamer, atlas, ThreadPool::getPool(), tileSize) {
        }

        TextureID create(const std::shared_ptr<TileLoader> &tileLoader) {
            const auto ret = tileStreamer.create(tileLoader->getSize(), tileLoader->getMipLevels());
            tileLoaders[ret] = tileLoader;

            const auto mip = tileLoader->getMipLevels() - 1;
            loadMip(ret, mip);
            return ret;
        }

        void destroy(const TileStreamer::TextureID textureID) {
            tileStreamer.destroy(textureID);
            tileLoaders.erase(textureID);
        }

        void update(RenderQueue &queue) {
            readback(queue);
        }

        void commit(RenderQueue &queue) {
            atlas.commit(queue);
            tileStreamer.commit(queue);
        }

        rg::HeapResource<rg::Buffer> getTileMapOffsetsBuffer() const {
            return tileStreamer.getTileMapOffsetsBuffer();
        }

        rg::HeapResource<rg::Buffer> getTileMapBuffer() const {
            return tileStreamer.getTileMapBuffer();
        }

        rg::HeapResource<rg::Buffer> getResidencyMapOffsetsBuffer() const {
            return tileStreamer.getResidencyMapOffsetsBuffer();
        }

        rg::HeapResource<rg::Buffer> getResidencyMapBuffer() const {
            return tileStreamer.getResidencyMapBuffer();
        }

        rg::HeapResource<rg::Buffer> getReadbackBuffer() const {
            return tileStreamer.getReadbackBuffer();
        }

        rg::HeapResource<rg::Texture> getAtlasTexture() const {
            return atlas.getTexture();
        }

        float getMaxAnisotropy() const {
            return maxAnisotropy;
        }

        void setMaxAnisotropy() {
            //TODO: Reallocate / copy resources on max anisotropy change
            throw std::runtime_error("Not implemented");
        }

        unsigned int getTileSize() const {
            return atlas.getTileSize();
        }

        unsigned int getTileBorder() const {
            return atlas.getTileBorder();
        }

        size_t getTilesInFlight() const {
            return tileStreamer.getTilesInFlight();
        }

    private:
        void loadMip(const TextureID texture, const unsigned int mip) {
            const auto &state = tileStreamer.getTextureState(texture).at(mip);
            for (unsigned int x = 0; x < state.tileCount.x; x++) {
                for (unsigned int y = 0; y < state.tileCount.y; y++) {
                    tileStreamer.uploadTile(texture,
                                            mip,
                                            {x, y},
                                            tileLoaders.at(texture)->getTile(mip, {x, y}),
                                            0);
                    tileStreamer.flushTile(texture, mip, {x, y});
                }
            }
        }

        static constexpr size_t timeOut = 10'000'000'000ULL;

        void readback(RenderQueue &queue) {
            // Readback taps

            // TODO: Implement efficient tile streaming

            // Now I need to implement the actual tile streaming / eviction logic.
            // The challenges are that the vt system expects any given mip chain to be fully resident because
            // the residency map is one tile map sized to mip 0 where each tile contains the finest allocated mip chain
            // for that tile.

            // RenderDoc appears to not handle coherent mappings correctly.
            // This is now fixed internally in the opengl adapter via explicit flush / invalidate semantics.
            if (readbackFence != nullptr) {
                if (!readbackFence->wait(timeOut)) {
                    throw std::runtime_error("Virtual texture readback timed out.");
                }

                std::unordered_map<unsigned int, std::unordered_set<Vec2u> > pinnedTiles;

                const auto mapping = runtime.getResourceHeap().map(tileStreamer.getReadbackHostBuffer());
                const auto ptr = reinterpret_cast<unsigned int *>(mapping->data());
                for (auto &pair: tileLoaders) {
                    const auto &states = tileStreamer.getTextureState(pair.first);
                    for (auto mip = 0; mip < states.size() - 1; mip++) {
                        auto &state = states.at(mip);
                        const auto tileCount = state.tileCount;
                        const auto offset = state.tileMapOffset;
                        for (auto tileIndex = 0; tileIndex < tileCount.x * tileCount.y; tileIndex++) {
                            const auto tile = TileStreamer::indexToTile(tileIndex, tileCount);
                            const unsigned int taps = *(ptr
                                                        + offset
                                                        + tileIndex);
                            if (taps > 0) {
                                auto p = loadTileChain(pair.first,
                                                       tile,
                                                       mip,
                                                       states);
                                for (auto &pin: p) {
                                    pinnedTiles[pin.first].insert(pin.second.begin(), pin.second.end());
                                }
                            } else {
                                if (state.getTileState(tile) == TileStreamer::TILE_EVICTED)
                                    continue;
                                if (pinnedTiles[mip].find(tile) == pinnedTiles[mip].end())
                                    tileStreamer.evictTile(pair.first, mip, tile);
                            }

                            if (tileStreamer.getTilesInFlight() >= 128) {
                                break;
                            }
                        }
                        if (tileStreamer.getTilesInFlight() >= 128) {
                            break;
                        }
                    }
                    if (tileStreamer.getTilesInFlight() >= 128) {
                        break;
                    }
                }
            }
            readbackFence = queue.addPostFrame(rg::GraphicsPassBuilder("VirtualTextureStreamer/Readback")
                .transferRead(tileStreamer.getReadbackBuffer())
                .transferWrite(tileStreamer.getReadbackHostBuffer())
                .execute([this](rg::RasterContext &, rg::TransferContext &ctx, rg::ComputeContext &) {
                    ctx.copyBuffer(tileStreamer.getReadbackHostBuffer(),
                                   tileStreamer.getReadbackBuffer(),
                                   0,
                                   0,
                                   tileStreamer.getReadbackBuffer().getDescription().size);
                }));
        }

        std::unordered_map<unsigned int, std::unordered_set<Vec2u> > loadTileChain(const TextureID texture,
            const Vec2u &tile,
            const unsigned int mip,
            const std::vector<TileStreamer::VirtualTextureState> &mips) {
            std::unordered_map<unsigned int, std::unordered_set<Vec2u> > ret;

            const auto size0 = mips.at(0).size;
            const auto coarsest = static_cast<unsigned int>(mips.size()) - 1;
            const unsigned int x0 = (tile.x * atlas.getTileSize()) << mip;
            const unsigned int y0 = (tile.y * atlas.getTileSize()) << mip;
            const unsigned int x1 = std::min(((tile.x + 1) * atlas.getTileSize()) << mip, size0.x) - 1;
            const unsigned int y1 = std::min(((tile.y + 1) * atlas.getTileSize()) << mip, size0.y) - 1;

            for (auto m = mip; m <= coarsest; ++m) {
                const auto &state = mips.at(m);
                const unsigned int tx0 = std::min((x0 >> m) / atlas.getTileSize(), state.tileCount.x - 1);
                const unsigned int tx1 = std::min((x1 >> m) / atlas.getTileSize(), state.tileCount.x - 1);
                const unsigned int ty0 = std::min((y0 >> m) / atlas.getTileSize(), state.tileCount.y - 1);
                const unsigned int ty1 = std::min((y1 >> m) / atlas.getTileSize(), state.tileCount.y - 1);
                for (auto ty = ty0; ty <= ty1; ++ty) {
                    for (auto tx = tx0; tx <= tx1; ++tx) {
                        if (state.getTileState({tx, ty}) == TileStreamer::TILE_EVICTED) {
                            tileStreamer.uploadTile(texture,
                                                    m,
                                                    {tx, ty},
                                                    tileLoaders.at(texture)->getTile(m, {tx, ty}),
                                                    static_cast<int>(mips.size() - m));
                        }
                        ret[m].insert({tx, ty});
                    }
                }
            }
            return ret;
        }

        rg::Runtime &runtime;
        TextureAtlas atlas;
        TileStreamer tileStreamer;

        float maxAnisotropy = 8.0f;

        std::shared_ptr<RenderQueue::SubmitFence> readbackFence = nullptr;

        std::unordered_map<TextureID, std::shared_ptr<TileLoader> > tileLoaders;
    };
}

#endif //XENGINE_VIRTUALTEXTURESTREAMER_HPP
