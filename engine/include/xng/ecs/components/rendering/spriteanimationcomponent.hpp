/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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

#include "xng/ecs/component.hpp"

#include "xng/assets/spriteanimation.hpp"
#include "xng/util/time.hpp"

namespace xng {
    /**
     * A sprite animation system advances the sprite animations each update and sets the sprite to be rendered
     * on the sprite component of the entity.
     */
    struct XENGINE_EXPORT SpriteAnimationComponent final : Component {
        XNG_COMPONENT_TYPENAME(SpriteAnimationComponent)

        ResourceHandle<SpriteAnimation> animation;

        float animationSpeed = 1.0f; // Multiplier for the animation fps
        bool loop = true; // Whether the animation should loop

        Duration elapsedTime = Duration(0);

        Messageable &operator<<(const Message &message) override {
            animation << message["animation"];
            animationSpeed << message["animationSpeed"];
            loop << message["loop"];
            elapsedTime << message["elapsedTime"];
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            animation >> message["animation"];
            animationSpeed >> message["animationSpeed"];
            loop >> message["loop"];
            elapsedTime >> message["elapsedTime"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_ANIMATEDSPRITECOMPONENT_HPP
