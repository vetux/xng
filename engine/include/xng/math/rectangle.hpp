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

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "vector2.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * A axis aligned rectangle defining position and dimensions.
     * */
    template<typename T>
    struct XENGINE_EXPORT Rectangle : public Messageable {
        Vector2<T> position; //Top left point of the rectangle
        Vector2<T> dimensions;

        Rectangle()
                : position(0), dimensions(0) {
        }

        Rectangle(Vector2<T> position, Vector2<T> dimensions)
                : position(position), dimensions(dimensions) {
        }

        Vector2<T> center() const {
            return Vector2<T>(position.x + dimensions.x / 2, position.y - dimensions.y / 2);
        }

        Vector2<T> top() const {
            return Vector2<T>(position.x + dimensions.x / 2, position.y);
        }

        Vector2<T> right() const {
            return Vector2<T>(position.x + dimensions.x, position.y - dimensions.y / 2);
        }

        Vector2<T> bottom() const {
            return Vector2<T>(position.x + dimensions.x / 2, position.y - dimensions.y);
        }

        Vector2<T> left() const {
            return Vector2<T>(position.x, position.y - dimensions.y / 2);
        }

        template<typename R>
        explicit operator Rectangle<R>() const {
            return convert<R>();
        }

        template<typename R>
        Rectangle<R> convert() const {
            return Rectangle<R>(Vector2<R>(static_cast<R>(position.x), static_cast<R>(position.y)),
                                Vector2<R>(static_cast<R>(dimensions.x), static_cast<R>(dimensions.y)));
        }

        bool testPoint(Vector2<T> point) const {
            return point.x >= position.x
            && point.y <= position.y
            && point.x <= position.x + dimensions.x
            && point.y >= position.y - dimensions.y;
        }

        bool testOverlap(Rectangle<T> other) const {
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

        Messageable &operator<<(const Message &message) override {
            position << message.getMessage("position");
            dimensions << message.getMessage("dimensions");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            position >> message["position"];
            dimensions >> message["dimensions"];
            return message;
        }
    };

    typedef Rectangle<float> Rectf;
    typedef Rectangle<int> Recti;
}

#endif // RECTANGLE_HPP