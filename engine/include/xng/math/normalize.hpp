/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_NORMALIZE_HPP
#define XENGINE_NORMALIZE_HPP

#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

namespace xng {
    template<typename T>
    T normalize(const T &value) {
        if (value < -1) {
            return -1;
        } else if (value > 1) {
            return 1;
        } else {
            return value;
        }
    }

    template<typename T>
    Vector2<T> normalize(const Vector2<T> &value) {
        return {normalize(value.x), normalize(value.y)};
    }

    template<typename T>
    Vector3<T> normalize(const Vector3<T> &value) {
        return {normalize(value.x), normalize(value.y), normalize(value.z)};
    }

    template<typename T>
    Vector4<T> normalize(const Vector4<T> &value) {
        return {normalize(value.x), normalize(value.y), normalize(value.z), normalize(value.w)};
    }
}
#endif //XENGINE_NORMALIZE_HPP
