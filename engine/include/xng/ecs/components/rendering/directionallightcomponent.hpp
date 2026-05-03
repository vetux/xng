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

#ifndef XENGINE_DIRECTIONALLIGHTCOMPONENT_HPP
#define XENGINE_DIRECTIONALLIGHTCOMPONENT_HPP

#include "xng/ecs/component.hpp"

namespace xng {
    struct DirectionalLightComponent final : Component {
        XNG_COMPONENT_TYPENAME(DirectionalLightComponent)

        Vec3f direction{};
        ColorRGB color = ColorRGB(255, 255, 255);
        float power = 1;

        bool castShadows = false;
        float shadowNearPlane = 0.1f;
        float shadowFarPlane = 1000.0f;
        float shadowExtent = 10.0f;

        Messageable &operator<<(const Message &message) override {
            direction << message["direction"];
            color << message["color"];
            power << message["power"];
            castShadows << message["castShadows"];
            shadowNearPlane << message["shadowNearPlane"];
            shadowFarPlane << message["shadowFarPlane"];
            shadowExtent << message["shadowExtent"];
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            direction >> message["direction"];
            color >> message["color"];
            power >> message["power"];
            castShadows >> message["castShadows"];
            shadowNearPlane >> message["shadowNearPlane"];
            shadowFarPlane >> message["shadowFarPlane"];
            shadowExtent >> message["shadowExtent"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_DIRECTIONALLIGHTCOMPONENT_HPP
