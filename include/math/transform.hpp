/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TRANSFORM_HPP
#define XENGINE_TRANSFORM_HPP

#include "math/vector3.hpp"
#include "quaternion.hpp"
#include "math/matrixmath.hpp"

namespace xengine {
    struct XENGINE_EXPORT Transform {
        Transform() = default;

        Transform(Vec3f position, Vec3f rotation, Vec3f scale) : mPosition(position),
                                                                 mRotation(rotation),
                                                                 mScale(scale) {}

        Transform(Vec3f position, Quaternion rotation, Vec3f scale) : mPosition(position),
                                                                      mRotation(rotation),
                                                                      mScale(scale) {}

        Transform &operator+=(const Transform &other) {
            mPosition += other.mPosition;
            mRotation = other.mRotation * mRotation;
            mScale += other.mScale;
            return *this;
        }

        Vec3f rotate(Vec3f vec) const {
            Vec4f ret = MatrixMath::inverse(mRotation.matrix()) * Vec4f(vec.x, vec.y, vec.z, 1);
            return Vec3f(ret.x, ret.y, ret.z);
        }

        Mat4f model() {
            if (dirty) {
                dirty = false;
                mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
            }
            return mModel;
        }

        void setPosition(Vec3f position) {
            dirty = true;
            mPosition = position;
        }

        const Vec3f &getPosition() const {
            return mPosition;
        }

        void setRotation(const Quaternion &quaternion) {
            dirty = true;
            mRotation = quaternion;
        }

        const Quaternion &getRotation() const {
            return mRotation;
        }

        void setScale(const Vec3f &scale) {
            dirty = true;
            mScale = scale;
        }

        const Vec3f &getScale() const {
            return mScale;
        }

    private:
        Mat4f mModel;
        Vec3f mPosition;
        Quaternion mRotation;
        Vec3f mScale = Vec3f(1);
        bool dirty = true;
    };
}

#endif //XENGINE_TRANSFORM_HPP
