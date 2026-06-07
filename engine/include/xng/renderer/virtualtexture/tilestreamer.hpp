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

#include <vector>
#include <cstdint>

#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

#include "xng/renderer/virtualtexture/textureatlas.hpp"
#include "xng/renderer/virtualtexture/tilemap.hpp"
#include "xng/renderer/virtualtexture/tileloader.hpp"

namespace xng {
    /**
     * Per logical texture there is one TileStreamer that handles loading tiles via TileLoader asynchronously
     * and uploads the tiles to the atlas texture on the render thread and updates the residency buffer.
     */
    class TileStreamer {
    public:
        TileStreamer(rg::Heap &heap,
                     TextureAtlas &atlas,
                     TileMap &tileMap,
                     TileMap::TextureID textureID,
                     std::unique_ptr<TileLoader> tileLoader);

        ~TileStreamer();

        TileStreamer(const TileStreamer &) = delete;

        TileStreamer &operator=(const TileStreamer &) = delete;

        TileStreamer(TileStreamer &&) = default;

        TileStreamer &operator=(TileStreamer &&) = default;

        void loadTile(const Vec2u &tile, unsigned int mip);

        void evictTile(const Vec2u &tile, unsigned int mip);

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph);

    private:
        [[nodiscard]] unsigned int getTileIndex(unsigned int x, unsigned int y) const;

        struct Tile {
            bool resident;
        };

        struct Mip {
            Vec2i size;
            Vec2u tileCount;
            std::vector<Tile> tiles;
        };

        Vec2u tileSize;
        std::vector<Mip> mips;
    };
}
#endif //XENGINE_TILESTREAMER_HPP
