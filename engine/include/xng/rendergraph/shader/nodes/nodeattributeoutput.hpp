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

#ifndef XENGINE_NODEATTRIBUTEOUTPUT_HPP
#define XENGINE_NODEATTRIBUTEOUTPUT_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeAttributeOutput final : ShaderNode {
        uint32_t attributeIndex = 0;

        explicit NodeAttributeOutput(const uint32_t attribute_index)
            : attributeIndex(attribute_index) {
        }

        NodeType getType() const override {
            return ATTRIBUTE_OUT;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeAttributeOutput>(attributeIndex);
        }
    };
}
#endif //XENGINE_NODEATTRIBUTEOUTPUT_HPP