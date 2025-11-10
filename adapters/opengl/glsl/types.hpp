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

#ifndef XENGINE_TYPES_HPP
#define XENGINE_TYPES_HPP

#include "xng/rendergraph/shader/shaderprimitive.hpp"

using namespace xng;

static std::string getTypeName(const ShaderPrimitiveType &value) {
    std::string ret;
    switch (value.type) {
        case ShaderPrimitiveType::SCALAR:
            switch (value.component) {
                case ShaderPrimitiveType::BOOLEAN:
                    return "bool";
                case ShaderPrimitiveType::UNSIGNED_INT:
                    return "uint";
                case ShaderPrimitiveType::SIGNED_INT:
                    return "int";
                case ShaderPrimitiveType::FLOAT:
                    return "float";
                case ShaderPrimitiveType::DOUBLE:
                    return "double";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::VECTOR2:
            switch (value.component) {
                case ShaderPrimitiveType::BOOLEAN:
                    return "bvec2";
                case ShaderPrimitiveType::UNSIGNED_INT:
                    return "uvec2";
                case ShaderPrimitiveType::SIGNED_INT:
                    return "ivec2";
                case ShaderPrimitiveType::FLOAT:
                    return "vec2";
                case ShaderPrimitiveType::DOUBLE:
                    return "dvec2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::VECTOR3:
            switch (value.component) {
                case ShaderPrimitiveType::BOOLEAN:
                    return "bvec3";
                case ShaderPrimitiveType::UNSIGNED_INT:
                    return "uvec3";
                case ShaderPrimitiveType::SIGNED_INT:
                    return "ivec3";
                case ShaderPrimitiveType::FLOAT:
                    return "vec3";
                case ShaderPrimitiveType::DOUBLE:
                    return "dvec3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::VECTOR4:
            switch (value.component) {
                case ShaderPrimitiveType::BOOLEAN:
                    return "bvec4";
                case ShaderPrimitiveType::UNSIGNED_INT:
                    return "uvec4";
                case ShaderPrimitiveType::SIGNED_INT:
                    return "ivec4";
                case ShaderPrimitiveType::FLOAT:
                    return "vec4";
                case ShaderPrimitiveType::DOUBLE:
                    return "dvec4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::MAT2:
            switch (value.component) {
                case ShaderPrimitiveType::FLOAT:
                    return "mat2";
                case ShaderPrimitiveType::DOUBLE:
                    return "dmat2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::MAT3:
            switch (value.component) {
                case ShaderPrimitiveType::FLOAT:
                    return "mat3";
                case ShaderPrimitiveType::DOUBLE:
                    return "dmat3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case ShaderPrimitiveType::MAT4:
            switch (value.component) {
                case ShaderPrimitiveType::FLOAT:
                    return "mat4";
                case ShaderPrimitiveType::DOUBLE:
                    return "dmat4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        default:
            throw std::runtime_error("Invalid type");
    }
}

inline std::string getSampler(const ShaderTexture &texture) {
    std::string prefix;
    if (texture.format >= R8I && texture.format <= RGBA32I) {
        prefix = "i";
    } else if (texture.format >= R8UI && texture.format <= RGBA32UI) {
        prefix = "u";
    }
    switch (texture.type) {
        case TEXTURE_2D:
            return prefix + "sampler2D";
        case TEXTURE_2D_MULTISAMPLE:
            return prefix + "sampler2DMS";
        case TEXTURE_CUBE_MAP:
            return prefix + "samplerCube";
        case TEXTURE_2D_ARRAY:
            return prefix + "sampler2DArray";
        case TEXTURE_2D_MULTISAMPLE_ARRAY:
            return prefix + "sampler2DMSArray";
        case TEXTURE_CUBE_MAP_ARRAY:
            return prefix + "samplerCubeArray";
        default:
            throw std::runtime_error("Unrecognized texture type");
    }
}

#endif //XENGINE_TYPES_HPP
