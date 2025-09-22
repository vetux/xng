/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_NODETEXTUREFETCH_HPP
#define XENGINE_NODETEXTUREFETCH_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeTextureFetch final : ShaderNode {
        // TODO: Redesign texture interaction nodes to be more generic to support directx

        std::unique_ptr<ShaderNode> texture;
        std::unique_ptr<ShaderNode> coordinate;
        std::unique_ptr<ShaderNode> index; // Either the lod or sample index

        explicit NodeTextureFetch(std::unique_ptr<ShaderNode> texture,
                                  std::unique_ptr<ShaderNode> coordinate,
                                  std::unique_ptr<ShaderNode> index)
            : texture(std::move(texture)),
              coordinate(std::move(coordinate)),
              index(std::move(index)) {
        }

        NodeType getType() const override {
            return TEXTURE_FETCH;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeTextureFetch>(texture->copy(),
                                                      coordinate->copy(),
                                                      index->copy());
        }
    };
}
#endif //XENGINE_NODETEXTUREFETCH_HPP
