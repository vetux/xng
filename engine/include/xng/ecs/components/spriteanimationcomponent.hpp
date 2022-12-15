/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_SPRITEANIMATIONCOMPONENT_HPP
#define XENGINE_SPRITEANIMATIONCOMPONENT_HPP

#include "xng/animation/sprite/spriteanimation.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A sprite animation system updates the sprite animations each update
     * and sets the sprite instance on the sprite render component of the entity if it exists.
     */
    struct XENGINE_EXPORT SpriteAnimationComponent : public Component {
        ResourceHandle<SpriteAnimation> animation{};
        float animationDurationOverride = -1;
        bool finished = false;

        Messageable &operator<<(const Message &message) override {
            animation << message.getMessage("animation");
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            animation >> message["animation"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(SpriteAnimationComponent);
        }
    };
}
#endif //XENGINE_SPRITEANIMATIONCOMPONENT_HPP
