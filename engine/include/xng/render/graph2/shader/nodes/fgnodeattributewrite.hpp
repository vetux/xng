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

#ifndef XENGINE_FGNODEATTRIBUTEWRITE_HPP
#define XENGINE_FGNODEATTRIBUTEWRITE_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    /**
     * Write to the output attribute for this given shader stage.
     * For fragment shaders the output attributes represent the bound textures in the context.
     */
    struct FGNodeAttributeWrite final : FGShaderNode {
        uint32_t attributeIndex = 0;

        std::unique_ptr<FGShaderNode> value;

        explicit FGNodeAttributeWrite(const uint32_t attribute_index, std::unique_ptr<FGShaderNode> value)
            : attributeIndex(attribute_index), value(std::move(value)) {
        }

        NodeType getType() const override {
            return ATTRIBUTE_WRITE;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeAttributeWrite>(attributeIndex, value->copy());
        }
    };
}

#endif //XENGINE_FGNODEATTRIBUTEWRITE_HPP
