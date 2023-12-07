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

#ifndef XENGINE_COLLIDERPROPERTIES_HPP
#define XENGINE_COLLIDERPROPERTIES_HPP

#include "xng/io/messageable.hpp"

namespace xng {
    struct ColliderProperties : public Messageable {
        float friction = 1.0f;
        float restitution = 0;
        float restitution_threshold = 0;
        float density = 1;
        bool isSensor = false;

        Messageable &operator<<(const Message &message) override {
            message.value("friction", friction, 1.0f);
            message.value("restitution", restitution, 0.0f);
            message.value("restitution_threshold", restitution_threshold, 0.0f);
            message.value("density", density, 1.0f);
            message.value("isSensor", isSensor, false);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["friction"] = friction;
            message["restitution"] = restitution;
            message["restitution_threshold"] = restitution_threshold;
            message["density"] = density;
            message["isSensor"] = isSensor;
            return message;
        }
    };
}
#endif //XENGINE_COLLIDERPROPERTIES_HPP
