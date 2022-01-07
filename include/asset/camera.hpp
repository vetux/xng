/**
 *  Mana - 3D Game Engine
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

#ifndef XENGINE_CAMERA_HPP
#define XENGINE_CAMERA_HPP

#include "math/transform.hpp"
#include "math/matrix.hpp"
#include "math/matrixmath.hpp"

#include <stdexcept>

namespace xengine {
    enum CameraType {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    /**
     * A camera provides a view and projection matrix.
     */
    struct XENGINE_EXPORT Camera {
        Camera() {}

        explicit Camera(CameraType type) : type(type) {}

        Mat4f view() const {
            Mat4f ret = transform.getRotation().matrix();

            // "The engines move the universe" - Futurama (Negate camera position)
            return ret * MatrixMath::translate(transform.getPosition() * -1);
        }

        Mat4f projection() const {
            switch (type) {
                case ORTHOGRAPHIC:
                    return MatrixMath::ortho(left,
                                             right,
                                             bottom,
                                             top,
                                             nearClip,
                                             farClip);
                case PERSPECTIVE:
                    return MatrixMath::perspective(fov,
                                                   aspectRatio,
                                                   nearClip,
                                                   farClip);
                default:
                    throw std::runtime_error("Unsupported camera type");
            }
        }

        CameraType type = PERSPECTIVE;

        Transform transform;

        float nearClip = 0.1f;
        float farClip = 1000.0f;

        //Perspective
        float fov = 60;
        float aspectRatio = 4.0f / 3.0f;

        //Orthographic
        float left = -1;
        float top = 1;
        float right = 1;
        float bottom = -1;
    };
}

#endif //XENGINE_CAMERA_HPP
