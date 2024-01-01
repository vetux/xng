/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_TRANSFORM_HPP
#define XENGINE_TRANSFORM_HPP

#include "vector3.hpp"
#include "quaternion.hpp"
#include "matrixmath.hpp"

#include "xng/io/message.hpp"

namespace xng {
    class XENGINE_EXPORT Transform : public Messageable {
    public:
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

        Mat4f model() const {
            return MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        void setPosition(Vec3f position) {
            mPosition = position;
        }

        const Vec3f &getPosition() const {
            return mPosition;
        }

        void setRotation(const Quaternion &quaternion) {
            mRotation = quaternion;
        }

        const Quaternion &getRotation() const {
            return mRotation;
        }

        /**
         * Apply the given quaternion rotation to the existing rotation.
         *
         * @param quaternion
         * @param localAxis If true apply the rotation relative to the world origin rotation, else rotate relative to the current rotation.
         */
        void applyRotation(const Quaternion &quaternion, bool worldAxis = false) {
            if (worldAxis) {
                mRotation = mRotation * quaternion;
            } else {
                mRotation = quaternion * mRotation;
            }
        }

        void setScale(const Vec3f &scale) {
            mScale = scale;
        }

        const Vec3f &getScale() const {
            return mScale;
        }

        Vec3f forward() const {
            return rotate(Vec3f(0, 0, 1));
        }

        Vec3f up() const {
            return rotate(Vec3f(0, 1, 0));
        }

        Vec3f left() const {
            return rotate(Vec3f(-1, 0, 0));
        }

        Messageable &operator<<(const Message &message) override {
            mPosition << message.getMessage("position");
            Vec3f euler;
            euler << message.getMessage("rotation");
            mRotation = Quaternion(euler);
            mScale << message.getMessage("scale");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            mPosition >> message["position"];
            mRotation.getEulerAngles() >> message["rotation"];
            mScale >> message["scale"];
            return message;
        }

    private:
        Mat4f mModel;
        Vec3f mPosition;
        Quaternion mRotation;
        Vec3f mScale = Vec3f(1);
    };
}

#endif //XENGINE_TRANSFORM_HPP
