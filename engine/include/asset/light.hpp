/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_LIGHT_HPP
#define XENGINE_LIGHT_HPP

#include "math/transform.hpp"
#include "io/messageable.hpp"

namespace xng {
    enum LightType : int {
        LIGHT_DIRECTIONAL = 0,
        LIGHT_POINT = 1,
        LIGHT_SPOT = 2
    };

    struct XENGINE_EXPORT Light : public Messageable {
        Light() : type() {}

        explicit Light(LightType type) : type(type) {}

        Messageable &operator<<(const Message &message) override {
            type = static_cast<LightType>(message.value("type", 0));
            transform << message.value("transform");
            ambient << message.value("ambient");
            diffuse << message.value("diffuse");
            specular << message.value("specular");
            direction << message.value("direction");
            cutOff = message.value("cutOff", 0.0f);
            outerCutOff = message.value("outerCutOff", 0.0f);
            constant = message.value("constant", 0.0f);
            linear = message.value("linear", 0.0f);
            quadratic = message.value("quadratic", 0.0f);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["type"] = static_cast<int>(type);
            transform >> message["transform"];
            ambient >> message["ambient"];
            diffuse >> message["diffuse"];
            specular >> message["specular"];
            direction >> message["direction"];
            message["cutOff"] = cutOff;
            message["outerCutOff"] = outerCutOff;
            message["constant"] = constant;
            message["linear"] = linear;
            message["quadratic"] = quadratic;
            return message;
        }

        LightType type;

        Transform transform;

        Vec3f ambient = Vec3f(0.1f);
        Vec3f diffuse = Vec3f(1.0f);
        Vec3f specular = Vec3f(1.0f);

        //Directional / Spot
        Vec3f direction = Vec3f(0);

        float cutOff = 10;
        float outerCutOff = 14;

        //Point / Spot
        float constant = 1;
        float linear = 0.09;
        float quadratic = 0.032;
    };
}

#endif //XENGINE_LIGHT_HPP
