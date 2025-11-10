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

#include "xng/rendergraph/shader/shaderprimitive.hpp"

namespace xng {
    ShaderPrimitiveType ShaderPrimitive::getType() const {
         switch (value.index()) {
            case 0:
                return {ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::BOOLEAN};
            case 1:
                return {ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::UNSIGNED_INT};
            case 2:
                return {ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::SIGNED_INT};
            case 3:
                return {ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::FLOAT};
            case 4:
                return {ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::DOUBLE};
            case 5:
                return {ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::BOOLEAN};
            case 6:
                return {ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::UNSIGNED_INT};
            case 7:
                return {ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::SIGNED_INT};
            case 8:
                return {ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::FLOAT};
            case 9:
                return {ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::DOUBLE};
            case 10:
                return {ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::BOOLEAN};
            case 11:
                return {ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::UNSIGNED_INT};
            case 12:
                return {ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::SIGNED_INT};
            case 13:
                return {ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::FLOAT};
            case 14:
                return {ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::DOUBLE};
            case 15:
                return {ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::BOOLEAN};
            case 16:
                return {ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::UNSIGNED_INT};
            case 17:
                return {ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::SIGNED_INT};
            case 18:
                return {ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::FLOAT};
            case 19:
                return {ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::DOUBLE};
            case 20:
                return {ShaderPrimitiveType::MAT2, ShaderPrimitiveType::FLOAT};
            case 21:
                return {ShaderPrimitiveType::MAT2, ShaderPrimitiveType::DOUBLE};
            case 22:
                return {ShaderPrimitiveType::MAT3, ShaderPrimitiveType::FLOAT};
            case 23:
                return {ShaderPrimitiveType::MAT3, ShaderPrimitiveType::DOUBLE};
            case 24:
                return {ShaderPrimitiveType::MAT4, ShaderPrimitiveType::FLOAT};
            case 25:
                return {ShaderPrimitiveType::MAT4, ShaderPrimitiveType::DOUBLE};
            default:
                throw std::runtime_error("Invalid literal type");
        }
    }
}
