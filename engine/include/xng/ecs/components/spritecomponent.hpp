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

#ifndef XENGINE_SPRITECOMPONENT_HPP
#define XENGINE_SPRITECOMPONENT_HPP

#include "xng/render/scene/sprite.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A sprite is an image displayed on a plane in 3d space.
     */
    struct SpriteComponent : public Component {
        ResourceHandle<Sprite> sprite; // The static sprite to draw
        TextureFiltering filter = NEAREST;

        Messageable &operator<<(const Message &message) override {
            message.value("sprite", sprite);
            message.value("filter", reinterpret_cast<int &>(filter), static_cast<int>(NEAREST));
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            sprite >> message["sprite"];
            filter >> message["filter"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(SpriteComponent);
        }
    };
}

#endif //XENGINE_SPRITECOMPONENT_HPP
