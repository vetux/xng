/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
    /**
     * TODO: NDC Independent transform coordinate space
     */
    class XENGINE_EXPORT Transform : public Messageable {
    public:
        Transform() {
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        Transform(Vec3f position, Vec3f rotation, Vec3f scale) : mPosition(position),
                                                                 mRotation(rotation),
                                                                 mScale(scale) {
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        Transform(Vec3f position, Quaternion rotation, Vec3f scale) : mPosition(position),
                                                                      mRotation(rotation),
                                                                      mScale(scale) {
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        Transform &operator+=(const Transform &other) {
            mPosition += other.mPosition;
            mRotation = other.mRotation * mRotation;
            mScale += other.mScale;
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
            return *this;
        }

        const Mat4f &model() const {
            return mModel;
        }

        void setPosition(Vec3f position) {
            if (mPosition == position) return;
            mPosition = std::move(position);
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        const Vec3f &getPosition() const {
            return mPosition;
        }

        void setRotation(const Quaternion &quaternion) {
            if (mRotation == quaternion) return;
            mRotation = quaternion;
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
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
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        void setScale(const Vec3f &scale) {
            if (mScale == scale) return;
            mScale = scale;
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        const Vec3f &getScale() const {
            return mScale;
        }

        Vec3f forward() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(0, 0, 1));
        }

        Vec3f up() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(0, 1, 0));
        }

        Vec3f left() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(-1, 0, 0));
        }

        bool operator==(const Transform &other) const {
            return mPosition == other.mPosition && mRotation == other.mRotation && mScale == other.mScale;
        }

        bool operator!=(const Transform &other) const {
            return !(*this == other);
        }

        Messageable &operator<<(const Message &message) override {
            mPosition << message.getMessage("position");
            Vec3f euler;
            euler << message.getMessage("rotation");
            mRotation = Quaternion(euler);
            mScale << message.getMessage("scale");
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
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
