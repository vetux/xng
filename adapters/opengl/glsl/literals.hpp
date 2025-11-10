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

#ifndef XENGINE_LITERALS_HPP
#define XENGINE_LITERALS_HPP

#include "xng/rendergraph/shader/shaderprimitive.hpp"

using namespace xng;

inline std::string boolToString(bool b) {
    return b ? "true" : "false";
}

static std::string literalToString(const ShaderPrimitive &value) {
    auto type = value.getType();

    switch (type.type) {
        case ShaderPrimitiveType::SCALAR:
            switch (type.component) {
                case ShaderPrimitiveType::BOOLEAN:
                    return boolToString(std::get<bool>(value.value));
                case ShaderPrimitiveType::UNSIGNED_INT:
                    return std::to_string(std::get<unsigned int>(value.value));
                case ShaderPrimitiveType::SIGNED_INT:
                    return std::to_string(std::get<int>(value.value));
                case ShaderPrimitiveType::FLOAT:
                    return std::to_string(std::get<float>(value.value));
                case ShaderPrimitiveType::DOUBLE:
                    return std::to_string(std::get<double>(value.value));
            }
            break;
        case ShaderPrimitiveType::VECTOR2:
            switch (type.component) {
                case ShaderPrimitiveType::BOOLEAN: {
                    auto vec = std::get<Vector2<bool> >(value.value);
                    return "bvec2(" + boolToString(vec.x) + ", " + boolToString(vec.y) + ")";
                }
                case ShaderPrimitiveType::UNSIGNED_INT: {
                    auto vecu = std::get<Vector2<unsigned int> >(value.value);
                    return "uvec2(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ")";
                }
                case ShaderPrimitiveType::SIGNED_INT: {
                    auto veci = std::get<Vector2<int> >(value.value);
                    return "ivec2(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ")";
                }
                case ShaderPrimitiveType::FLOAT: {
                    auto vecf = std::get<Vector2<float> >(value.value);
                    return "vec2(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto vecd = std::get<Vector2<double> >(value.value);
                    return "dvec2(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ")";
                }
            }
        case ShaderPrimitiveType::VECTOR3:
            switch (type.component) {
                case ShaderPrimitiveType::BOOLEAN: {
                    auto vec = std::get<Vector3<bool> >(value.value);
                    return "bvec3(" + boolToString(vec.x) + ", " + boolToString(vec.y) + ", " +
                           std::to_string(vec.z) + ")";
                }
                case ShaderPrimitiveType::UNSIGNED_INT: {
                    auto vecu = std::get<Vector3<unsigned int> >(value.value);
                    return "uvec3(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ")";
                }
                case ShaderPrimitiveType::SIGNED_INT: {
                    auto veci = std::get<Vector3<int> >(value.value);
                    return "ivec3(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ")";
                }
                case ShaderPrimitiveType::FLOAT: {
                    auto vecf = std::get<Vector3<float> >(value.value);
                    return "vec3(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto vecd = std::get<Vector3<double> >(value.value);
                    return "dvec3(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ")";
                }
            }
        case ShaderPrimitiveType::VECTOR4:
            switch (type.component) {
                case ShaderPrimitiveType::BOOLEAN: {
                    auto vec = std::get<Vector4<bool> >(value.value);
                    return "bvec4(" + boolToString(vec.x) + ", " + boolToString(vec.y) + ", " +
                           boolToString(vec.z) + ", " + boolToString(vec.w) + ")";
                }
                case ShaderPrimitiveType::UNSIGNED_INT: {
                    auto vecu = std::get<Vector4<unsigned int> >(value.value);
                    return "uvec4(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ", " + std::to_string(vecu.w) + ")";
                }
                case ShaderPrimitiveType::SIGNED_INT: {
                    auto veci = std::get<Vector4<int> >(value.value);
                    return "ivec4(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ", " + std::to_string(veci.w) + ")";
                }
                case ShaderPrimitiveType::FLOAT: {
                    auto vecf = std::get<Vector4<float> >(value.value);
                    return "vec4(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ", " + std::to_string(vecf.w) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto vecd = std::get<Vector4<double> >(value.value);
                    return "dvec4(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ", " + std::to_string(vecd.w) + ")";
                }
            }
        case ShaderPrimitiveType::MAT2:
            switch (type.component) {
                case ShaderPrimitiveType::FLOAT: {
                    auto mat2 = std::get<Matrix<float, 2, 2> >(value.value);
                    return "mat2(" + std::to_string(mat2.get(0, 0)) + ", " + std::to_string(mat2.get(0, 1)) + ", " +
                           std::to_string(mat2.get(1, 0)) + ", " + std::to_string(mat2.get(1, 1)) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto mat2d = std::get<Matrix<double, 2, 2> >(value.value);
                    return "dmat2(" + std::to_string(mat2d.get(0, 0)) + ", " + std::to_string(mat2d.get(0, 1)) + ", " +
                           std::to_string(mat2d.get(1, 0)) + ", " + std::to_string(mat2d.get(1, 1)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case ShaderPrimitiveType::MAT3:
            switch (type.component) {
                case ShaderPrimitiveType::FLOAT: {
                    auto mat3 = std::get<Matrix<float, 3, 3> >(value.value);
                    return "mat3(" + std::to_string(mat3.get(0, 0)) + ", " + std::to_string(mat3.get(0, 1)) + ", " +
                           std::to_string(mat3.get(0, 2)) + ", " + std::to_string(mat3.get(1, 0)) + ", " +
                           std::to_string(mat3.get(1, 1)) + ", " + std::to_string(mat3.get(1, 2)) + ", " +
                           std::to_string(mat3.get(2, 0)) + ", " + std::to_string(mat3.get(2, 1)) + ", " +
                           std::to_string(mat3.get(2, 2)) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto mat3d = std::get<Matrix<double, 3, 3> >(value.value);
                    return "dmat3(" + std::to_string(mat3d.get(0, 0)) + ", " + std::to_string(mat3d.get(0, 1)) + ", " +
                           std::to_string(mat3d.get(0, 2)) + ", " + std::to_string(mat3d.get(1, 0)) + ", " +
                           std::to_string(mat3d.get(1, 1)) + ", " + std::to_string(mat3d.get(1, 2)) + ", " +
                           std::to_string(mat3d.get(2, 0)) + ", " + std::to_string(mat3d.get(2, 1)) + ", " +
                           std::to_string(mat3d.get(2, 2)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case ShaderPrimitiveType::MAT4:
            switch (type.component) {
                case ShaderPrimitiveType::FLOAT: {
                    auto mat4 = std::get<Matrix<float, 4, 4> >(value.value);
                    return "mat4(" + std::to_string(mat4.get(0, 0)) + ", " + std::to_string(mat4.get(0, 1)) + ", " +
                           std::to_string(mat4.get(0, 2)) + ", " + std::to_string(mat4.get(0, 3)) + ", " +
                           std::to_string(mat4.get(1, 0)) + ", " + std::to_string(mat4.get(1, 1)) + ", " +
                           std::to_string(mat4.get(1, 2)) + ", " + std::to_string(mat4.get(1, 3)) + ", " +
                           std::to_string(mat4.get(2, 0)) + ", " + std::to_string(mat4.get(2, 1)) + ", " +
                           std::to_string(mat4.get(2, 2)) + ", " + std::to_string(mat4.get(2, 3)) + ", " +
                           std::to_string(mat4.get(3, 0)) + ", " + std::to_string(mat4.get(3, 1)) + ", " +
                           std::to_string(mat4.get(3, 2)) + ", " + std::to_string(mat4.get(3, 3)) + ")";
                }
                case ShaderPrimitiveType::DOUBLE: {
                    auto mat4d = std::get<Matrix<double, 4, 4> >(value.value);
                    return "dmat4(" + std::to_string(mat4d.get(0, 0)) + ", " + std::to_string(mat4d.get(0, 1)) + ", " +
                           std::to_string(mat4d.get(0, 2)) + ", " + std::to_string(mat4d.get(0, 3)) + ", " +
                           std::to_string(mat4d.get(1, 0)) + ", " + std::to_string(mat4d.get(1, 1)) + ", " +
                           std::to_string(mat4d.get(1, 2)) + ", " + std::to_string(mat4d.get(1, 3)) + ", " +
                           std::to_string(mat4d.get(2, 0)) + ", " + std::to_string(mat4d.get(2, 1)) + ", " +
                           std::to_string(mat4d.get(2, 2)) + ", " + std::to_string(mat4d.get(2, 3)) + ", " +
                           std::to_string(mat4d.get(3, 0)) + ", " + std::to_string(mat4d.get(3, 1)) + ", " +
                           std::to_string(mat4d.get(3, 2)) + ", " + std::to_string(mat4d.get(3, 3)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
    }
    throw std::runtime_error("Invalid type");
}

#endif //XENGINE_LITERALS_HPP
