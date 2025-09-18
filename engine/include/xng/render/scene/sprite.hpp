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

#ifndef XENGINE_SPRITE_HPP
#define XENGINE_SPRITE_HPP

#include "xng/resource/resource.hpp"
#include "xng/resource/resourcehandle.hpp"

#include "xng/math/rectangle.hpp"

#include "xng/render/scene/texture.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * A sprite is a texture displayed on a planar quad mesh perpendicular to the camera.
     */
    struct XENGINE_EXPORT Sprite final : Resource, Messageable {
        RESOURCE_TYPENAME(Sprite)

        Sprite() = default;

        Sprite(ResourceHandle<ImageRGBA> image,
               Recti offset)
                : image(std::move(image)),
                  offset(std::move(offset)) {}

        ~Sprite() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Sprite>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            offset << message.getMessage("offset");
            image << message.getMessage("image");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            offset >> message["offset"];
            if (image.assigned())
                image >> message["image"];
            return message;
        }

        ResourceHandle<ImageRGBA> image{}; // The image which contains the sprite
        Recti offset{}; // The part of the image which contains the sprite

        bool isLoaded() const override {
            return image.isLoaded();
        }
    };
}

#endif //XENGINE_SPRITE_HPP
