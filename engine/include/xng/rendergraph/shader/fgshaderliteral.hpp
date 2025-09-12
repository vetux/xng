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

#ifndef XENGINE_FGSHADERVALUE_HPP
#define XENGINE_FGSHADERVALUE_HPP

#include <variant>

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

namespace xng {
    // A user specified literal value for use in a shader
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
        Matrix<double, 4, 4> > FGShaderLiteral;

    struct FGShaderValue;

    XENGINE_EXPORT FGShaderValue getLiteralType(const FGShaderLiteral &literal);
}

#endif //XENGINE_FGSHADERVALUE_HPP
