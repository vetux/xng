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
#include <variant>

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

namespace xng {
    class FGShaderBuilder;

    class FGShaderValue {
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

        FGShaderValue operator+(const FGShaderValue &other);
        FGShaderValue operator-(const FGShaderValue &other);
        FGShaderValue operator*(const FGShaderValue &other);
        FGShaderValue operator/(const FGShaderValue &other);

        FGShaderValue x();

        FGShaderValue y();

        FGShaderValue z();

        FGShaderValue w();

        FGShaderValue row(size_t index);

        FGShaderValue column(size_t index);

        FGShaderValue element(size_t column, size_t row);

        FGShaderBuilder &builder;
        unsigned int handle;
        std::string identifier;

        std::variant<unsigned char,
                char,
                unsigned int,
                int,
                float,
                double,

                Vector2<unsigned char>,
                Vector2<char>,
                Vector2<unsigned int>,
                Vector2<int>,
                Vector2<float>,
                Vector2<double>,

                Vector3<unsigned char>,
                Vector3<char>,
                Vector3<unsigned int>,
                Vector3<int>,
                Vector3<float>,
                Vector3<double>,

                Matrix<unsigned char, 2, 2>,
                Matrix<char, 2, 2>,
                Matrix<unsigned int, 2, 2>,
                Matrix<int, 2, 2>,
                Matrix<float, 2, 2>,
                Matrix<double, 2, 2>,

                Matrix<unsigned char, 3, 3>,
                Matrix<char, 3, 3>,
                Matrix<unsigned int, 3, 3>,
                Matrix<int, 3, 3>,
                Matrix<float, 3, 3>,
                Matrix<double, 3, 3>,

                Matrix<unsigned char, 4, 4>,
                Matrix<char, 4, 4>,
                Matrix<unsigned int, 4, 4>,
                Matrix<int, 4, 4>,
                Matrix<float, 4, 4>,
                Matrix<double, 4, 4>> literalValue;
    };
}

#endif //XENGINE_FGVALUE_HPP
