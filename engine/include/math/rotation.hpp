#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <cmath>

#include "math/vector2.hpp"

#define PI 3.141592653589793238463

namespace xng {
    inline double radiansToDegrees(double radians) {
        return radians * (180.0 / PI);
    }

    inline double degreesToRadians(double degrees) {
        return degrees * PI / 180.0;
    }

    inline float radiansToDegrees(float radians) {
        return (float) (radians * (180.0 / PI));
    }

    inline float degreesToRadians(float degrees) {
        return (float) (degrees * PI / 180.0);
    }

    //Rotate the vector around the point clockwise by angleDegrees
    inline Vec2f rotateVectorAroundPoint(Vec2f vector, Vec2f point, double angleDegrees) {
        double radians = degreesToRadians(angleDegrees);

        //https://math.stackexchange.com/questions/814950/how-can-i-rotate-a-coordinate-around-a-circle
        Vec2f ret;
        ret.x = std::cos(radians) * (vector.x - point.x) - std::sin(radians) * (vector.y - point.y) + point.x;
        ret.y = std::sin(radians) * (vector.x - point.x) + std::cos(radians) * (vector.y - point.y) + point.y;
        return ret;
    }

    inline double getAngle(const Vec2f &vec) {
        return radiansToDegrees(atan2(vec.y, vec.y));
    }
}

#endif // ROTATION_HPP