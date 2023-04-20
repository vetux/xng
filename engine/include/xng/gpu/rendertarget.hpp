/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_RENDERTARGET_HPP
#define XENGINE_RENDERTARGET_HPP

#include <variant>

#include "xng/gpu/textureproperties.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/texturearraybuffer.hpp"
#include "xng/gpu/rendertargetdesc.hpp"
#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/gpufence.hpp"

#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT RenderTarget : public RenderObject {
    public:
        enum AttachmentType {
            ATTACHMENT_TEXTURE = 0,
            ATTACHMENT_CUBEMAP = 1,
            ATTACHMENT_TEXTUREARRAY = 2
        };

        typedef std::variant<std::reference_wrapper<TextureBuffer>,
                std::pair<CubeMapFace, std::reference_wrapper<TextureBuffer>>,
                std::pair<size_t, std::reference_wrapper<TextureArrayBuffer>>> Attachment;

        ~RenderTarget() override = default;

        Type getType() override {
            return RENDER_OBJECT_RENDER_TARGET;
        }

        virtual const RenderTargetDesc &getDescription() = 0;

        virtual std::unique_ptr<GpuFence> blitColor(RenderTarget &source,
                                                    Vec2i sourceOffset,
                                                    Vec2i targetOffset,
                                                    Vec2i sourceRect,
                                                    Vec2i targetRect,
                                                    TextureFiltering filter,
                                                    int sourceIndex,
                                                    int targetIndex) = 0;

        virtual std::unique_ptr<GpuFence> blitDepth(RenderTarget &source,
                                                    Vec2i sourceOffset,
                                                    Vec2i targetOffset,
                                                    Vec2i sourceRect,
                                                    Vec2i targetRect) = 0;

        virtual std::unique_ptr<GpuFence> blitStencil(RenderTarget &source,
                                                      Vec2i sourceOffset,
                                                      Vec2i targetOffset,
                                                      Vec2i sourceRect,
                                                      Vec2i targetRect) = 0;

        virtual std::unique_ptr<GpuFence> setColorAttachments(const std::vector<Attachment> &attachments) = 0;

        virtual std::unique_ptr<GpuFence> setDepthStencilAttachment(Attachment attachment) = 0;

        virtual std::unique_ptr<GpuFence> clearDepthStencilAttachment() = 0;

        virtual bool isComplete() = 0;
    };
}

#endif //XENGINE_RENDERTARGET_HPP
