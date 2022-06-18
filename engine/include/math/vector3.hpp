#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>
#include <array>

#include "cast/numeric_cast.hpp"

namespace xengine {
    template<typename T>
    class XENGINE_EXPORT Vector3 {
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

        std::array<float, 3> getMemory() {
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
            v /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        Vector3<T> &operator*=(const T &v) {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        Vector3<T> &operator/=(const T &v) {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }

        template<typename R>
        explicit operator Vector3<R>() {
            return convert<R>();
        }

        template<typename R>
        Vector3<R> convert() const {
            return Vector3<R>(numeric_cast<R>(x), numeric_cast<R>(y), numeric_cast<R>(z));
        }

        friend Vector3<T> operator+(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        friend Vector3<T> operator-(const Vector3<T> &lhs, const Vector3<T> &rhs) {
            return Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }

        friend Vector3<T> operator*(const Vector3<T> &lhs, const T &rhs) {
            return Vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
        }

        friend Vector3<T> operator/(const Vector3<T> &lhs, const T &rhs) {
            return Vector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
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

        //https://en.wikipedia.org/wiki/Euclidean_distance
        XENGINE_EXPORT static double distance(const Vector3<T> &start, const Vector3<T> end) {
            return std::sqrt(
                    std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2) + std::pow(start.z - end.z, 2));
        }
    };

    typedef Vector3<bool> Vec3b;
    typedef Vector3<int> Vec3i;
    typedef Vector3<float> Vec3f;
    typedef Vector3<double> Vec3d;
}

#endif //VECTOR3_HPP