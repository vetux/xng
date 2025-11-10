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

#ifndef XENGINE_SHADERPRIMITIVE_HPP
#define XENGINE_SHADERPRIMITIVE_HPP

#include <variant>

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

namespace xng {
    struct ShaderPrimitiveType {
        enum VectorComponent : int {
            COMPONENT_x = 0,
            COMPONENT_y,
            COMPONENT_z,
            COMPONENT_w
        };

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

        static int getBytes(const Component type) {
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

        static int getCount(const Type count) {
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
            return getBytes(component) * getCount(type);
        }

        ShaderPrimitiveType() = default;

        ShaderPrimitiveType(const Type type, const Component component)
            : type(type),
              component(component) {
        }

        bool operator==(const ShaderPrimitiveType &other) const {
            return type == other.type && component == other.component;
        }

        bool operator!=(const ShaderPrimitiveType &other) const {
            return !(*this == other);
        }

        Type type{};
        Component component{};

        static ShaderPrimitiveType Bool() {
            return {SCALAR, BOOLEAN};
        }

        static ShaderPrimitiveType Int() {
            return {SCALAR, SIGNED_INT};
        }

        static ShaderPrimitiveType UInt() {
            return {SCALAR, UNSIGNED_INT};
        }

        static ShaderPrimitiveType Float() {
            return {SCALAR, FLOAT};
        }

        static ShaderPrimitiveType Double() {
            return {SCALAR, DOUBLE};
        }

        static ShaderPrimitiveType bvec2() {
            return {VECTOR2, BOOLEAN};
        }

        static ShaderPrimitiveType bvec3() {
            return {VECTOR3, BOOLEAN};
        }

        static ShaderPrimitiveType bvec4() {
            return {VECTOR4, BOOLEAN};
        }

        static ShaderPrimitiveType ivec2() {
            return {VECTOR2, SIGNED_INT};
        }

        static ShaderPrimitiveType ivec3() {
            return {VECTOR3, SIGNED_INT};
        }

        static ShaderPrimitiveType ivec4() {
            return {VECTOR4, SIGNED_INT};
        }

        static ShaderPrimitiveType uvec2() {
            return {VECTOR2, UNSIGNED_INT};
        }

        static ShaderPrimitiveType uvec3() {
            return {VECTOR3, UNSIGNED_INT};
        }

        static ShaderPrimitiveType uvec4() {
            return {VECTOR4, UNSIGNED_INT};
        }

        static ShaderPrimitiveType vec2() {
            return {VECTOR2, FLOAT};
        }

        static ShaderPrimitiveType vec3() {
            return {VECTOR3, FLOAT};
        }

        static ShaderPrimitiveType vec4() {
            return {VECTOR4, FLOAT};
        }

        static ShaderPrimitiveType mat2() {
            return {MAT2, FLOAT};
        }

        static ShaderPrimitiveType mat3() {
            return {MAT3, FLOAT};
        }

        static ShaderPrimitiveType mat4() {
            return {MAT4, FLOAT};
        }

        static ShaderPrimitiveType dvec2() {
            return {VECTOR2, DOUBLE};
        }

        static ShaderPrimitiveType dvec3() {
            return {VECTOR3, DOUBLE};
        }

        static ShaderPrimitiveType dvec4() {
            return {VECTOR4, DOUBLE};
        }

        static ShaderPrimitiveType dmat2() {
            return {MAT2, DOUBLE};
        }

        static ShaderPrimitiveType dmat3() {
            return {MAT3, DOUBLE};
        }

        static ShaderPrimitiveType dmat4() {
            return {MAT4, DOUBLE};
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

        static ShaderPrimitiveType fromString(const std::string &typeStr) {
            if (typeStr == "Bool") return Bool();
            if (typeStr == "Int") return Int();
            if (typeStr == "UInt") return UInt();
            if (typeStr == "Float") return Float();
            if (typeStr == "Double") return Double();
            if (typeStr == "bvec2") return bvec2();
            if (typeStr == "bvec3") return bvec3();
            if (typeStr == "bvec4") return bvec4();
            if (typeStr == "ivec2") return ivec2();
            if (typeStr == "ivec3") return ivec3();
            if (typeStr == "ivec4") return ivec4();
            if (typeStr == "uvec2") return uvec2();
            if (typeStr == "uvec3") return uvec3();
            if (typeStr == "uvec4") return uvec4();
            if (typeStr == "vec2") return vec2();
            if (typeStr == "vec3") return vec3();
            if (typeStr == "vec4") return vec4();
            if (typeStr == "mat2") return mat2();
            if (typeStr == "mat3") return mat3();
            if (typeStr == "mat4") return mat4();
            if (typeStr == "dvec2") return dvec2();
            if (typeStr == "dvec3") return dvec3();
            if (typeStr == "dvec4") return dvec4();
            if (typeStr == "dmat2") return dmat2();
            if (typeStr == "dmat3") return dmat3();
            if (typeStr == "dmat4") return dmat4();
            throw std::runtime_error("Invalid type");
        }
    };

    struct ShaderPrimitive {
        typedef std::variant<bool,
            unsigned int,
            int,
            float,
            double,

            Vector2<bool>,
            Vector2<unsigned int>,
            Vector2<int>,
            Vector2<float>,
            Vector2<double>,

            Vector3<bool>,
            Vector3<unsigned int>,
            Vector3<int>,
            Vector3<float>,
            Vector3<double>,

            Vector4<bool>,
            Vector4<unsigned int>,
            Vector4<int>,
            Vector4<float>,
            Vector4<double>,

            Matrix<float, 2, 2>,
            Matrix<double, 2, 2>,

            Matrix<float, 3, 3>,
            Matrix<double, 3, 3>,

            Matrix<float, 4, 4>,
            Matrix<double, 4, 4> > Value;

        Value value;

        ShaderPrimitive() = default;

        ShaderPrimitive(Value value)
            : value(std::move(value)) {
        }

        ShaderPrimitiveType getType() const;
    };
}

#endif //XENGINE_SHADERPRIMITIVE_HPP
