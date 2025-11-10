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

#ifndef XENGINE_SHADERDATATYPE_HPP
#define XENGINE_SHADERDATATYPE_HPP

#include "xng/rendergraph/shader/shaderprimitive.hpp"

namespace xng {
    typedef std::string ShaderStructType; // The name of the struct type

    struct ShaderDataType {
        std::variant<ShaderPrimitiveType, ShaderStructType> value{};
        size_t count = 1; // If larger than 1, this type is a fixed size array

        ShaderDataType() = default;

        ShaderDataType(const std::variant<ShaderPrimitiveType, ShaderStructType> &type, size_t count = 1)
            : value(type), count(count) {
        }

        const ShaderPrimitiveType &getPrimitive() const {
            return std::get<ShaderPrimitiveType>(value);
        }

        const ShaderStructType &getStruct() const {
            return std::get<ShaderStructType>(value);
        }

        static ShaderDataType fromString(const std::string &typeName) {
            if (typeName == "Bool") return ShaderDataType(ShaderPrimitiveType::Bool());
            if (typeName == "Int") return ShaderDataType(ShaderPrimitiveType::Int());
            if (typeName == "UInt") return ShaderDataType(ShaderPrimitiveType::UInt());
            if (typeName == "Float") return ShaderDataType(ShaderPrimitiveType::Float());
            if (typeName == "Double") return ShaderDataType(ShaderPrimitiveType::Double());
            if (typeName == "bvec2") return ShaderDataType(ShaderPrimitiveType::bvec2());
            if (typeName == "bvec3") return ShaderDataType(ShaderPrimitiveType::bvec3());
            if (typeName == "bvec4") return ShaderDataType(ShaderPrimitiveType::bvec4());
            if (typeName == "ivec2") return ShaderDataType(ShaderPrimitiveType::ivec2());
            if (typeName == "ivec3") return ShaderDataType(ShaderPrimitiveType::ivec3());
            if (typeName == "ivec4") return ShaderDataType(ShaderPrimitiveType::ivec4());
            if (typeName == "uvec2") return ShaderDataType(ShaderPrimitiveType::uvec2());
            if (typeName == "uvec3") return ShaderDataType(ShaderPrimitiveType::uvec3());
            if (typeName == "uvec4") return ShaderDataType(ShaderPrimitiveType::uvec4());
            if (typeName == "vec2") return ShaderDataType(ShaderPrimitiveType::vec2());
            if (typeName == "vec3") return ShaderDataType(ShaderPrimitiveType::vec3());
            if (typeName == "vec4") return ShaderDataType(ShaderPrimitiveType::vec4());
            if (typeName == "dvec2") return ShaderDataType(ShaderPrimitiveType::dvec2());
            if (typeName == "dvec3") return ShaderDataType(ShaderPrimitiveType::dvec3());
            if (typeName == "dvec4") return ShaderDataType(ShaderPrimitiveType::dvec4());
            if (typeName == "mat2") return ShaderDataType(ShaderPrimitiveType::mat2());
            if (typeName == "mat3") return ShaderDataType(ShaderPrimitiveType::mat3());
            if (typeName == "mat4") return ShaderDataType(ShaderPrimitiveType::mat4());
            if (typeName == "dmat2") return ShaderDataType(ShaderPrimitiveType::dmat2());
            if (typeName == "dmat3") return ShaderDataType(ShaderPrimitiveType::dmat3());
            if (typeName == "dmat4") return ShaderDataType(ShaderPrimitiveType::dmat4());
            auto it = typeName.rfind("Array");
            if (it == 0) {
                auto itStart = typeName.rfind('<');
                auto itEnd = typeName.rfind('>');
                if (itStart != std::string::npos && itEnd != std::string::npos && itStart + 1 < itEnd) {
                    std::string params = typeName.substr(itStart + 1, itEnd - itStart - 1);
                    std::string type = typeName.substr(5, itStart - 5);
                    int count = std::stoi(params);
                    if (type == "Bool") return ShaderDataType(ShaderPrimitiveType::Bool(), count);
                    if (type == "Int") return ShaderDataType(ShaderPrimitiveType::Int(), count);
                    if (type == "UInt") return ShaderDataType(ShaderPrimitiveType::UInt(), count);
                    if (type == "Float") return ShaderDataType(ShaderPrimitiveType::Float(), count);
                    if (type == "Double") return ShaderDataType(ShaderPrimitiveType::Double(), count);
                    if (type == "BVec2") return ShaderDataType(ShaderPrimitiveType::bvec2(), count);
                    if (type == "BVec3") return ShaderDataType(ShaderPrimitiveType::bvec3(), count);
                    if (type == "BVec4") return ShaderDataType(ShaderPrimitiveType::bvec4(), count);
                    if (type == "IVec2") return ShaderDataType(ShaderPrimitiveType::ivec2(), count);
                    if (type == "IVec3") return ShaderDataType(ShaderPrimitiveType::ivec3(), count);
                    if (type == "IVec4") return ShaderDataType(ShaderPrimitiveType::ivec4(), count);
                    if (type == "UVec2") return ShaderDataType(ShaderPrimitiveType::uvec2(), count);
                    if (type == "UVec3") return ShaderDataType(ShaderPrimitiveType::uvec3(), count);
                    if (type == "UVec4") return ShaderDataType(ShaderPrimitiveType::uvec4(), count);
                    if (type == "Vec2") return ShaderDataType(ShaderPrimitiveType::vec2(), count);
                    if (type == "Vec3") return ShaderDataType(ShaderPrimitiveType::vec3(), count);
                    if (type == "Vec4") return ShaderDataType(ShaderPrimitiveType::vec4(), count);
                    if (type == "DVec2") return ShaderDataType(ShaderPrimitiveType::dvec2(), count);
                    if (type == "DVec3") return ShaderDataType(ShaderPrimitiveType::dvec3(), count);
                    if (type == "DVec4") return ShaderDataType(ShaderPrimitiveType::dvec4(), count);
                    if (type == "Mat2") return ShaderDataType(ShaderPrimitiveType::mat2(), count);
                    if (type == "Mat3") return ShaderDataType(ShaderPrimitiveType::mat3(), count);
                    if (type == "Mat4") return ShaderDataType(ShaderPrimitiveType::mat4(), count);
                    if (type == "DMat2") return ShaderDataType(ShaderPrimitiveType::dmat2(), count);
                    if (type == "DMat3") return ShaderDataType(ShaderPrimitiveType::dmat3(), count);
                    if (type == "DMat4") return ShaderDataType(ShaderPrimitiveType::dmat4(), count);
                }
            }
            //TODO: Struct Array support
            return ShaderDataType(typeName);
        }
    };
}

#endif //XENGINE_SHADERDATATYPE_HPP
