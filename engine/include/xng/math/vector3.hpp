#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>
#include <array>

#include "xng/util/hashcombine.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    template<typename T>
    class XENGINE_EXPORT Vector3 : public Messageable {
    public:
        T x;
        T y;
        T z;

        Vector3()
                : x(0), y(0), z(0) {
        }

        explicit Vector3(T v)
                : x(v), y(v), z(v) {
        }

        Vector3(T x, T y, T z)
                : x(x), y(y), z(z) {
        }

        std::array<float, 3> getMemory() const {
            static_assert(sizeof(std::array<T, 3>) == sizeof(T[3]));
            return std::array<float, 3>{x, y, z};
        }

        Vector3<T> &operator+=(const Vector3<T> &v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3<T> &operator-=(const Vector3<T> &v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Vector3<T> &operator*=(const Vector3<T> &v) {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }

        Vector3<T> &operator/=(const Vector3<T> &v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        template<typename R>
        Vector3<T> &operator*=(const R &v) {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        template<typename R>
        Vector3<T> &operator/=(const R &v) {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }

        template<typename R>
        explicit operator Vector3<R>() {
            return convert<R>();
        }

        Messageable &operator<<(const Message &message) override {
            message.valueOf({"x", "r"}, x);
            message.valueOf({"y", "g"}, y);
            message.valueOf({"z", "b"}, z);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["x"] = x;
            message["y"] = y;
            message["z"] = z;
            return message;
        }

        template<typename R>
        Vector3<R> convert() const {
            return Vector3<R>(static_cast<R>(x), static_cast<R>(y), static_cast<R>(z));
        }

        friend Vector3<T> operator+(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        friend Vector3<T> operator-(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }

        friend Vector3<T> operator*(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }

        friend Vector3<T> operator/(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
        }

        friend bool operator==(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }

        friend bool operator!=(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return !(lhs == rhs);
        }

        template<typename R>
        friend Vector3<T> operator*(const Vector3<T> &lhs, const R &rhs) {
            return Vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
        }

        template<typename R>
        friend Vector3<T> operator/(const Vector3<T> &lhs, const R &rhs) {
            return Vector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
        }

        T magnitude() const {
            return magnitude(*this);
        }

        T distance(const Vector3<T> &other) const {
            return distance(*this, other);
        }

        static T magnitude(const Vector3<T> &vec) {
            return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2) + std::pow(vec.z, 2));
        }

        //https://en.wikipedia.org/wiki/Euclidean_distance
        static T distance(const Vector3<T> &start, const Vector3<T> end) {
            return std::sqrt(std::pow(start.x - end.x, 2)
                             + std::pow(start.y - end.y, 2)
                             + std::pow(start.z - end.z, 2));
        }
    };

    typedef Vector3<bool> Vec3b;
    typedef Vector3<int> Vec3i;
    typedef Vector3<unsigned int> Vec3u;
    typedef Vector3<long> Vec3l;
    typedef Vector3<unsigned long> Vec3ul;
    typedef Vector3<float> Vec3f;
    typedef Vector3<double> Vec3d;
}

namespace std {
    template<typename T>
    struct hash<xng::Vector3<T>> {
        std::size_t operator()(const xng::Vector3<T> &vec) const {
            size_t ret = 0;
            xng::hash_combine(ret, vec.x);
            xng::hash_combine(ret, vec.y);
            xng::hash_combine(ret, vec.z);
            return ret;
        }
    };
}


#endif //VECTOR3_HPP