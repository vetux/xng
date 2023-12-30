/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_DIRECTIONALLIGHT_HPP
#define XENGINE_DIRECTIONALLIGHT_HPP

#include "xng/math/transform.hpp"
#include "xng/io/messageable.hpp"
#include "xng/render/scene/color.hpp"

namespace xng {
    struct XENGINE_EXPORT DirectionalLight : public Messageable {
        Messageable &operator<<(const Message &message) override {
            color << message.getMessage("color");
            message.value("power", power);
            message.value("castShadows", castShadows);
            message.value("shadowNearPlane", shadowNearPlane);
            message.value("shadowFarPlane", shadowFarPlane);
            message.value("shadowPosition", shadowPosition);
            message.value("direction", direction);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            color >> message["color"];
            power >> message["power"];
            castShadows >> message["castShadows"];
            shadowNearPlane >> message["shadowNearPlane"];
            shadowFarPlane >> message["shadowFarPlane"];
            shadowPosition >> message["shadowPosition"];
            direction >> message["direction"];
            return message;
        }

        ColorRGBA color = ColorRGBA::white(); // The color of the light
        float power = 1; // The strength of the light, for directional lights should be in the range 0 - 1
        Vec3f direction = Vec3f(0.5, -1, -1);

        bool castShadows = true;

        float shadowNearPlane = -10;
        float shadowFarPlane = 100;

        Vec2f shadowPosition{}; // The x and z coordinates used for generating the shadow map for this light
        float shadowProjectionExtent = 50; // The extent of the orthographic projection used for generating shadow maps,
                                            // larger values means larger shadow area around the shadowPosition at the cost of reduced resolution.
    };
}

#endif //XENGINE_DIRECTIONALLIGHT_HPP
