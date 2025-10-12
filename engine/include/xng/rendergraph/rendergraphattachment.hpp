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

#ifndef XENGINE_RENDERGRAPHATTACHMENT_HPP
#define XENGINE_RENDERGRAPHATTACHMENT_HPP

#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/rendergraph/rendergraphtextureproperties.hpp"
#include "xng/graphics/color.hpp"

namespace xng {
    struct RenderGraphAttachment {
        RenderGraphResource texture{};
        size_t index{};
        CubeMapFace face{};
        size_t mipMapLevel{};

        bool clearAttachment{false};

        ColorRGBA clearColor{};
        Vec4f clearColorFloat{};
        Vec4i clearColorInt{};
        Vec4u clearColorUint{};

        float clearDepth{};
        unsigned int clearStencil{};

        // If texture is a TEXTURE_CUBE_MAP, TEXTURE_2D_ARRAY or TEXTURE_CUBE_MAP_ARRAY either an index / face can
        // be specified or the texture can be attached layered where geometry shaders can set the draw layer.
        bool layered{true};

        RenderGraphAttachment() = default;

        explicit RenderGraphAttachment(const RenderGraphResource texture)
            : texture(texture) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const ColorRGBA clearColor)
            : texture(texture), clearAttachment(true), clearColor(clearColor) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const Vec4f &clearColorFloat)
            : texture(texture), clearAttachment(true), clearColorFloat(clearColorFloat) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const Vec4i &clearColorInt)
            : texture(texture), clearAttachment(true), clearColorInt(clearColorInt) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const Vec4u &clearColorUint)
            : texture(texture), clearAttachment(true), clearColorUint(clearColorUint) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const float clearDepth,
                              const unsigned int clearStencil)
            : texture(texture), clearAttachment(true), clearDepth(clearDepth), clearStencil(clearStencil) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const size_t index)
            : texture(texture), index(index), layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture, const CubeMapFace face)
            : texture(texture), face(face), layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel)
            : texture(texture), index(index), face(face), mipMapLevel(mipMapLevel), layered(false) {
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
              clearColor(clearColor),
              layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const Vec4f &clearColorFloat)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColorFloat(clearColorFloat),
              layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const Vec4i &clearColor)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColorInt(clearColor),
              layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const Vec4u &clearColor)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearColorUint(clearColor),
              layered(false) {
        }

        RenderGraphAttachment(const RenderGraphResource texture,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel,
                              const float clearDepth,
                              const unsigned int clearStencil)
            : texture(texture),
              index(index),
              face(face),
              mipMapLevel(mipMapLevel),
              clearAttachment(true),
              clearDepth(clearDepth),
              clearStencil(clearStencil),
              layered(false) {
        }
    };
}
#endif //XENGINE_RENDERGRAPHATTACHMENT_HPP
