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

#ifndef XENGINE_NODELOOP_HPP
#define XENGINE_NODELOOP_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeLoop : ShaderNode {
        std::unique_ptr<ShaderNode> initializer;
        std::unique_ptr<ShaderNode> predicate;
        std::unique_ptr<ShaderNode> iterator;
        std::vector<std::unique_ptr<ShaderNode> > body;

        NodeLoop(std::unique_ptr<ShaderNode> initializer,
                 std::unique_ptr<ShaderNode> predicate,
                 std::unique_ptr<ShaderNode> iterator,
                 std::vector<std::unique_ptr<ShaderNode> > body)
            : initializer(std::move(initializer)),
              predicate(std::move(predicate)),
              iterator(std::move(iterator)),
              body(std::move(body)) {
        }

        NodeType getType() const override {
            return LOOP;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            std::vector<std::unique_ptr<ShaderNode> > bodyCopy;
            bodyCopy.reserve(body.size());
            for (auto &node: body) {
                bodyCopy.push_back(node->copy());
            }
            return std::make_unique<NodeLoop>(initializer ? initializer->copy() : nullptr,
                                              predicate ? predicate->copy() : nullptr,
                                              iterator ? iterator->copy() : nullptr,
                                              std::move(bodyCopy));
        }
    };
}

#endif //XENGINE_NODELOOP_HPP
