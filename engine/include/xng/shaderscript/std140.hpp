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

#ifndef XENGINE_STD140_HPP
#define XENGINE_STD140_HPP

#include <array>
#include <type_traits>

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

namespace xng {
    /**
     * Stores a CPU type in its exact std140 memory layout for direct upload to GPU buffers.
     *
     * All specializations use raw scalar storage — no virtual types are stored directly,
     * avoiding vtable pointer contamination from Vector/Matrix types which inherit Messageable.
     *
     * Interface: assign from T via operator=, convert back to T via implicit conversion.
     */

    // ── scalar (primary) ─────────────────────────────────────────────────────
    // Covers: bool, int, uint, float, double
    template<typename T>
    struct alignas(sizeof(T)) Std140 {
        static_assert(std::is_arithmetic_v<T>,
                      "Std140<T>: no specialization for T. "
                      "Supported types: bool, int, uint, float, double, "
                      "Vector2/3/4<T>, Matrix<T,2,2>, Matrix<T,3,3>, Matrix<T,4,4>, std::array<T,N>.");
        T value{};

        Std140() = default;

        Std140(const T &v) : value(v) {
        }

        Std140 &operator=(const T &v) {
            value = v;
            return *this;
        }

        operator T() const { return value; }
        operator T &() { return value; }
    };

    // ── vec2 ─────────────────────────────────────────────────────────────────
    // std140: base alignment 2N, size 2N
    template<typename T>
    struct alignas(2 * sizeof(T)) Std140<Vector2<T> > {
        T x{}, y{};

        Std140() = default;

        Std140(const Vector2<T> &v) : x(v.x), y(v.y) {
        }

        Std140 &operator=(const Vector2<T> &v) {
            x = v.x;
            y = v.y;
            return *this;
        }

        operator Vector2<T>() const { return {x, y}; }
    };

    // ── vec3 ─────────────────────────────────────────────────────────────────
    // std140: base alignment 4N, size 4N
    template<typename T>
    struct alignas(4 * sizeof(T)) Std140<Vector3<T> > {
        T x{}, y{}, z{};

    private:
        T _pad{};

    public:
        Std140() = default;

