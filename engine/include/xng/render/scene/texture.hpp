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

#ifndef XENGINE_TEXTURE_HPP
#define XENGINE_TEXTURE_HPP

#include "xng/gpu/texturebuffer.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/render/scene/image.hpp"

namespace xng {
    struct XENGINE_EXPORT Texture : public Resource, public Messageable {
        ~Texture() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Texture>(*this);
        }

        std::type_index getTypeIndex() const override {
            return typeid(Texture);
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
        TextureBufferDesc description;

        bool isLoaded() const override {
            return image.isLoaded();
        }
    };
}

#endif //XENGINE_TEXTURE_HPP
