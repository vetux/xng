#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>
#include <array>

#include "util/numeric_cast.hpp"

#include "io/messageable.hpp"

namespace xng {
    template<typename T>
    class XENGINE_EXPORT Vector2 : public Messageable {
    public:
        T x;
        T y;

        Vector2()
                : x(0), y(0) {
        }

        explicit Vector2(T v)
                : x(v), y(v) {
        }

        Vector2(T x, T y)
                : x(x), y(y) {
        }

        std::array<float, 2> getMemory() {
            static_assert(sizeof(std::array<T, 2>) == sizeof(T[2]));
            return std::array<float, 2>({x, y});
        }

        Vector2<T> &operator+=(const Vector2<T> &v) {
            x += v.x;
            y += v.y;
            return *this;
        }

        Vector2<T> &operator-=(const Vector2<T> &v) {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        Vector2<T> &operator*=(const Vector2<T> &v) {
            x *= v.x;
            y *= v.y;
            return *this;
        }

        Vector2<T> &operator/=(const Vector2<T> &v) {
            x /= v.x;
            y /= v.y;
            return *this;
        }

        Vector2<T> &operator*=(const T &v) {
            x *= v;
            y *= v;
            return *this;
        }

        Vector2<T> &operator/=(const T &v) {
            x /= v;
            y /= v;
            return *this;
        }

        template <typename R>
         explicit operator Vector2<R>(){
             return convert<R>();
         }

        Messageable &operator<<(const Message &message) override {
            x = message.valueOf({"x", "r"}, 0.0f);
            y = message.valueOf({"y", "g"}, 0.0f);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["x"] = x;
            message["y"] = y;
            return message;
        }

        template<typename R>
        Vector2<R> convert() const {
            return Vector2<R>(numeric_cast<R>(x), numeric_cast<R>(y));
        }

        friend Vector2<T> operator+(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
        }

        friend Vector2<T> operator-(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
        }

        friend Vector2<T> operator*(const Vector2<T> &lhs, const T &rhs) {
            return Vector2<T>(lhs.x * rhs, lhs.y * rhs);
        }

        friend Vector2<T> operator/(const Vector2<T> &lhs, const T &rhs) {
            return Vector2<T>(lhs.x / rhs, lhs.y / rhs);
        }

        friend Vector2<T> operator*(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return Vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
        }

        friend Vector2<T> operator/(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return Vector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
        }

        friend bool operator==(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }

        friend bool operator!=(const Vector2<T> &lhs, const Vector2<T> &rhs) {
            return !(lhs == rhs);
        }

        //https://en.wikipedia.org/wiki/Euclidean_distance
        XENGINE_EXPORT static double distance(const Vector2<T> &start, const Vector2<T> end) {
            return std::sqrt(std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2));
        }
    };

    typedef Vector2<bool> Vec2b;
    typedef Vector2<int> Vec2i;
    typedef Vector2<float> Vec2f;
    typedef Vector2<double> Vec2d;
}

#endif //VECTOR2_HPP