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
#include "xng/rendergraph/surface.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng::rendergraph {
    struct Attachment {
        std::variant<Resource<Texture>, std::shared_ptr<Surface> > target{};

        /**
         * When the texture is a TEXTURE_CUBE_MAP, TEXTURE_CUBE_MAP_ARRAY, TEXTURE_2D_ARRAY
         * or TEXTURE_2D_MULTISAMPLE_ARRAY and no target array layer / face is specified,
         * it becomes a layered attachment where geometry shaders can specify the target subresource/s.
         */
        Texture::SubResource targetSubResource{};

        /**
         * An optional clear value.
         *
         * The texture is cleared with the specified value before the RasterPass callback is run.
         */
        std::optional<Texture::ClearValue> clearValue{};

        explicit Attachment(const Resource<Texture> &target)
            : target(std::move(target)) {
        }

        Attachment(const Resource<Texture> &target, Texture::ClearValue clearColor)
            : target(std::move(target)), clearValue(std::move(clearColor)) {
        }

        Attachment(const Resource<Texture> &target, const Texture::SubResource &targetSubResource)
            : target(std::move(target)), targetSubResource(targetSubResource) {
        }

        Attachment(const Resource<Texture> &target,
                   const Texture::SubResource &targetSubResource,
                   Texture::ClearValue clearColor)
            : target(std::move(target)), targetSubResource(targetSubResource), clearValue(std::move(clearColor)) {
        }

        explicit Attachment(const std::shared_ptr<Surface> &target)
            : target(std::move(target)) {
        }

        Attachment(const std::shared_ptr<Surface> &target, Texture::ClearValue clearColor)
            : target(std::move(target)), clearValue(std::move(clearColor)) {
        }
    };
}

#endif //XENGINE_RENDERGRAPH_ATTACHMENT_HPP