        Std140(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {
        }

        Std140 &operator=(const Vector3<T> &v) {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        operator Vector3<T>() const { return {x, y, z}; }
    };

    // ── vec4 ─────────────────────────────────────────────────────────────────
    // std140: base alignment 4N, size 4N
    template<typename T>
    struct alignas(4 * sizeof(T)) Std140<Vector4<T> > {
        T x{}, y{}, z{}, w{};

        Std140() = default;

        Std140(const Vector4<T> &v) : x(v.x), y(v.y), z(v.z), w(v.w) {
        }

        Std140 &operator=(const Vector4<T> &v) {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        operator Vector4<T>() const { return {x, y, z, w}; }
    };

    // ── mat2 ─────────────────────────────────────────────────────────────────
    // std140: column-major, each column is a vec2 padded to vec4
    // total: 2 * 4N = 8N
    template<typename T>
    struct alignas(4 * sizeof(T)) Std140<Matrix<T, 2, 2> > {
    private:
        T _data[8]{};

    public:
        Std140() = default;

        Std140(const Matrix<T, 2, 2> &m) { *this = m; }

        Std140 &operator=(const Matrix<T, 2, 2> &m) {
            _data[0] = m.get(0, 0);
            _data[1] = m.get(0, 1);
            _data[4] = m.get(1, 0);
            _data[5] = m.get(1, 1);
            return *this;
        }

        operator Matrix<T, 2, 2>() const {
            Matrix<T, 2, 2> m;
            m.set(0, 0, _data[0]);
            m.set(0, 1, _data[1]);
            m.set(1, 0, _data[4]);
            m.set(1, 1, _data[5]);
            return m;
        }
    };

    // ── mat3 ─────────────────────────────────────────────────────────────────
    // std140: column-major, each column is a vec3 padded to vec4
    // total: 3 * 4N = 12N
    template<typename T>
    struct alignas(4 * sizeof(T)) Std140<Matrix<T, 3, 3> > {
    private:
        T _data[12]{};

    public:
        Std140() = default;

        Std140(const Matrix<T, 3, 3> &m) { *this = m; }

        Std140 &operator=(const Matrix<T, 3, 3> &m) {
            _data[0] = m.get(0, 0);
            _data[1] = m.get(0, 1);
            _data[2] = m.get(0, 2);
            _data[4] = m.get(1, 0);
            _data[5] = m.get(1, 1);
            _data[6] = m.get(1, 2);
            _data[8] = m.get(2, 0);
            _data[9] = m.get(2, 1);
            _data[10] = m.get(2, 2);
            return *this;
        }

        operator Matrix<T, 3, 3>() const {
            Matrix<T, 3, 3> m;
            m.set(0, 0, _data[0]);
            m.set(0, 1, _data[1]);
            m.set(0, 2, _data[2]);
            m.set(1, 0, _data[4]);
            m.set(1, 1, _data[5]);
            m.set(1, 2, _data[6]);
            m.set(2, 0, _data[8]);
            m.set(2, 1, _data[9]);
            m.set(2, 2, _data[10]);
            return m;
        }
    };

    // ── mat4 ─────────────────────────────────────────────────────────────────
    // std140: column-major, each column is a vec4 — no padding needed
    // total: 4 * 4N = 16N
    template<typename T>
    struct alignas(4 * sizeof(T)) Std140<Matrix<T, 4, 4> > {
    private:
        T _data[16]{};

    public:
        Std140() = default;

        Std140(const Matrix<T, 4, 4> &m) { *this = m; }

        Std140 &operator=(const Matrix<T, 4, 4> &m) {
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    _data[c * 4 + r] = m.get(c, r);
            return *this;
        }

        operator Matrix<T, 4, 4>() const {
            Matrix<T, 4, 4> m;
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    m.set(c, r, _data[c * 4 + r]);
            return m;
        }
    };

    // ── std::array<T, N> ─────────────────────────────────────────────────────
    // std140: array elements are strided at a multiple of 16 bytes (vec4 size),
    // regardless of the element's own natural size/alignment — even a plain
    // float[] or vec2[] must be padded out to 16 bytes per element.
    //
    // Std140ArrayElement dispatches on whether padding is needed at all, so the
    // no-padding case (element already a multiple of 16, e.g. vec3/vec4/mat4)
    // never has to declare a zero-length `std::byte _pad[0]` array, which isn't
    // portable C++.
    template<typename T, bool NeedsPad>
    struct Std140ArrayElement {
        // sizeof(Std140<T>) % 16 == 0: no padding required.
        Std140<T> value{};
    };

    template<typename T>
    struct Std140ArrayElement<T, true> {
        // sizeof(Std140<T>) % 16 != 0: pad up to the next multiple of 16.
        // The subtrahend is in (0, 16), so this is never zero-length.
        Std140<T> value{};
        std::byte _pad[16 - (sizeof(Std140<T>) % 16)]{};
    };

    template<typename T, size_t N>
    struct alignas(16) Std140<std::array<T, N> > {
    private:
        using Element = Std140ArrayElement<T, (sizeof(Std140<T>) % 16 != 0)>;
        static_assert(sizeof(Element) % 16 == 0,
                      "Std140 array element stride must be a multiple of 16 bytes (std140 rule).");

        Element elements[N]{};

    public:
        Std140() = default;

        Std140(const std::array<T, N> &a) { for (size_t i = 0; i < N; ++i) elements[i].value = a[i]; }

        Std140 &operator=(const std::array<T, N> &a) {
            for (size_t i = 0; i < N; ++i) elements[i].value = a[i];
            return *this;
        }

        operator std::array<T, N>() const {
            std::array<T, N> a;
            for (size_t i = 0; i < N; ++i) a[i] = elements[i].value;
            return a;
        }
    };

    template<>
    struct alignas(4) Std140<bool> {
        int32_t value{};

        Std140() = default;

        Std140(bool v) : value(v ? 1 : 0) {
        }

        Std140 &operator=(bool v) {
            value = v ? 1 : 0;
            return *this;
        }

        operator bool() const { return value != 0; }
    };
} // namespace xng

#endif // XENGINE_STD140_HPP
