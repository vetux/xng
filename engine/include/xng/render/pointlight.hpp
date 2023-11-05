/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_POINTLIGHT_HPP
#define XENGINE_POINTLIGHT_HPP

#include "xng/math/transform.hpp"
#include "xng/io/messageable.hpp"
#include "xng/render/color.hpp"

namespace xng {
    struct XENGINE_EXPORT PointLight : public Messageable {
        Messageable &operator<<(const Message &message) override {
            color << message.getMessage("color");
            message.value("power", power);
            message.value("castShadows", castShadows);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            color >> message["color"];
            message["power"] = power;
            message["castShadows"] = castShadows;
            return message;
        }

        ColorRGBA color = ColorRGBA::white(); // The color of the light
        float power = 1; // The strength of the light

        bool castShadows = true;
    };
}

#endif //XENGINE_POINTLIGHT_HPP
