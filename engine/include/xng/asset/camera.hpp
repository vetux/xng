/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_CAMERA_HPP
#define XENGINE_CAMERA_HPP

#include "xng/math/transform.hpp"
#include "xng/math/matrix.hpp"
#include "xng/math/matrixmath.hpp"

#include <stdexcept>

namespace xng {
    enum CameraType {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    /**
     * A camera provides a view and projection matrix.
     *
     * //TODO: Cache view and projection matrices
     */
    struct XENGINE_EXPORT Camera : public Messageable {
        Camera() {}

        explicit Camera(CameraType type) : type(type) {}

        Mat4f view(const Transform &transform) {
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

        Messageable &operator<<(const Message &message) override {
            type = static_cast<CameraType>(message.value("type", static_cast<int>(PERSPECTIVE)));
            nearClip = message.value("nearClip", 0.1f);
            farClip = message.value("farClip", 1000.0f);
            fov = message.value("fov", 60.0f);
            aspectRatio = message.value("aspectRatio", 4.0f / 3.0f);
            left = message.value("left", -1.0f);
            top = message.value("top", 1.0f);
            right = message.value("right", 1.0f);
            bottom = message.value("bottom", -1.0f);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["type"] = static_cast<int>(type);
            message["nearClip"] = nearClip;
            message["farClip"] = farClip;
            message["fov"] = fov;
            message["aspectRatio"] = aspectRatio;
            message["left"] = left;
            message["top"] = top;
            message["right"] = right;
            message["bottom"] = bottom;
            return message;
        }

        CameraType type = PERSPECTIVE;

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
