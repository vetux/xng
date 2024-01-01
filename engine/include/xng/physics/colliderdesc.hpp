/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_COLLIDERDEF_HPP
#define XENGINE_COLLIDERDEF_HPP

#include "xng/resource/resource.hpp"

#include "collidershape.hpp"
#include "xng/physics/colliderproperties.hpp"

namespace xng {
    struct ColliderDesc : public Messageable {
        ColliderProperties properties;
        ColliderShape shape;

        Messageable &operator<<(const Message &message) override {
            properties << message.getMessage("properties");
            shape << message.getMessage("shape");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            properties >> message["properties"];
            shape >> message["shape"];
            return message;
        }
    };
}

#endif //XENGINE_COLLIDERDEF_HPP
