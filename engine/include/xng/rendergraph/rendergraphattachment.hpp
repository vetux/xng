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
#include "xng/render/scene/color.hpp"

namespace xng {
    struct RenderGraphAttachment {
        RenderGraphResource texture{};
        size_t index{};
        CubeMapFace face{};
        size_t mipMapLevel{};

        bool clearAttachment{false};

        ColorRGBA clearColor{};
        Vec4i clearColorInt{};
        Vec4u clearColorUint{};

        float clearDepth{};
        unsigned int clearStencil{};

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

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel)
            : texture(texture), index(index), face(face), mipMapLevel(mipMapLevel) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const ColorRGBA clearColor)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColor(clearColor) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const Vec4i clearColor)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColorInt(clearColor) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const Vec4u clearColor)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColorUint(clearColor) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const float clearDepth)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearDepth(clearDepth) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const unsigned int clearStencil)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearStencil(clearStencil) {
        }
    };
}
#endif //XENGINE_RENDERGRAPHATTACHMENT_HPP
