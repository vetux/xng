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

        explicit VirtualTextureStreamer(rg::Runtime &runtime,
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

        void readback() {
            //TODO: Implement readback / Readback needs to be transparent to user defined passes

            // Upload all coarsest mip tiles for testing.
            for (auto &pair: tileStreamer.getTextureStates()) {
                auto &state = pair.second.back();
                for (auto x = 0u; x < state.tileCount.x; x++) {
                    for (auto y = 0u; y < state.tileCount.y; y++) {
                        if (!state.isResident({x, y})) {
                            tileStreamer.loadTile(pair.first, pair.second.size() - 1, {x, y});
                        }
                    }
                }
                tileStreamer.flush(pair.first);
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
