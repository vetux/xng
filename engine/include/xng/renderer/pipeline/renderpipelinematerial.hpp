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

#ifndef XENGINE_RENDERPIPELINEMATERIAL_HPP
#define XENGINE_RENDERPIPELINEMATERIAL_HPP

#include <cstdint>
#include <variant>

#include "xng/renderer/objects/rendertexture.hpp"

namespace xng {
    /**
     * A pipeline material is data that is per draw.
     */
    class RenderPipelineMaterial {
    public:
        typedef size_t AttributeID;

        struct AttributeType {
            enum Type : int {
                ATTRIBUTE_TEXTURE = 0,
                ATTRIBUTE_PRIMITIVE,
            } type{};

            rg::ShaderPrimitiveType primitiveType{};

            static AttributeType texture() {
                AttributeType ret;
                ret.type = ATTRIBUTE_TEXTURE;
                return ret;
            }

            static AttributeType primitive(const rg::ShaderPrimitiveType type) {
                AttributeType ret;
                ret.type = ATTRIBUTE_PRIMITIVE;
                ret.primitiveType = type;
                return ret;
            }
        };

        virtual ~RenderPipelineMaterial() = default;

        virtual void setValue(AttributeID attribute, RenderObjectHandle<RenderTexture> texture) = 0;

        virtual void setValue(AttributeID attribute, rg::ShaderPrimitive value) = 0;

        virtual const std::unordered_map<AttributeID, AttributeType> &getAttributes() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINEMATERIAL_HPP
