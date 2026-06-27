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
              tileStreamer(runtime.getResourceHeap(), chunkStreamer, atlas, ThreadPool::getPool(), tileSize) {
        }

        TextureID create(const std::shared_ptr<TileLoader> &tileLoader) {
            return tileStreamer.create(tileLoader);
        }

        void destroy(const TileStreamer::TextureID textureID) {
            tileStreamer.destroy(textureID);
        }

        void update() {
            // TODO: Tile streaming limiting based on available VRAM
            // TODO: Tile eviction / History

            tileStreamer.readback();

            const auto tileSize = atlas.getTileSize();

            for (const auto &pair: tileStreamer.getTextureStates()) {
                const TextureID tex = pair.first;
                const auto &mips = pair.second; // [0] = finest, back() = coarsest
                const auto coarsest = static_cast<unsigned int>(mips.size()) - 1;
                const auto size0 = mips[0].size;

                std::vector<std::vector<uint8_t> > queued(mips.size());
                for (auto m = 0u; m < mips.size(); ++m)
                    queued[m].assign(mips[m].tileCount.x * mips[m].tileCount.y, 0);

                auto request = [&](const unsigned int mip, const Vec2u &tile) {
                    const auto idx = TileStreamer::tileToIndex(tile, mips[mip].tileCount);
                    if (queued[mip][idx])
                        return;
                    queued[mip][idx] = 1;
                    tileStreamer.loadTile(tex, mip, tile, pair.second.size() - mip);
                };

                // A tile's full covering tile-RANGE at every mip from `fromMip` to coarsest.
                // Uses the same mip0-texel >> mip mapping as getResidencyMap, so coverage and
                // residency always agree. Under the >> convention the range is always 1x1;
                // if you switch to native-res addressing it widens to <=2x2 at NPOT boundaries.
                auto requestChain = [&](const unsigned int fromMip, const Vec2u &tile) {
                    const unsigned int x0 = (tile.x * tileSize) << fromMip;
                    const unsigned int y0 = (tile.y * tileSize) << fromMip;
                    const unsigned int x1 = std::min(((tile.x + 1) * tileSize) << fromMip, size0.x) - 1;
                    const unsigned int y1 = std::min(((tile.y + 1) * tileSize) << fromMip, size0.y) - 1;

                    for (auto m = fromMip; m <= coarsest; ++m) {
                        const auto &state = mips[m];
                        const unsigned int tx0 = std::min((x0 >> m) / tileSize, state.tileCount.x - 1);
                        const unsigned int tx1 = std::min((x1 >> m) / tileSize, state.tileCount.x - 1);
                        const unsigned int ty0 = std::min((y0 >> m) / tileSize, state.tileCount.y - 1);
                        const unsigned int ty1 = std::min((y1 >> m) / tileSize, state.tileCount.y - 1);
                        for (auto ty = ty0; ty <= ty1; ++ty)
                            for (auto tx = tx0; tx <= tx1; ++tx)
                                request(m, {tx, ty});
                    }
                };

                // Coarsest mip: always fully resident, flushed below.
                for (auto x = 0u; x < mips[coarsest].tileCount.x; ++x)
                    for (auto y = 0u; y < mips[coarsest].tileCount.y; ++y)
                        request(coarsest, {x, y});

                // Every tapped tile + its covering coarser chain (streamed, not flushed).
                for (auto mip = 0u; mip < coarsest; ++mip) {
                    const auto &state = mips[mip];
                    for (auto x = 0u; x < state.tileCount.x; ++x)
                        for (auto y = 0u; y < state.tileCount.y; ++y)
                            if (state.getTaps({x, y}) != 0)
                                requestChain(mip, {x, y});
                }

                tileStreamer.flush(tex, coarsest);
            }
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            std::vector<rg::TransferPass> ret;
            auto passes = tileStreamer.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());
            passes = atlas.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());
            return ret;
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

    private:
        rg::Runtime &runtime;
        TextureAtlas atlas;
        TileStreamer tileStreamer;

        float maxAnisotropy = 8.0f;
    };
}

#endif //XENGINE_VIRTUALTEXTURESTREAMER_HPP
