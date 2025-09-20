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

#ifndef XENGINE_SKYBOX_HPP
#define XENGINE_SKYBOX_HPP

#include <array>

#include "xng/resource/uri.hpp"
#include "xng/resource/resourcehandle.hpp"

#include "xng/graphics/scene/cubemap.hpp"
#include "xng/graphics/color.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT Skybox final : Resource, Messageable {
        RESOURCE_TYPENAME(Skybox)

        ~Skybox() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Skybox>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            color << message.getMessage("color");
            texture << message.getMessage("texture");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            color >> message["color"];
            if (texture.assigned())
                texture >> message["texture"];
            return message;
        }

        ColorRGBA color = {12, 123, 123, 255}; // If texture is unassigned skybox color is drawn
        ResourceHandle<CubeMap> texture; // The cube map texture of a static skybox, for more advanced animated skyboxes the user should create custom passes to achieve the desired effect.

        bool isLoaded() const override {
            return texture.isLoaded() && texture.get().isLoaded();
        }
    };
}

#endif //XENGINE_SKYBOX_HPP
