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
     */
    struct XENGINE_EXPORT Camera : public Messageable {
        static Mat4f view(const Transform &cameraTransform) {
            Mat4f ret = cameraTransform.getRotation().matrix();
            // "The engines move the universe" - Futurama (Negate camera position)
            return ret * MatrixMath::translate(cameraTransform.getPosition() * -1);
        }

        Camera() = default;

        explicit Camera(CameraType type) : type(type) {}

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
            type = (CameraType) message.getMessage("type", Message((int) PERSPECTIVE)).asInt();
            message.value("nearClip", nearClip, 0.1f);
            message.value("farClip", farClip, 1000.0f);
            message.value("fov", fov, 60.0f);
            message.value("aspectRatio", aspectRatio, 4.0f / 3.0f);
            message.value("left", left, -1.0f);
            message.value("top", top, 1.0f);
            message.value("right", right, 1.0f);
            message.value("bottom", bottom, -1.0f);
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

        bool operator==(const Camera &other) const {
            return type == other.type
            && nearClip == other.nearClip
            && farClip == other.farClip
            && left == other.left
            && top == other.top
            && right == other.right
            && bottom == other.bottom;
        };

        bool operator!=(const Camera &other) const{
            return !(*this == other);
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
