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

#ifndef XENGINE_LITERALS_HPP
#define XENGINE_LITERALS_HPP

#include "xng/rendergraph/shader/fgshaderliteral.hpp"
#include "xng/rendergraph/shader/fgshadervalue.hpp"

using namespace xng;

static std::string literalToString(const FGShaderLiteral &value) {
    auto type = getLiteralType(value);

    switch (type.type) {
        case FGShaderValue::SCALAR:
            switch (type.component) {
                case FGShaderValue::BOOLEAN:
                    return std::to_string(std::get<bool>(value));
                case FGShaderValue::UNSIGNED_INT:
                    return std::to_string(std::get<unsigned int>(value));
                case FGShaderValue::SIGNED_INT:
                    return std::to_string(std::get<int>(value));
                case FGShaderValue::FLOAT:
                    return std::to_string(std::get<float>(value));
                case FGShaderValue::DOUBLE:
                    return std::to_string(std::get<double>(value));
            }
            break;
        case FGShaderValue::VECTOR2:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector2<bool> >(value);
                    return "bvec2(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector2<unsigned int> >(value);
                    return "uvec2(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector2<int> >(value);
                    return "ivec2(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector2<float> >(value);
                    return "vec2(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector2<double> >(value);
                    return "dvec2(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ")";
                }
            }
        case FGShaderValue::VECTOR3:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector3<bool> >(value);
                    return "bvec3(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +
                           std::to_string(vec.z) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector3<unsigned int> >(value);
                    return "uvec3(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector3<int> >(value);
                    return "ivec3(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector3<float> >(value);
                    return "vec3(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector3<double> >(value);
                    return "dvec3(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ")";
                }
            }
        case FGShaderValue::VECTOR4:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector4<bool> >(value);
                    return "bvec4(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +
                           std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector4<unsigned int> >(value);
                    return "uvec4(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ", " + std::to_string(vecu.w) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector4<int> >(value);
                    return "ivec4(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ", " + std::to_string(veci.w) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector4<float> >(value);
                    return "vec4(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ", " + std::to_string(vecf.w) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector4<double> >(value);
                    return "dvec4(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ", " + std::to_string(vecd.w) + ")";
                }
            }
        case FGShaderValue::MAT2:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat2 = std::get<Matrix<float, 2, 2> >(value);
                    return "mat2(" + std::to_string(mat2.get(0, 0)) + ", " + std::to_string(mat2.get(0, 1)) + ", " +
                           std::to_string(mat2.get(1, 0)) + ", " + std::to_string(mat2.get(1, 1)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat2d = std::get<Matrix<double, 2, 2> >(value);
                    return "dmat2(" + std::to_string(mat2d.get(0, 0)) + ", " + std::to_string(mat2d.get(0, 1)) + ", " +
                           std::to_string(mat2d.get(1, 0)) + ", " + std::to_string(mat2d.get(1, 1)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case FGShaderValue::MAT3:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat3 = std::get<Matrix<float, 3, 3> >(value);
                    return "mat3(" + std::to_string(mat3.get(0, 0)) + ", " + std::to_string(mat3.get(0, 1)) + ", " +
                           std::to_string(mat3.get(0, 2)) + ", " + std::to_string(mat3.get(1, 0)) + ", " +
                           std::to_string(mat3.get(1, 1)) + ", " + std::to_string(mat3.get(1, 2)) + ", " +
                           std::to_string(mat3.get(2, 0)) + ", " + std::to_string(mat3.get(2, 1)) + ", " +
                           std::to_string(mat3.get(2, 2)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat3d = std::get<Matrix<double, 3, 3> >(value);
                    return "dmat3(" + std::to_string(mat3d.get(0, 0)) + ", " + std::to_string(mat3d.get(0, 1)) + ", " +
                           std::to_string(mat3d.get(0, 2)) + ", " + std::to_string(mat3d.get(1, 0)) + ", " +
                           std::to_string(mat3d.get(1, 1)) + ", " + std::to_string(mat3d.get(1, 2)) + ", " +
                           std::to_string(mat3d.get(2, 0)) + ", " + std::to_string(mat3d.get(2, 1)) + ", " +
                           std::to_string(mat3d.get(2, 2)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case FGShaderValue::MAT4:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat4 = std::get<Matrix<float, 4, 4> >(value);
                    return "mat4(" + std::to_string(mat4.get(0, 0)) + ", " + std::to_string(mat4.get(0, 1)) + ", " +
                           std::to_string(mat4.get(0, 2)) + ", " + std::to_string(mat4.get(0, 3)) + ", " +
                           std::to_string(mat4.get(1, 0)) + ", " + std::to_string(mat4.get(1, 1)) + ", " +
                           std::to_string(mat4.get(1, 2)) + ", " + std::to_string(mat4.get(1, 3)) + ", " +
                           std::to_string(mat4.get(2, 0)) + ", " + std::to_string(mat4.get(2, 1)) + ", " +
                           std::to_string(mat4.get(2, 2)) + ", " + std::to_string(mat4.get(2, 3)) + ", " +
                           std::to_string(mat4.get(3, 0)) + ", " + std::to_string(mat4.get(3, 1)) + ", " +
                           std::to_string(mat4.get(3, 2)) + ", " + std::to_string(mat4.get(3, 3)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat4d = std::get<Matrix<double, 4, 4> >(value);
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
