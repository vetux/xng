#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include <cmath>
#include <array>

#include "util/numeric_cast.hpp"
#include "io/messageable.hpp"

namespace xng {
    template<typename T>
    class XENGINE_EXPORT Vector4 : public Messageable {
    public:
        T x;
        T y;
        T z;
        T w;

        Vector4()
                : x(0), y(0), z(0), w(0) {
        }

        explicit Vector4(T v)
                : x(v), y(v), z(v), w(v) {
        }

        Vector4(T x, T y, T z, T w)
                : x(x), y(y), z(z), w(w) {
        }

        std::array<float, 4> getMemory() {
            static_assert(sizeof(std::array<T, 4>) == sizeof(T[4]));
            return std::array<float, 4>{x, y, z, w};
        }

        Vector4<T> &operator+=(const Vector4<T> &v) {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        Vector4<T> &operator-=(const Vector4<T> &v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        Vector4<T> &operator*=(const Vector4<T> &v) {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }

        Vector4<T> &operator/=(const Vector4<T> &v) {
            v /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }

        Vector4<T> &operator*=(const T &v) {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }

        Vector4<T> &operator/=(const T &v) {
            x /= v;
            y /= v;
            z /= v;
            w /= v;
            return *this;
        }

        template<typename R>
        explicit operator Vector4<R>() {
            return convert<R>();
        }

        Messageable &operator<<(const Message &message) override {
            x = message.valueOf({"x", "r"}, 0.0f);
            y = message.valueOf({"y", "g"}, 0.0f);
            z = message.valueOf({"z", "b"}, 0.0f);
            w = message.valueOf({"w", "a"}, 0.0f);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["x"] = x;
            message["y"] = y;
            message["z"] = z;
            message["w"] = w;
            return message;
        }

        template<typename R>
        Vector4<R> convert() const {
            return Vector4<R>(numeric_cast<R>(x), numeric_cast<R>(y), numeric_cast<R>(z), numeric_cast<R>(w));
        }

        friend Vector4<T> operator+(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return Vector4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        }

        friend Vector4<T> operator-(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return Vector4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        }

        friend Vector4<T> operator*(const Vector4<T> &lhs, const T &rhs) {
            return Vector4<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
        }

        friend Vector4<T> operator/(const Vector4<T> &lhs, const T &rhs) {
            return Vector4<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
        }

        friend Vector4<T> operator*(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return Vector4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        friend Vector4<T> operator/(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return Vector4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
        }

        friend bool operator==(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }

        friend bool operator!=(const Vector4<T> &lhs, const Vector4<T> &rhs) {
            return !(lhs == rhs);
        }

        //https://en.wikipedia.org/wiki/Euclidean_distance
        XENGINE_EXPORT static double distance(const Vector4<T> &start, const Vector4<T> end) {
            return std::sqrt(
                    std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2) + std::pow(start.z - end.z, 2) +
                    std::pow(start.w - end.w, 2));
        }
    };

    typedef Vector4<bool> Vec4b;
    typedef Vector4<int> Vec4i;
    typedef Vector4<float> Vec4f;
    typedef Vector4<double> Vec4d;
}

#endif //VECTOR4_HPP