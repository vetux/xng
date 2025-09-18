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

#ifndef XENGINE_TEXTURE_HPP
#define XENGINE_TEXTURE_HPP

#include "xng/resource/resourcehandle.hpp"
#include "xng/render/scene/image.hpp"
#include "xng/rendergraph/texture.hpp"

namespace xng {
    struct XENGINE_EXPORT Texture final : Resource, Messageable {
        RESOURCE_TYPENAME(Texture)

        ~Texture() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Texture>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("image", image);
            message.value("description", description);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            image >> message["image"];
            description >> message["description"];
            return message;
        }

        ResourceHandle<ImageRGBA> image;
        RenderGraphTexture description;

        bool isLoaded() const override {
            return image.isLoaded();
        }
    };
}

#endif //XENGINE_TEXTURE_HPP
