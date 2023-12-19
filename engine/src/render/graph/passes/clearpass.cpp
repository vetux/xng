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

#include "xng/render/graph/passes/clearpass.hpp"

#include "xng/render/graph/framegraphsettings.hpp"

namespace xng {
    ClearPass::ClearPass() {

    }

    void ClearPass::setup(FrameGraphBuilder &builder) {
        auto renderSize = builder.getBackBufferDescription().size
                     * builder.getSettings().get<float>(FrameGraphSettings::SETTING_RENDER_SCALE);

        auto desc = TextureBufferDesc();
        desc.size = renderSize;
        desc.format = RGBA;

        auto screenColor = builder.createTextureBuffer(desc);
        auto deferredColor = builder.createTextureBuffer(desc);
        auto forwardColor = builder.createTextureBuffer(desc);
        auto backgroundColor = builder.createTextureBuffer(desc);

        desc.format = DEPTH_STENCIL;

        auto screenDepth = builder.createTextureBuffer(desc);
        auto deferredDepth = builder.createTextureBuffer(desc);
        auto forwardDepth = builder.createTextureBuffer(desc);

        builder.assignSlot(SLOT_SCREEN_COLOR, screenColor);
        builder.assignSlot(SLOT_SCREEN_DEPTH, screenDepth);

        builder.assignSlot(SLOT_DEFERRED_COLOR, deferredColor);
        builder.assignSlot(SLOT_DEFERRED_DEPTH, deferredDepth);

        builder.assignSlot(SLOT_FORWARD_COLOR, forwardColor);
        builder.assignSlot(SLOT_FORWARD_DEPTH, forwardDepth);

        builder.assignSlot(SLOT_BACKGROUND_COLOR, backgroundColor);

        builder.beginPass({FrameGraphAttachment::texture(screenColor),
                           FrameGraphAttachment::texture(deferredColor),
                           FrameGraphAttachment::texture(forwardColor),
                           FrameGraphAttachment::texture(backgroundColor)},
                          FrameGraphAttachment::texture(screenDepth));
        builder.clearColor(ColorRGBA::black());
        builder.clearDepth(1);
        builder.finishPass();

        builder.beginPass({}, FrameGraphAttachment::texture(deferredDepth));
        builder.clearDepth(1);
        builder.finishPass();

        builder.beginPass({}, FrameGraphAttachment::texture(forwardDepth));
        builder.clearDepth(1);
        builder.finishPass();
    }

    std::type_index ClearPass::getTypeIndex() const {
        return typeid(ClearPass);
    }
}