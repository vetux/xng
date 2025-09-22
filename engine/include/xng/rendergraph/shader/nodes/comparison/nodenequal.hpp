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

#ifndef XENGINE_NODENEQUAL_HPP
#define XENGINE_NODENEQUAL_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeNotEqual final : ShaderNode {
        std::unique_ptr<ShaderNode> left;
        std::unique_ptr<ShaderNode> right;

        NodeNotEqual(std::unique_ptr<ShaderNode> left, std::unique_ptr<ShaderNode> right)
            : left(std::move(left)),
              right(std::move(right)) {
        }

        NodeType getType() const override {
            return NEQUAL;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeNotEqual>(left->copy(), right->copy());
        }
    };
}
#endif //XENGINE_NODENEQUAL_HPP