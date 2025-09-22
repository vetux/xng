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

#include "xng/rendergraph/shader/shaderliteral.hpp"

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    ShaderDataType getLiteralType(const ShaderLiteral &literal) {
        switch (literal.index()) {
            case 0:
                return {ShaderDataType::SCALAR, ShaderDataType::BOOLEAN, 1};
            case 1:
                return {ShaderDataType::SCALAR, ShaderDataType::UNSIGNED_INT, 1};
            case 2:
                return {ShaderDataType::SCALAR, ShaderDataType::SIGNED_INT, 1};
            case 3:
                return {ShaderDataType::SCALAR, ShaderDataType::FLOAT, 1};
            case 4:
                return {ShaderDataType::SCALAR, ShaderDataType::DOUBLE, 1};
            case 5:
                return {ShaderDataType::VECTOR2, ShaderDataType::BOOLEAN, 1};
            case 6:
                return {ShaderDataType::VECTOR2, ShaderDataType::UNSIGNED_INT, 1};
            case 7:
                return {ShaderDataType::VECTOR2, ShaderDataType::SIGNED_INT, 1};
            case 8:
                return {ShaderDataType::VECTOR2, ShaderDataType::FLOAT, 1};
            case 9:
                return {ShaderDataType::VECTOR2, ShaderDataType::DOUBLE, 1};
            case 10:
                return {ShaderDataType::VECTOR3, ShaderDataType::BOOLEAN, 1};
            case 11:
                return {ShaderDataType::VECTOR3, ShaderDataType::UNSIGNED_INT, 1};
            case 12:
                return {ShaderDataType::VECTOR3, ShaderDataType::SIGNED_INT, 1};
            case 13:
                return {ShaderDataType::VECTOR3, ShaderDataType::FLOAT, 1};
            case 14:
                return {ShaderDataType::VECTOR3, ShaderDataType::DOUBLE, 1};
            case 15:
                return {ShaderDataType::VECTOR4, ShaderDataType::BOOLEAN, 1};
            case 16:
                return {ShaderDataType::VECTOR4, ShaderDataType::UNSIGNED_INT, 1};
            case 17:
                return {ShaderDataType::VECTOR4, ShaderDataType::SIGNED_INT, 1};
            case 18:
                return {ShaderDataType::VECTOR4, ShaderDataType::FLOAT, 1};
            case 19:
                return {ShaderDataType::VECTOR4, ShaderDataType::DOUBLE, 1};
            case 20:
                return {ShaderDataType::MAT2, ShaderDataType::FLOAT, 1};
            case 21:
                return {ShaderDataType::MAT2, ShaderDataType::DOUBLE, 1};
            case 22:
                return {ShaderDataType::MAT3, ShaderDataType::FLOAT, 1};
            case 23:
                return {ShaderDataType::MAT3, ShaderDataType::DOUBLE, 1};
            case 24:
                return {ShaderDataType::MAT4, ShaderDataType::FLOAT, 1};
            case 25:
                return {ShaderDataType::MAT4, ShaderDataType::DOUBLE, 1};
            default:
                throw std::runtime_error("Invalid literal type");
        }
    }
}
