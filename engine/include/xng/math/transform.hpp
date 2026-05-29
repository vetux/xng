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

#ifndef XENGINE_TRANSFORM_HPP
#define XENGINE_TRANSFORM_HPP

#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
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

        /**
         * Explicitly moves along the object's local orientation vectors
         */
        void translateLocal(const Vec3f &offset) {
            // Multiply offset by current rotation quaternion/matrix first
            auto rOffset = mRotation.matrix() * Vec4f(offset.x, offset.y, offset.z, 0.0f);
            mPosition += Vec3f(rOffset.x, rOffset.y, rOffset.z);
            updateModelMatrix();
        }

        /**
         * Moves strictly along the world X, Y, Z grid lines
         */
        void translateWorld(const Vec3f &offset) {
            mPosition += offset;
            updateModelMatrix();
        }

        /**
         * Rotate around the object's own local axes (e.g. pitching up/down relative to where it looks)
         */
        void rotateLocal(const Quaternion &deltaRotation) {
            // Child First: Multiply on the RIGHT Side
            mRotation = mRotation * deltaRotation;
            updateModelMatrix();
        }

        /**
         * Rotate around the global, unmoving world coordinate grid axes
         */
        void rotateWorld(const Quaternion &deltaRotation) {
            // World First: Multiply on the LEFT Side
            mRotation = deltaRotation * mRotation;
            updateModelMatrix();
        }

        /**
         * Scale along the object's current size (Multiplicative accumulation)
         */
        void scaleLocal(const Vec3f &scaleMultiplier) {
            mScale.x *= scaleMultiplier.x;
            mScale.y *= scaleMultiplier.y;
            mScale.z *= scaleMultiplier.z;
            updateModelMatrix();
        }

        /**
         * Return world transform relative to parent.
         *
         * @param parent
         * @return
         */
        [[nodiscard]] Transform getWorldTransform(const Transform &parent) const {
            Transform ret = Transform(mPosition, mRotation, mScale);

            // 1. POSITION: Rotate and scale the child's local position by the parent,
            // then offset it by the parent's world position.
            Vec4f rotatedScaledPos = parent.model() * Vec4f(ret.mPosition.x, ret.mPosition.y, ret.mPosition.z, 1.0f);
            ret.mPosition = Vec3f(rotatedScaledPos.x, rotatedScaledPos.y, rotatedScaledPos.z);

            // 2. ROTATION: Accumulate rotations using quaternion multiplication order (Parent * Child)
            ret.mRotation = parent.mRotation * ret.mRotation;

            // 3. SCALE: Scales MUST be multiplied, never added!
            ret.mScale = ret.mScale * parent.mScale;

            ret.updateModelMatrix();

            return ret;
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
