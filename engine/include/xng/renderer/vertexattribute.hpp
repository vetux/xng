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

#ifndef XENGINE_VERTEXATTRIBUTE_HPP
#define XENGINE_VERTEXATTRIBUTE_HPP

#include <cstddef>

namespace xng {
    enum VertexAttribute : int {
        POSITION = 0,
        NORMAL,
        TANGENT,
        BITANGENT,
        UV,
        ATTRIBUTE_BEGIN = POSITION,
        ATTRIBUTE_END = UV
    };

    inline size_t getVertexAttributeSize(const VertexAttribute attribute) {
        switch (attribute) {
            default:
                return sizeof(float) * 3;
            case UV:
                return sizeof(float) * 2;
        }
    }

    inline rg::ShaderPrimitiveType getVertexAttributeType(const VertexAttribute attribute) {
        switch (attribute) {
            case POSITION:
                return rg::ShaderPrimitiveType::vec3();
            case NORMAL:
                return rg::ShaderPrimitiveType::vec3();
            case TANGENT:
                return rg::ShaderPrimitiveType::vec3();
            case BITANGENT:
                return rg::ShaderPrimitiveType::vec3();
            case UV:
                return rg::ShaderPrimitiveType::vec2();
            default:
                throw std::runtime_error("Unknown vertex attribute.");
        }
    }
}

#endif //XENGINE_VERTEXATTRIBUTE_HPP
