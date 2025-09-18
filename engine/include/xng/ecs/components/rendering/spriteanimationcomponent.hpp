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

#ifndef XENGINE_ANIMATEDSPRITECOMPONENT_HPP
#define XENGINE_ANIMATEDSPRITECOMPONENT_HPP

#include "xng/animation/sprite/spriteanimation.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A sprite animation system updates the sprite animations each update and updates the sprite.
     */
    struct XENGINE_EXPORT AnimatedSpriteComponent final : Component {
        XNG_COMPONENT_TYPENAME(AnimatedSpriteComponent)

        ResourceHandle<SpriteAnimation> animation{};
        float animationSpeed = 1.0f;

        ResourceHandle<Sprite> sprite;
        bool finished = false;

        Messageable &operator<<(const Message &message) override {
            message.value("animation", animation);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            animation >> message["animation"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_ANIMATEDSPRITECOMPONENT_HPP
