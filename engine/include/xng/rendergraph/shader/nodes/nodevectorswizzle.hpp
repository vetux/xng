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

#ifndef XENGINE_NODEVECTORSWIZZLE_HPP
#define XENGINE_NODEVECTORSWIZZLE_HPP

#include <utility>

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeVectorSwizzle final : ShaderNode {
        enum ComponentIndex : int {
            COMPONENT_X = 0,
            COMPONENT_Y,
            COMPONENT_Z,
            COMPONENT_W
        };

        std::unique_ptr<ShaderNode> vector;
        std::vector<ComponentIndex> indices; // Specify up to 4 indices for swizzling

        NodeVectorSwizzle(std::unique_ptr<ShaderNode> vector, std::vector<ComponentIndex> indices)
            : vector(std::move(vector)),
              indices(std::move(indices)) {
        }

        NodeType getType() const override {
            return VECTOR_SWIZZLE;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeVectorSwizzle>(vector->copy(), indices);
        }
    };
}
#endif //XENGINE_NODEVECTORSWIZZLE_HPP