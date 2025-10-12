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