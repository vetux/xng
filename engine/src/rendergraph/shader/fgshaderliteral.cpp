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

#include "xng/rendergraph/shader/fgshaderliteral.hpp"

#include "xng/rendergraph/shader/fgshadervalue.hpp"

namespace xng {
    FGShaderValue getLiteralType(const FGShaderLiteral &literal) {
        switch (literal.index()) {
            case 0:
                return {FGShaderValue::SCALAR, FGShaderValue::BOOLEAN, 1};
            case 1:
                return {FGShaderValue::SCALAR, FGShaderValue::UNSIGNED_INT, 1};
            case 2:
                return {FGShaderValue::SCALAR, FGShaderValue::SIGNED_INT, 1};
            case 3:
                return {FGShaderValue::SCALAR, FGShaderValue::FLOAT, 1};
            case 4:
                return {FGShaderValue::SCALAR, FGShaderValue::DOUBLE, 1};
            case 5:
                return {FGShaderValue::VECTOR2, FGShaderValue::BOOLEAN, 1};
            case 6:
                return {FGShaderValue::VECTOR2, FGShaderValue::UNSIGNED_INT, 1};
            case 7:
                return {FGShaderValue::VECTOR2, FGShaderValue::SIGNED_INT, 1};
            case 8:
                return {FGShaderValue::VECTOR2, FGShaderValue::FLOAT, 1};
            case 9:
                return {FGShaderValue::VECTOR2, FGShaderValue::DOUBLE, 1};
            case 10:
                return {FGShaderValue::VECTOR3, FGShaderValue::BOOLEAN, 1};
            case 11:
                return {FGShaderValue::VECTOR3, FGShaderValue::UNSIGNED_INT, 1};
            case 12:
                return {FGShaderValue::VECTOR3, FGShaderValue::SIGNED_INT, 1};
            case 13:
                return {FGShaderValue::VECTOR3, FGShaderValue::FLOAT, 1};
            case 14:
                return {FGShaderValue::VECTOR3, FGShaderValue::DOUBLE, 1};
            case 15:
                return {FGShaderValue::VECTOR4, FGShaderValue::BOOLEAN, 1};
            case 16:
                return {FGShaderValue::VECTOR4, FGShaderValue::UNSIGNED_INT, 1};
            case 17:
                return {FGShaderValue::VECTOR4, FGShaderValue::SIGNED_INT, 1};
            case 18:
                return {FGShaderValue::VECTOR4, FGShaderValue::FLOAT, 1};
            case 19:
                return {FGShaderValue::VECTOR4, FGShaderValue::DOUBLE, 1};
            case 20:
                return {FGShaderValue::MAT2, FGShaderValue::FLOAT, 1};
            case 21:
                return {FGShaderValue::MAT2, FGShaderValue::DOUBLE, 1};
            case 22:
                return {FGShaderValue::MAT3, FGShaderValue::FLOAT, 1};
            case 23:
                return {FGShaderValue::MAT3, FGShaderValue::DOUBLE, 1};
            case 24:
                return {FGShaderValue::MAT4, FGShaderValue::FLOAT, 1};
            case 25:
                return {FGShaderValue::MAT4, FGShaderValue::DOUBLE, 1};
            default:
                throw std::runtime_error("Invalid literal type");
        }
    }
}
