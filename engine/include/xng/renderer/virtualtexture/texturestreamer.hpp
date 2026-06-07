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

#ifndef XENGINE_TEXTURESTREAMER_HPP
#define XENGINE_TEXTURESTREAMER_HPP

#include "tilestreamer.hpp"
#include "xng/assets/image.hpp"
#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

#include "xng/renderer/stream/streambuffer.hpp"
#include "xng/renderer/stream/streamtexture.hpp"

#include "xng/renderer/virtualtexture/tilemap.hpp"
#include "xng/renderer/virtualtexture/textureatlas.hpp"
#include "xng/renderer/virtualtexture/tileloader.hpp"

namespace xng {
    /**
     * The TextureStreamer manages TileStreamers for each texture in the scene.
     * It samples from the readback buffer to determine which tiles to stream in via the TileStreamers.
     */
    class TextureStreamer {
    public:
        TileMap::TextureID create(std::unique_ptr<TileLoader> tileLoader) {
            auto id = tileMap.create(tileLoader->getSize(), tileLoader->getMipLevels());
            tileStreamers.emplace(id, TileStreamer(heap, atlas, tileMap, id, std::move(tileLoader)));
            return id;
        }

        void destroy(const TileMap::TextureID textureID) {
            tileMap.destroy(textureID);
            tileStreamers.erase(textureID);
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            std::vector<rg::TransferPass> ret;
            for (auto &pair: tileStreamers) {
                auto passes = pair.second.commit(graph);
                ret.insert(ret.end(), passes.begin(), passes.end());
            }
            auto passes = tileMap.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());
            passes = atlas.commit(graph);
            ret.insert(ret.end(), passes.begin(), passes.end());
            return ret;
        }

    private:
        rg::Heap &heap;
        TextureAtlas atlas;
        TileMap tileMap;
        std::unordered_map<TileMap::TextureID, TileStreamer> tileStreamers;
    };
}

#endif //XENGINE_TEXTURESTREAMER_HPP
