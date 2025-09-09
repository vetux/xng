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

#include "xng/render/graph2/shader/fgshadervalue.hpp"

using namespace xng;

static std::string getTypeName(const FGShaderValue &value) {
    std::string ret;
    switch (value.type) {
        case FGShaderValue::SCALAR:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bool";
                case FGShaderValue::UNSIGNED_INT:
                    return "uint";
                case FGShaderValue::SIGNED_INT:
                    return "int";
                case FGShaderValue::FLOAT:
                    return "float";
                case FGShaderValue::DOUBLE:
                    return "double";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR2:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec2";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec2";
                case FGShaderValue::SIGNED_INT:
                    return "ivec2";
                case FGShaderValue::FLOAT:
                    return "vec2";
                case FGShaderValue::DOUBLE:
                    return "dvec2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR3:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec3";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec3";
                case FGShaderValue::SIGNED_INT:
                    return "ivec3";
                case FGShaderValue::FLOAT:
                    return "vec3";
                case FGShaderValue::DOUBLE:
                    return "dvec3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR4:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec4";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec4";
                case FGShaderValue::SIGNED_INT:
                    return "ivec4";
                case FGShaderValue::FLOAT:
                    return "vec4";
                case FGShaderValue::DOUBLE:
                    return "dvec4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT2:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat2";
                case FGShaderValue::DOUBLE:
                    return "dmat2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT3:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat3";
                case FGShaderValue::DOUBLE:
                    return "dmat3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT4:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat4";
                case FGShaderValue::DOUBLE:
                    return "dmat4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        default:
            throw std::runtime_error("Invalid type");
    }
}

#endif //XENGINE_TYPES_HPP