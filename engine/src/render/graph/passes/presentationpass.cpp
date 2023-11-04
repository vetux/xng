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

#include "xng/render/graph/passes/presentationpass.hpp"

#include "xng/render/graph/framegraphbuilder.hpp"

namespace xng {
    PresentationPass::PresentationPass() = default;

    void PresentationPass::setup(FrameGraphBuilder &builder) {
        target = builder.createRenderTarget(builder.getBackBufferDescription());
        backBuffer = builder.getBackBuffer();
        screenColor = builder.getSlot(SLOT_SCREEN_COLOR);
        screenDepth = builder.getSlot(SLOT_SCREEN_DEPTH);

        builder.read(target);
        builder.write(backBuffer);
        builder.read(screenColor);
        builder.read(screenDepth);

        commandBuffer = builder.createCommandBuffer();
        builder.write(commandBuffer);
    }

    void PresentationPass::execute(FrameGraphPassResources &resources,
                                   const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                                   const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                                   const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) {
        auto &t = resources.get<RenderTarget>(target);
        auto &b = resources.get<RenderTarget>(backBuffer);

        auto &color = resources.get<TextureBuffer>(screenColor);
        auto &depth = resources.get<TextureBuffer>(screenDepth);

        auto &cBuffer = resources.get<CommandBuffer>(commandBuffer);

        t.setAttachments({RenderTargetAttachment::texture(color)}, RenderTargetAttachment::texture(depth));

        cBuffer.begin();
        cBuffer.add(
                b.blitColor(t, {}, {}, t.getDescription().size, b.getDescription().size, TextureFiltering::NEAREST, 0,
                            0));
        cBuffer.add(b.blitDepth(t, {}, {}, t.getDescription().size, b.getDescription().size));
        cBuffer.end();

        renderQueues.at(0).get().submit(cBuffer);

        t.clearAttachments();
    }
}