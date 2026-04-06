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

#include "xng/math/vector3.hpp"
#include "xng/math/quaternion.hpp"
#include "xng/math/matrixmath.hpp"

#include "xng/io/message.hpp"

namespace xng {
    class Transform final : public Messageable {
    public:
        Transform() {
            updateModelMatrix();
        }

        Transform(Vec3f position, const Vec3f &rotation, Vec3f scale)
            : mPosition(std::move(position)),
              mRotation(rotation),
              mScale(std::move(scale)) {
            updateModelMatrix();
        }

        Transform(Vec3f position, Quaternion rotation, Vec3f scale)
            : mPosition(std::move(position)),
              mRotation(std::move(rotation)),
              mScale(std::move(scale)) {
            updateModelMatrix();
        }

        void setPosition(Vec3f position) {
            if (mPosition == position) return;
            mPosition = std::move(position);
            updateModelMatrix();
        }

        [[nodiscard]] const Vec3f &getPosition() const {
            return mPosition;
        }

        void setRotation(const Quaternion &quaternion) {
            if (mRotation == quaternion) return;
            mRotation = quaternion;
            updateModelMatrix();
        }

        [[nodiscard]] const Quaternion &getRotation() const {
            return mRotation;
        }

        void setScale(const Vec3f &scale) {
            if (mScale == scale) return;
            mScale = scale;
            updateModelMatrix();
        }

        [[nodiscard]] const Vec3f &getScale() const {
            return mScale;
        }

        [[nodiscard]] const Mat4f &model() const {
            return mModel;
        }

        [[nodiscard]] Vec3f forward() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(0, 0, 1));
        }

        [[nodiscard]] Vec3f up() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(0, 1, 0));
        }

        [[nodiscard]] Vec3f left() const {
            return MatrixMath::inverse(mRotation.matrix()) * (Vec3f(-1, 0, 0));
        }

        /**
         * Apply the given quaternion rotation to the existing rotation.
         *
         * @param quaternion
         * @param worldAxis If true, apply the rotation relative to the world origin rotation, else rotate relative to the current rotation.
         */
        void applyRotation(const Quaternion &quaternion, const bool worldAxis = false) {
            if (worldAxis) {
                mRotation = mRotation * quaternion;
            } else {
                mRotation = quaternion * mRotation;
            }
            updateModelMatrix();
        }

        bool operator==(const Transform &other) const {
            return mPosition == other.mPosition && mRotation == other.mRotation && mScale == other.mScale;
        }

        bool operator!=(const Transform &other) const {
            return !(*this == other);
        }

        Transform &operator+=(const Transform &other) {
            mPosition += other.mPosition;
            mRotation = other.mRotation * mRotation;
            mScale += other.mScale;
            updateModelMatrix();
            return *this;
        }

        Messageable &operator<<(const Message &message) override {
            mPosition << message.getMessage("position");
            Vec3f euler;
            euler << message.getMessage("rotation");
            mRotation = Quaternion(euler);
            mScale << message.getMessage("scale");
            updateModelMatrix();
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
        void updateModelMatrix() {
            mModel = MatrixMath::translate(mPosition) * mRotation.matrix() * MatrixMath::scale(mScale);
        }

        Vec3f mPosition;
        Quaternion mRotation;
        Vec3f mScale = Vec3f(1);

        Mat4f mModel;
    };
}

#endif //XENGINE_TRANSFORM_HPP
