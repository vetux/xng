/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_QUATERNION_HPP
#define XENGINE_QUATERNION_HPP

#include "matrix.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    class XENGINE_EXPORT Quaternion : public Messageable {
    public:
        float w, x, y, z;

        Quaternion();

        Quaternion(float w, float x, float y, float z);

        Quaternion(const Quaternion &other) = default;

        explicit Quaternion(Vec4f vec);

        explicit Quaternion(const Vec3f& eulerAngles);

        Quaternion &operator=(const Quaternion &other) = default;

        explicit operator Vec4f() const;

        Quaternion operator*(const Quaternion &other) const;

        void setEulerAngles(Vec3f eulerAngles);

        Vec3f getEulerAngles() const;

        /**
         * Return the matrix which applies the rotation of this quaternion.
         * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
         *
         * The returned matrix completely breaks the projection when rotating in the x axis, y axis rotation works.
         *
         * @return
         */
        Mat4f matrix() const;

        float magnitude() const;

        void normalize();

        static Quaternion normalize(const Quaternion &q);

        static Quaternion slerp(const Quaternion &a, const Quaternion &b, float advance);

        Messageable &operator<<(const Message &message) override;

        Message &operator>>(Message &message) const override;
    };
}

#endif //XENGINE_QUATERNION_HPP
