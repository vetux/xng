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

#ifndef XENGINE_SHADERDATATYPE_HPP
#define XENGINE_SHADERDATATYPE_HPP

#include <stdexcept>

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

namespace xng {
    struct ShaderDataType {
        enum Type : int {
            SCALAR,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MAT2,
            MAT3,
            MAT4
        };

        enum Component : int {
            BOOLEAN = 0, // 1 Byte boolean
            UNSIGNED_INT, // 4 Byte unsigned
            SIGNED_INT, // 4 Byte signed
            FLOAT, // 4 Byte float
            DOUBLE, // 8 Byte double
        };

        static int getBytes(Component type) {
            switch (type) {
                case BOOLEAN:
                    return 1;
                case UNSIGNED_INT:
                case SIGNED_INT:
                case FLOAT:
                    return 4;
                case DOUBLE:
                    return 8;
                default:
                    throw std::runtime_error("Invalid component");
            }
        }

        static int getCount(Type count) {
            switch (count) {
                case SCALAR:
                    return 1;
                case VECTOR2:
                    return 2;
                case VECTOR3:
                    return 3;
                case VECTOR4:
                case MAT2:
                    return 4;
                case MAT3:
                    return 9;
                case MAT4:
                    return 16;
                default:
                    throw std::runtime_error("Invalid type");
            }
        }

        size_t stride() const {
            return (getBytes(component) * getCount(type)) * count;
        }

        ShaderDataType() = default;

        ShaderDataType(const Type type, const Component component, const size_t count = 1)
            : type(type),
              component(component),
              count(count) {
        }

        bool operator==(const ShaderDataType &other) const {
            return type == other.type && component == other.component && count == other.count;
        }

        bool operator!=(const ShaderDataType &other) const {
            return !(*this == other);
        }

        Type type{};
        Component component{};
        size_t count = 1; // If larger than 1, this element is a fixed size array

        static ShaderDataType boolean() {
            return {SCALAR, BOOLEAN};
        }

        static ShaderDataType integer() {
            return {SCALAR, SIGNED_INT};
        }

        static ShaderDataType unsignedInteger() {
            return {SCALAR, UNSIGNED_INT};
        }

        static ShaderDataType float32() {
            return {SCALAR, FLOAT};
        }

        static ShaderDataType float64() {
            return {SCALAR, DOUBLE};
        }

        static ShaderDataType bvec2() {
            return {VECTOR2, BOOLEAN};
        }

        static ShaderDataType bvec3() {
            return {VECTOR3, BOOLEAN};
        }

        static ShaderDataType bvec4() {
            return {VECTOR4, BOOLEAN};
        }

        static ShaderDataType ivec2() {
            return {VECTOR2, SIGNED_INT};
        }

        static ShaderDataType ivec3() {
            return {VECTOR3, SIGNED_INT};
        }

        static ShaderDataType ivec4() {
            return {VECTOR4, SIGNED_INT};
        }

        static ShaderDataType uvec2() {
            return {VECTOR2, UNSIGNED_INT};
        }

        static ShaderDataType uvec3() {
            return {VECTOR3, UNSIGNED_INT};
        }

        static ShaderDataType uvec4() {
            return {VECTOR4, UNSIGNED_INT};
        }

        static ShaderDataType vec2() {
            return {VECTOR2, FLOAT};
        }

        static ShaderDataType vec3() {
            return {VECTOR3, FLOAT};
        }

        static ShaderDataType vec4() {
            return {VECTOR4, FLOAT};
        }

        static ShaderDataType mat2() {
            return {MAT2, FLOAT};
        }

        static ShaderDataType mat3() {
            return {MAT3, FLOAT};
        }

        static ShaderDataType mat4() {
            return {MAT4, FLOAT};
        }

        static ShaderDataType dvec2() {
            return {VECTOR2, DOUBLE};
        }

        static ShaderDataType dvec3() {
            return {VECTOR3, DOUBLE};
        }

        static ShaderDataType dvec4() {
            return {VECTOR4, DOUBLE};
        }

        static ShaderDataType dmat2() {
            return {MAT2, DOUBLE};
        }

        static ShaderDataType dmat3() {
            return {MAT3, DOUBLE};
        }

        static ShaderDataType dmat4() {
            return {MAT4, DOUBLE};
        }

        static ShaderDataType array(const ShaderDataType &type, const size_t count) {
            return {type.type, type.component, count};
        }

        static Component getColorComponent(const ColorFormat format) {
            if (format >= R && format <= RGBA32F) {
                return FLOAT;
            } else if (format >= R8I && format <= RGBA32I) {
                return SIGNED_INT;
            } else {
                return UNSIGNED_INT;
            }
        }
    };
}

#endif //XENGINE_SHADERDATATYPE_HPP
