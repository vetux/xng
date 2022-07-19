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

#ifndef XENGINE_SKYBOXCOMPONENT_HPP
#define XENGINE_SKYBOXCOMPONENT_HPP

#include <string>

#include "asset/skybox.hpp"

#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT SkyboxComponent : public Messageable {
        Skybox skybox;

        bool operator==(const SkyboxComponent &other) const {
            return skybox.texture == other.skybox.texture && skybox.color == other.skybox.color;
        }

        Messageable &operator<<(const Message &message) override {
            skybox << message.value("skybox");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            skybox >> message["skybox"];
            return message;
        }
    };
}
#endif //XENGINE_SKYBOXCOMPONENT_HPP
