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

#ifndef XENGINE_BUTTONCOMPONENT_HPP
#define XENGINE_BUTTONCOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/render/sprite.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT ButtonComponent : public Component {
        std::string id;
        int layer; // The layer of this button used for resolving inputs on overlapping buttons
        ResourceHandle<Sprite> sprite;
        ResourceHandle<Sprite> spriteHover;
        ResourceHandle<Sprite> spritePressed;

        Messageable &operator<<(const Message &message) override {
            message.value("id", id);
            message.value("layer", layer);
            message.value("sprite", sprite);
            message.value("spriteHover", sprite);
            message.value("spritePressed", sprite);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            id >> message["id"];
            layer >> message["layer"];
            sprite >> message["sprite"];
            spriteHover >> message["spriteHover"];
            spritePressed >> message["spritePressed"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(ButtonComponent);
        }
    };
}

#endif //XENGINE_BUTTONCOMPONENT_HPP
