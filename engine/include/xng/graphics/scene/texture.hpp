/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
#include "xng/graphics/image.hpp"
#include "xng/rendergraph/rendergraphtexture.hpp"

namespace xng {
    struct XENGINE_EXPORT Texture final : Resource, Messageable {
        RESOURCE_TYPENAME(Texture)

        enum Filtering : int {
            NEAREST = 0,
            LINEAR,
            BICUBIC
        };

        ~Texture() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Texture>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("image", image);
            message.value("filter", reinterpret_cast<int &>(filter));
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            image >> message["image"];
            filter >> message["filter"];
            return message;
        }

        ResourceHandle<ImageRGBA> image;
        Filtering filter = LINEAR;

        bool isLoaded() const override {
            return image.isLoaded();
        }
    };
}

#endif //XENGINE_TEXTURE_HPP
