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

#ifndef XENGINE_AUDIOLISTENERCOMPONENT_HPP
#define XENGINE_AUDIOLISTENERCOMPONENT_HPP

#include "math/vector3.hpp"

#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT AudioListenerComponent : public Messageable {
        Vec3f velocity;

        Messageable &operator<<(const Message &message) override {
            velocity << message.value("velocity");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            velocity >> message["velocity"];
            return message;
        }
    };
}
#endif //XENGINE_AUDIOLISTENERCOMPONENT_HPP
