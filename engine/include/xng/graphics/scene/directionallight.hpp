/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
#include "xng/graphics/color.hpp"

namespace xng {
    struct XENGINE_EXPORT DirectionalLight : public Messageable {
        ColorRGBA color = ColorRGBA::white(); // The color of the light
        float power = 1; // The strength of the light, for directional lights should be in the range 0 - 1

        bool castShadows = true;

        // The Near / Far plane of the orthographic projection used to generate the shadow map.
        // Must encompass the desired depth of the scene.
        float shadowNearPlane = -100;
        float shadowFarPlane = 100;

        // The extent of the orthographic projection used for generating the shadow map.
        // Larger values mean a larger part of the scene is rendered into the shadow map.
        // The light transform controls the x, y offset for generating the shadow map.
        // The light transform should typically be synced to the camera position.
        float shadowProjectionExtent = 10;

        Messageable &operator<<(const Message &message) override {
            color << message.getMessage("color");
            message.value("power", power);
            message.value("castShadows", castShadows);
            message.value("shadowNearPlane", shadowNearPlane);
            message.value("shadowFarPlane", shadowFarPlane);
            message.value("shadowProjectionExtent", shadowProjectionExtent);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            color >> message["color"];
            power >> message["power"];
            castShadows >> message["castShadows"];
            shadowNearPlane >> message["shadowNearPlane"];
            shadowFarPlane >> message["shadowFarPlane"];
            shadowProjectionExtent >> message["shadowProjectionExtent"];
            return message;
        }

        bool operator==(const DirectionalLight &other) const {
            return color == other.color
                   && power == other.power
                   && castShadows == other.castShadows
                   && shadowNearPlane == other.shadowNearPlane
                   && shadowFarPlane == other.shadowFarPlane
                   && shadowProjectionExtent == other.shadowProjectionExtent;
        }

        bool operator!=(const DirectionalLight &other) const {
            return !(*this == other);
        }
    };
}

#endif //XENGINE_DIRECTIONALLIGHT_HPP
