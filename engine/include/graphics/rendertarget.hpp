/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERTARGET_HPP
#define XENGINE_RENDERTARGET_HPP

#include "graphics/renderobject.hpp"
#include "graphics/textureproperties.hpp"
#include "graphics/texturebuffer.hpp"
#include "graphics/rendertargetdesc.hpp"

#include "math/vector2.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderTarget : public RenderObject {
    public:
        ~RenderTarget() override = default;

        virtual const RenderTargetDesc &getDescription() = 0;

        virtual void blitColor(RenderTarget &source,
                               Vec2i sourceOffset,
                               Vec2i targetOffset,
                               Vec2i sourceRect,
                               Vec2i targetRect,
                               TextureFiltering filter,
                               int sourceIndex,
                               int targetIndex) = 0;

        virtual void blitDepth(RenderTarget &source,
                               Vec2i sourceOffset,
                               Vec2i targetOffset,
                               Vec2i sourceRect,
                               Vec2i targetRect) = 0;

        virtual void blitStencil(RenderTarget &source,
                                 Vec2i sourceOffset,
                                 Vec2i targetOffset,
                                 Vec2i sourceRect,
                                 Vec2i targetRect) = 0;

        virtual void setColorAttachments(const std::vector<TextureBuffer *> &textures) = 0;

        virtual void setDepthStencilAttachment(TextureBuffer *texture) = 0;

        virtual void setColorAttachments(const std::vector<std::pair<CubeMapFace, TextureBuffer *>> &textures) = 0;

        virtual void setDepthStencilAttachment(CubeMapFace face, TextureBuffer *texture) = 0;

        virtual bool isComplete() = 0;
    };
}

#endif //XENGINE_RENDERTARGET_HPP
