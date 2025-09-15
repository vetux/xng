/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_RENDERGRAPHATTACHMENT_HPP
#define XENGINE_RENDERGRAPHATTACHMENT_HPP

#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/rendergraph/rendergraphtextureproperties.hpp"

namespace xng {
    struct RenderGraphAttachment {
        RenderGraphResource texture{};
        size_t index{};
        CubeMapFace face{};
        size_t mipMapLevel{};

        RenderGraphAttachment() = default;

        explicit RenderGraphAttachment(const RenderGraphResource texture)
            : texture(texture) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const size_t index)
            : texture(texture), index(index) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const CubeMapFace face)
            : texture(texture), face(face) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const size_t index, CubeMapFace face,
                              const size_t mipMapLevel)
            : texture(texture), index(index), face(face), mipMapLevel(mipMapLevel) {
        }
    };
}
#endif //XENGINE_RENDERGRAPHATTACHMENT_HPP
