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

#include "xng/render/graph2/shader/fgshadervalue.hpp"

namespace xng {
    class FGShaderBuilder;

    class FGShaderVariable {
    public:
        enum Type {
            LITERAL = 0,
            SAMPLER,
            ARRAY,
            SINGLE,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MAT2,
            MAT3,
            MAT4,
        } type;

        enum Component {
            UNSIGNED_BYTE = 0,
            SIGNED_BYTE,
            UNSIGNED_INT,
            SIGNED_INT,
            FLOAT,
            DOUBLE
        } component;

        FGShaderVariable operator+(const FGShaderVariable &other);
        FGShaderVariable operator-(const FGShaderVariable &other);
        FGShaderVariable operator*(const FGShaderVariable &other);
        FGShaderVariable operator/(const FGShaderVariable &other);

        FGShaderVariable operator[](const FGShaderVariable &index);

        void assign(const FGShaderVariable &var);

        FGShaderVariable x();

        FGShaderVariable y();

        FGShaderVariable z();

        FGShaderVariable w();

        FGShaderVariable row(size_t index);

        FGShaderVariable column(size_t index);

        FGShaderVariable element(size_t column, size_t row);

        FGShaderBuilder &builder;
        unsigned int handle;
        std::string identifier;

        size_t arraySize;

        FGShaderValue literalValue;
    };
}

#endif //XENGINE_FGVALUE_HPP
