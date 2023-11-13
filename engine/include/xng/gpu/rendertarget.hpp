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

#ifndef XENGINE_RENDERTARGET_HPP
#define XENGINE_RENDERTARGET_HPP

#include <utility>
#include <variant>

#include "xng/gpu/rendertargetattachment.hpp"

#include "xng/gpu/textureproperties.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/texturearraybuffer.hpp"
#include "xng/gpu/rendertargetdesc.hpp"
#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/command.hpp"

#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT RenderTarget : public RenderObject {
    public:
        ~RenderTarget() override = default;

        Type getType() override {
            return RENDER_OBJECT_RENDER_TARGET;
        }

        virtual const RenderTargetDesc &getDescription() = 0;

        Command blitColor(RenderTarget &source,
                          Vec2i sourceOffset,
                          Vec2i targetOffset,
                          Vec2i sourceRect,
                          Vec2i targetRect,
                          TextureFiltering filter,
                          int sourceIndex,
                          int targetIndex) {
            return {Command::BLIT_COLOR,
                    RenderTargetBlit(&source,
                                     this,
                                     std::move(sourceOffset),
                                     std::move(targetOffset),
                                     std::move(sourceRect),
                                     std::move(targetRect),
                                     filter,
                                     sourceIndex,
                                     targetIndex)};
        }

        Command blitDepth(RenderTarget &source,
                          Vec2i sourceOffset,
                          Vec2i targetOffset,
                          Vec2i sourceRect,
                          Vec2i targetRect) {
            return {Command::BLIT_DEPTH,
                    RenderTargetBlit(&source,
                                     this,
                                     std::move(sourceOffset),
                                     std::move(targetOffset),
                                     std::move(sourceRect),
                                     std::move(targetRect),
                                     {},
                                     {},
                                     {})};
        }

        Command blitStencil(RenderTarget &source,
                            Vec2i sourceOffset,
                            Vec2i targetOffset,
                            Vec2i sourceRect,
                            Vec2i targetRect) {
            return {Command::BLIT_STENCIL,
                    RenderTargetBlit(&source,
                                     this,
                                     std::move(sourceOffset),
                                     std::move(targetOffset),
                                     std::move(sourceRect),
                                     std::move(targetRect),
                                     {},
                                     {},
                                     {})};
        }

        virtual void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments) = 0;

        virtual void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments,
                                    RenderTargetAttachment depthStencilAttachment) = 0;

        virtual void clearAttachments() = 0;

        virtual bool isComplete() = 0;
    };
}

#endif //XENGINE_RENDERTARGET_HPP
