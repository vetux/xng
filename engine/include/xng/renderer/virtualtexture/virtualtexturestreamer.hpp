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
#include "xng/renderer/stream/texturestreamer.hpp"

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
            const auto ret = tileStreamer.create(tileLoader);
            const auto mip = tileLoader->getMipLevels() - 1;
            tileStreamer.loadTiles(ret, mip, 0);
            tileStreamer.flush(ret, mip);
            return ret;
        }

        void destroy(const TileStreamer::TextureID textureID) {
            tileStreamer.destroy(textureID);
        }

        void update() {
            // TODO: Tile streaming limiting based on available VRAM

            auto tappedTiles = tileStreamer.readback();
            for (auto &pair: tappedTiles) {
                const auto &mips = tileStreamer.getTextureStates().at(pair.first);

                unsigned int finestMipTapped = mips.size() - 1;
                for (auto &mipPair: pair.second) {
                    const auto &mip = mipPair.first;
                    const auto &state = mips.at(mip);
                    for (auto &tile: mipPair.second) {
                        if (state.getTaps(tile) > 0) {
                            finestMipTapped = std::min(finestMipTapped, mip);
                            loadTileChain(pair.first, tile, mip, static_cast<int>(mips.size() - mip), mips);
                        }
                    }
                }

                for (auto i = 0u; i < finestMipTapped; ++i) {
                    tileStreamer.evictTiles(pair.first, i);
                }
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

        bool isUploadComplete(const TextureID textureID) const {
            return tileStreamer.isUploadComplete(textureID);
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

        size_t getStreamingTiles() const {
            return tileStreamer.getStreamingTiles();
        }

    private:
        void loadTileChain(const TextureID texture,
                           const Vec2u &tile,
                           const unsigned int mip,
                           const int priority,
                           const std::vector<TileStreamer::TextureState> &mips) {
            const auto size0 = mips.at(0).size;
            const auto coarsest = static_cast<unsigned int>(mips.size()) - 1;
            const unsigned int x0 = (tile.x * atlas.getTileSize()) << mip;
            const unsigned int y0 = (tile.y * atlas.getTileSize()) << mip;
            const unsigned int x1 = std::min(((tile.x + 1) * mip) << mip, size0.x) - 1;
            const unsigned int y1 = std::min(((tile.y + 1) * atlas.getTileSize()) << mip, size0.y) - 1;

            for (auto m = mip; m <= coarsest; ++m) {
                const auto &state = mips.at(m);
                const unsigned int tx0 = std::min((x0 >> m) / atlas.getTileSize(), state.tileCount.x - 1);
                const unsigned int tx1 = std::min((x1 >> m) / atlas.getTileSize(), state.tileCount.x - 1);
                const unsigned int ty0 = std::min((y0 >> m) / atlas.getTileSize(), state.tileCount.y - 1);
                const unsigned int ty1 = std::min((y1 >> m) / atlas.getTileSize(), state.tileCount.y - 1);
                for (auto ty = ty0; ty <= ty1; ++ty) {
                    for (auto tx = tx0; tx <= tx1; ++tx) {
                        tileStreamer.loadTile(texture, m, {tx, ty}, priority);
                    }
                }
            }
        }

        rg::Runtime &runtime;
        TextureAtlas atlas;
        TileStreamer tileStreamer;

        float maxAnisotropy = 8.0f;
    };
}

#endif //XENGINE_VIRTUALTEXTURESTREAMER_HPP
