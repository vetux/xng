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

#ifndef XENGINE_CAMERACOMPONENT_HPP
#define XENGINE_CAMERACOMPONENT_HPP

#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT CameraComponent final : Component {
        XNG_COMPONENT_TYPENAME(CameraComponent)

        enum CameraType : int {
            ORTHOGRAPHIC = 0,
            PERSPECTIVE = 1
        } type;

        // Orthographic
        float left = -1.0f;
        float right = 1.0f;
        float bottom = -1.0f;
        float top = 1.0f;

        // Perspective
        float fov = 90;
        float aspectRatio = 4.0f / 3.0f;

        float nearClip = 0.1f;
        float farClip = 1000.0f;

        Messageable &operator<<(const Message &message) override {
            message.value("type", reinterpret_cast<int &>(type));
            left << message["left"];
            right << message["right"];
            bottom << message["bottom"];
            top << message["top"];
            fov << message["fov"];
            aspectRatio << message["aspectRatio"];
            nearClip << message["nearClip"];
            farClip << message["farClip"];
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            type >> message["type"];
            left >> message["left"];
            right >> message["right"];
            bottom >> message["bottom"];
            top >> message["top"];
            fov >> message["fov"];
            aspectRatio >> message["aspectRatio"];
            nearClip >> message["nearClip"];
            farClip >> message["farClip"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_CAMERACOMPONENT_HPP
