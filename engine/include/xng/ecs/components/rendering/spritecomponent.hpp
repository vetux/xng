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

#ifndef XENGINE_SPRITECOMPONENT_HPP
#define XENGINE_SPRITECOMPONENT_HPP

#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A sprite is an image displayed on a plane in 3d space.
     */
    struct SpriteComponent final : Component {
        XNG_COMPONENT_TYPENAME(SpriteComponent)

        ResourceHandle<ImageRGBA> image;
        Recti imageSubRegion;

        bool filter = false; // Whether to apply texture filtering when sampling from the image

        Messageable &operator<<(const Message &message) override {
            image << message["image"];
            imageSubRegion << message["imageSubRegion"];
            filter << message["filter"];
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            image >> message["image"];
            imageSubRegion >> message["imageSubRegion"];
            filter >> message["filter"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_SPRITECOMPONENT_HPP
