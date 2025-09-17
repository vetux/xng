/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_NODEMATRIX_HPP
#define XENGINE_NODEMATRIX_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeMatrix final : ShaderNode {
        ShaderDataType type;

        // The vectors to initialize the matrix with
        std::unique_ptr<ShaderNode> x;
        std::unique_ptr<ShaderNode> y;
        std::unique_ptr<ShaderNode> z;
        std::unique_ptr<ShaderNode> w;

        NodeMatrix(const ShaderDataType &type,
                   std::unique_ptr<ShaderNode> x,
                   std::unique_ptr<ShaderNode> y,
                   std::unique_ptr<ShaderNode> z,
                   std::unique_ptr<ShaderNode> w)
            : type(type),
              x(std::move(x)),
              y(std::move(y)),
              z(std::move(z)),
              w(std::move(w)) {
        }

        NodeType getType() const override {
            return MATRIX;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeMatrix>(type,
                                                x->copy(),
                                                y ? y->copy() : nullptr,
                                                z ? z->copy() : nullptr,
                                                w ? w->copy() : nullptr);
        }
    };
}
#endif //XENGINE_NODEMATRIX_HPP
