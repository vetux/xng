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

#ifndef XENGINE_FGVALUE_HPP
#define XENGINE_FGVALUE_HPP

#include <cstddef>
#include <utility>

namespace xng {
    // An assignable shader variable
    class FGShaderVariable {
    public:
        enum Type {
            SINGLE = 0,
            ARRAY,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MAT2,
            MAT3,
            MAT4,
        } type = SINGLE;

        enum Component {
            UNSIGNED_BYTE = 0,
            SIGNED_BYTE,
            UNSIGNED_INT,
            SIGNED_INT,
            FLOAT,
            DOUBLE
        } component = UNSIGNED_BYTE;

        size_t id; // Unique id for this variable
        std::string name; // Optional user specified name for this variable

        size_t arraySize{}; // If this variable is of type ARRAY the number of elements in this array
    };
}

#endif //XENGINE_FGVALUE_HPP
