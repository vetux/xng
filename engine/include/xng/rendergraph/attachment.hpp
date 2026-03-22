/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERGRAPH_ATTACHMENT_HPP
#define XENGINE_RENDERGRAPH_ATTACHMENT_HPP

#include <variant>
#include <optional>

#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/textureproperties.hpp"
#include "xng/rendergraph/color.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng::rendergraph {
    struct Attachment {
        Resource<Texture> texture{};

        /**
         * When the texture is a TEXTURE_CUBE_MAP, TEXTURE_2D_ARRAY or TEXTURE_CUBE_MAP_ARRAY and no target is specified,
         * it becomes a layered attachment where geometry shaders can specify the target subresource/s.
         *
         * Mip level 0 is used for TEXTURE_2D_* without a specified target.
         */
        std::optional<Texture::SubResource> target{};

        /**
         * An optional clear value.
         *
         * The texture is cleared with the specified value before the RasterPass callback is run.
         */
        std::optional<Texture::ClearValue> clearValue{};

        Attachment() = default;

        explicit Attachment(Resource<Texture> texture)
            : texture(std::move(texture)) {
        }

        Attachment(Resource<Texture> texture, Texture::ClearValue clearColor)
            : texture(std::move(texture)), clearValue(std::move(clearColor)) {
        }

        Attachment(Resource<Texture> texture, const Texture::SubResource &target)
            : texture(std::move(texture)), target(target) {
        }

        Attachment(Resource<Texture> texture, const Texture::SubResource &target, Texture::ClearValue clearColor)
            : texture(std::move(texture)), target(target), clearValue(std::move(clearColor)) {
        }
    };
}

#endif //XENGINE_RENDERGRAPH_ATTACHMENT_HPP
