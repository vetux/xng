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

#ifndef XENGINE_NODEVECTOR_HPP
#define XENGINE_NODEVECTOR_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    /**
     * Construct a vector from the given components.
     *
     * The vector size depends on the assigned inputs,
     * for example, if x,y,z is assigned, the resulting vector is a Vector3.
     */
    struct NodeVector final : ShaderNode {
        ShaderDataType type;

        std::unique_ptr<ShaderNode> x;
        std::unique_ptr<ShaderNode> y;
        std::unique_ptr<ShaderNode> z;
        std::unique_ptr<ShaderNode> w;

        NodeVector(const ShaderDataType type,
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
            return VECTOR;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeVector>(type,
                                                  x->copy(),
                                                  y->copy(),
                                                  z ? z->copy() : nullptr,
                                                  w ? w->copy() : nullptr);
        }
    };
}

#endif //XENGINE_NODEVECTOR_HPP
