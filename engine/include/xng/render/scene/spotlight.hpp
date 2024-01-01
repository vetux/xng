/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SPOTLIGHT_HPP
#define XENGINE_SPOTLIGHT_HPP

#include "xng/math/transform.hpp"
#include "xng/math/rotation.hpp"
#include "xng/io/messageable.hpp"
#include "xng/render/scene/color.hpp"

namespace xng {
    struct XENGINE_EXPORT SpotLight : public Messageable {
        Messageable &operator<<(const Message &message) override {
            color << message.getMessage("color");
            message.value("power", power);
            message.value("castShadows", castShadows);
            message.value("shadowNearPlane", shadowNearPlane);
            message.value("shadowFarPlane", shadowFarPlane);
            message.value("direction", direction);
            message.value("quadratic", quadratic);
            message.value("cutOff", cutOff);
            message.value("outerCutOff", outerCutOff);
            message.value("constant", constant);
            message.value("linear", linear);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            color >> message["color"];
            power >> message["power"];
            castShadows >> message["castShadows"];
            shadowNearPlane >> message["shadowNearPlane"];
            shadowFarPlane >> message["shadowFarPlane"];
            direction >> message["direction"];
            quadratic >> message["quadratic"];
            cutOff >> message["cutOff"];
            outerCutOff >> message["outerCutOff"];
            constant >> message["constant"];
            linear >> message["linear"];
            return message;
        }

        ColorRGBA color = ColorRGBA::white(); // The color of the light
        float power = 10; // The strength of the light
        Vec3f direction = Vec3f(0, 0, -1);

        float cutOff = 12.5; // The inner cut off angle in degrees
        float outerCutOff = 17.5; // The outer cut off angle in degrees, The light is faded out between cutOff / outerCutOff

        float quadratic = 0.032f;
        float constant = 1;
        float linear = 0.09;

        bool castShadows = true;
        float shadowNearPlane = 1;
        float shadowFarPlane = 50;
    };
}

#endif //XENGINE_SPOTLIGHT_HPP
