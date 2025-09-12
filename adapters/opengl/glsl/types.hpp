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

#ifndef XENGINE_TYPES_HPP
#define XENGINE_TYPES_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"

using namespace xng;

static std::string getTypeName(const ShaderDataType &value) {
    std::string ret;
    switch (value.type) {
        case ShaderDataType::SCALAR:
            switch (value.component) {
                case ShaderDataType::BOOLEAN:
                    return "bool";
                case ShaderDataType::UNSIGNED_INT:
                    return "uint";
                case ShaderDataType::SIGNED_INT:
                    return "int";
                case ShaderDataType::FLOAT:
                    return "float";
                case ShaderDataType::DOUBLE:
                    return "double";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::VECTOR2:
            switch (value.component) {
                case ShaderDataType::BOOLEAN:
                    return "bvec2";
                case ShaderDataType::UNSIGNED_INT:
                    return "uvec2";
                case ShaderDataType::SIGNED_INT:
                    return "ivec2";
                case ShaderDataType::FLOAT:
                    return "vec2";
                case ShaderDataType::DOUBLE:
                    return "dvec2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::VECTOR3:
            switch (value.component) {
                case ShaderDataType::BOOLEAN:
                    return "bvec3";
                case ShaderDataType::UNSIGNED_INT:
                    return "uvec3";
                case ShaderDataType::SIGNED_INT:
                    return "ivec3";
                case ShaderDataType::FLOAT:
                    return "vec3";
                case ShaderDataType::DOUBLE:
                    return "dvec3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::VECTOR4:
            switch (value.component) {
                case ShaderDataType::BOOLEAN:
                    return "bvec4";
                case ShaderDataType::UNSIGNED_INT:
                    return "uvec4";
                case ShaderDataType::SIGNED_INT:
                    return "ivec4";
                case ShaderDataType::FLOAT:
                    return "vec4";
                case ShaderDataType::DOUBLE:
                    return "dvec4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::MAT2:
            switch (value.component) {
                case ShaderDataType::FLOAT:
                    return "mat2";
                case ShaderDataType::DOUBLE:
                    return "dmat2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::MAT3:
            switch (value.component) {
                case ShaderDataType::FLOAT:
                    return "mat3";
                case ShaderDataType::DOUBLE:
                    return "dmat3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderDataType::MAT4:
            switch (value.component) {
                case ShaderDataType::FLOAT:
                    return "mat4";
                case ShaderDataType::DOUBLE:
                    return "dmat4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        default:
            throw std::runtime_error("Invalid type");
    }
}

#endif //XENGINE_TYPES_HPP