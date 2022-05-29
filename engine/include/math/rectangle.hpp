#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "math/vector2.hpp"

namespace xengine {
    /**
     * A axis aligned rectangle defining position and dimensions.
     * */
    template<typename T>
    struct XENGINE_EXPORT Rectangle {
        Vector2<T> position; //Top left point of the rectangle
        Vector2<T> dimensions;

        Rectangle()
                : position(0), dimensions(0) {
        }

        Rectangle(Vector2<T> position, Vector2<T> dimensions)
                : position(position), dimensions(dimensions) {
        }

        Vector2<T> center() {
            return Vector2<T>(position.x + dimensions.x / 2, position.y - dimensions.y / 2);
        }

        Vector2<T> top() {
            return Vector2<T>(position.x + dimensions.x / 2, position.y);
        }

        Vector2<T> right() {
            return Vector2<T>(position.x + dimensions.x, position.y - dimensions.y / 2);
        }

        Vector2<T> bottom() {
            return Vector2<T>(position.x + dimensions.x / 2, position.y - dimensions.y);
        }

        Vector2<T> left() {
            return Vector2<T>(position.x, position.y - dimensions.y / 2);
        }

        template<typename R>
        explicit operator Rectangle<R>() {
            return convert<R>();
        }

        template<typename R>
        Rectangle<R> convert() const {
            return Rectangle<R>(Vector2<R>(static_cast<R>(position.x), static_cast<R>(position.y)),
                                Vector2<R>(static_cast<R>(dimensions.x), static_cast<R>(dimensions.y)));
        }

        bool testPoint(Vector2<T> point) {
            return point.x >= position.x && point.y <= position.y && point.x <= position.x + dimensions.x &&
                   point.y >= position.y - dimensions.y;
        }

        bool testOverlap(Rectangle<T> other) {
            if (position.x > other.position.x + other.dimensions.x)
                return false;
            else if (position.y < other.position.y - other.dimensions.y)
                return false;
            else if (position.x + dimensions.x < other.position.x)
                return false;
            else if (position.y - dimensions.y > other.position.y)
                return false;
            else
                return true;
        }

        friend bool operator==(const Rectangle<T> &lhs, const Rectangle<T> &rhs) {
            return lhs.position == rhs.position && lhs.dimensions == rhs.dimensions;
        }

        friend bool operator!=(const Rectangle<T> &lhs, const Rectangle<T> &rhs) {
            return !(lhs == rhs);
        }
    };

    typedef Rectangle<float> Rectf;
    typedef Rectangle<int> Recti;
}

#endif // RECTANGLE_HPP