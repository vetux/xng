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

#ifndef XENGINE_TILEMAP_HPP
#define XENGINE_TILEMAP_HPP

#include "xng/math/vector2.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

namespace xng {
    class TileMap {
    public:
        typedef unsigned int TextureID;

        TextureID create(const Vec2u &imageSize, unsigned int mipLevels) {
        }

        void destroy(const TextureID textureID) {
        }

        void setAtlasTile(const TextureID textureID,
                          const unsigned int mip,
                          const Vec2u &tile,
                          const unsigned int atlasLayer,
                          const Vec2u &atlasTile) {
        }

        void setResidentMip(const TextureID textureID,
                            const Vec2u &tile,
                            const unsigned int residentMip) {
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {

        }

    private:
        rg::HeapResource<rg::Buffer> tileMapOffsetsBuffer;
        rg::HeapResource<rg::Buffer> tileMapBuffer;
        rg::HeapResource<rg::Buffer> residencyBuffer;
    };
}

#endif //XENGINE_TILEMAP_HPP
