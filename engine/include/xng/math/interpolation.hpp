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

#ifndef XENGINE_INTERPOLATION_HPP
#define XENGINE_INTERPOLATION_HPP

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/quaternion.hpp"

namespace xng {
    template<typename T>
    static T lerp(const T &a, const T &b, float t) {
        return a * t + (b * (1 - t));
    }

    template<typename T>
    static Vector2<T> lerp(const Vector2<T> &a, const Vector2<T> &b, float t) {
        return a * t + (b * (1 - t));
    }

    template<typename T>
    static Vector3<T> lerp(const Vector3<T> &a, const Vector3<T> &b, float t) {
        return a * t + (b * (1 - t));
    }

    template<typename T>
    static Vector4<T> lerp(const Vector4<T> &a, const Vector4<T> &b, float t) {
        return a * t + (b * (1 - t));
    }

    XENGINE_EXPORT Quaternion slerp(const Quaternion &a, const Quaternion &b, float t);
}

#endif //XENGINE_INTERPOLATION_HPP
