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

#ifndef XENGINE_CAMERA_HPP
#define XENGINE_CAMERA_HPP

#include "xng/math/matrixmath.hpp"
#include "xng/math/transform.hpp"

namespace xng {
    class Camera {
    public:
        static Mat4f getView(const Transform &cameraTransform) {
            // "The engines move the universe" - Futurama (Negate camera position)
            return cameraTransform.getRotation().matrix()
                   * MatrixMath::translate(cameraTransform.getPosition() * -1);
        }

        static Mat4f getOrthographicProjection(const float left = -1.0f,
                                               const float right = 1.0f,
                                               const float bottom = -1.0f,
                                               const float top = 1.0f,
                                               const float nearClip = 0.1f,
                                               const float farClip = 1000.0f) {
            return MatrixMath::ortho(left, right, bottom, top, nearClip, farClip);
        }

        static Mat4f getPerspectiveProjection(const float fov = 90,
                                              const float aspectRatio = 4.0f / 3.0f,
                                              const float nearClip = 0.1f,
                                              const float farClip = 1000.0f) {
            return MatrixMath::perspective(fov, aspectRatio, nearClip, farClip);
        }

        void setTransform(const Transform &cameraTransform) {
            view = getView(cameraTransform);
            viewProjection = projection * view;
        }

        const Transform &getTransform() const {
            return transform;
        }

        Vec4f getPosition() const {
            auto pos4 = transform.getPosition();
            return Vec4f(pos4.x, pos4.y, pos4.z, 0);
        }

        void setProjection(const Mat4f &p) {
            projection = p;
            viewProjection = projection * view;
        }

        Mat4f getView() const {
            return view;
        }

        Mat4f getProjection() const {
            return projection;
        }

        Mat4f getViewProjection() const {
            return viewProjection;
        }

    private:
        Transform transform;
        Mat4f view;
        Mat4f projection;
        Mat4f viewProjection;
    };
}

#endif //XENGINE_CAMERA_HPP
