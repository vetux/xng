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

#ifndef XENGINE_BUTTONCOMPONENT_HPP
#define XENGINE_BUTTONCOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/assets/image.hpp"

namespace xng {
    struct XENGINE_EXPORT ButtonComponent final : Component {
        XNG_COMPONENT_TYPENAME(ButtonComponent)

        std::string id;
        int layer; // The layer of this button used for resolving inputs on overlapping buttons
        ResourceHandle<ImageRGBA> sprite;
        ResourceHandle<ImageRGBA> spriteHover;
        ResourceHandle<ImageRGBA> spritePressed;

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
    };
}

#endif //XENGINE_BUTTONCOMPONENT_HPP
