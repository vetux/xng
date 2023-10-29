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

#ifndef XENGINE_DIRECTIONALLIGHT_HPP
#define XENGINE_DIRECTIONALLIGHT_HPP

#include "xng/math/transform.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT DirectionalLight : public Messageable {
        Messageable &operator<<(const Message &message) override {
            ambient << message.getMessage("ambient");
            diffuse << message.getMessage("diffuse");
            specular << message.getMessage("specular");
            direction << message.getMessage("direction");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            ambient >> message["ambient"];
            diffuse >> message["diffuse"];
            specular >> message["specular"];
            direction >> message["direction"];
            return message;
        }

        Vec3f ambient = Vec3f(0.2);
        Vec3f diffuse = Vec3f(0.4);
        Vec3f specular = Vec3f(0.1);

        Vec3f direction = Vec3f(0.0f, 0.0f, -1.0f);
    };
}

#endif //XENGINE_DIRECTIONALLIGHT_HPP
