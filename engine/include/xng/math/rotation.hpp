/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <cmath>

#include "xng/math/vector2.hpp"
#include "xng/math/pi.hpp"

namespace xng {
    inline double radiansToDegrees(const double radians) {
        return radians * (180.0 / PI);
    }

    inline double degreesToRadians(const double degrees) {
        return degrees * PI / 180.0;
    }

    inline float radiansToDegrees(const float radians) {
        return static_cast<float>(radians * (180.0 / PI));
    }

    inline float degreesToRadians(const float degrees) {
        return static_cast<float>(degrees * PI / 180.0);
    }

    //Rotate the vector around the point clockwise by angleDegrees
    inline Vec2f rotateVectorAroundPoint(const Vec2f &vector, const Vec2f &point, const double angleDegrees) {
        const float radians = static_cast<float>(degreesToRadians(angleDegrees));

        //https://math.stackexchange.com/questions/814950/how-can-i-rotate-a-coordinate-around-a-circle
        Vec2f ret;
        ret.x = std::cos(radians) * (vector.x - point.x) - std::sin(radians) * (vector.y - point.y) + point.x;
        ret.y = std::sin(radians) * (vector.x - point.x) + std::cos(radians) * (vector.y - point.y) + point.y;
        return ret;
    }

    inline double getAngle(const Vec2f &vec) {
        return radiansToDegrees(atan2(vec.y, vec.x));
    }
}

#endif // ROTATION_HPP