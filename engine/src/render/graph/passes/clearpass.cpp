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

namespace xng {
    ClearPass::ClearPass() {

    }

    void ClearPass::setup(FrameGraphBuilder &builder) {
        renderSize = builder.getRenderSize();
        RenderTargetDesc targetDesc;
        targetDesc.size = renderSize;
        targetDesc.numberOfColorAttachments = 4;
        targetDesc.hasDepthStencilAttachment = true;
        clearTargetRes = builder.createRenderTarget(targetDesc);

        builder.read(clearTargetRes);

        RenderPassDesc passDesc;
        passDesc.numberOfColorAttachments = 4;
        passDesc.hasDepthStencilAttachment = true;
        clearPassRes = builder.createRenderPass(passDesc);
        builder.read(clearPassRes);

        auto desc = TextureBufferDesc();
        desc.size = renderSize;
        desc.format = RGBA;

        screenColor = builder.createTextureBuffer(desc);
        deferredColor = builder.createTextureBuffer(desc);
        forwardColor = builder.createTextureBuffer(desc);
        backgroundColor = builder.createTextureBuffer(desc);

        desc.format = DEPTH_STENCIL;

        screenDepth = builder.createTextureBuffer(desc);
        deferredDepth = builder.createTextureBuffer(desc);
        forwardDepth = builder.createTextureBuffer(desc);

        builder.write(screenColor);
        builder.write(screenDepth);

        builder.write(deferredColor);
        builder.write(deferredDepth);

        builder.write(forwardColor);
        builder.write(forwardDepth);

        builder.write(backgroundColor);

        builder.assignSlot(SLOT_SCREEN_COLOR, screenColor);
        builder.assignSlot(SLOT_SCREEN_DEPTH, screenDepth);

        builder.assignSlot(SLOT_DEFERRED_COLOR, deferredColor);
        builder.assignSlot(SLOT_DEFERRED_DEPTH, deferredDepth);

        builder.assignSlot(SLOT_FORWARD_COLOR, forwardColor);
        builder.assignSlot(SLOT_FORWARD_DEPTH, forwardDepth);

        builder.assignSlot(SLOT_BACKGROUND_COLOR, backgroundColor);

        commandBuffer = builder.createCommandBuffer();
        builder.write(commandBuffer);
    }

    void ClearPass::execute(FrameGraphPassResources &resources,
                            const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                            const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                            const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) {
        auto &screenColorTex = resources.get<TextureBuffer>(screenColor);
        auto &screenDepthTex = resources.get<TextureBuffer>(screenDepth);

        auto &deferredColorTex = resources.get<TextureBuffer>(deferredColor);
        auto &deferredDepthTex = resources.get<TextureBuffer>(deferredDepth);

        auto &forwardColorTex = resources.get<TextureBuffer>(forwardColor);
        auto &forwardDepthTex = resources.get<TextureBuffer>(forwardDepth);

        auto &backgroundColorTex = resources.get<TextureBuffer>(backgroundColor);

        auto &clearTarget = resources.get<RenderTarget>(clearTargetRes);
        auto &clearPass = resources.get<RenderPass>(clearPassRes);

        auto &cBuffer = resources.get<CommandBuffer>(commandBuffer);

        // Clear textures
        clearTarget.setAttachments({RenderTargetAttachment::texture(screenColorTex),
                                    RenderTargetAttachment::texture(deferredColorTex),
                                    RenderTargetAttachment::texture(forwardColorTex),
                                    RenderTargetAttachment::texture(backgroundColorTex)},
                                   RenderTargetAttachment::texture(screenDepthTex));

        std::vector<Command> commands;

        commands.emplace_back(clearPass.begin(clearTarget));
        commands.emplace_back(clearPass.clearColorAttachments(ColorRGBA(0)));
        commands.emplace_back(clearPass.clearDepthAttachment(1));
        commands.emplace_back(clearPass.end());

        cBuffer.begin();
        cBuffer.add(commands);
        cBuffer.end();

        commands.clear();

        renderQueues.at(0).get().submit({cBuffer}, {}, {});

        clearTarget.setAttachments({RenderTargetAttachment::texture(screenColorTex),
                                    RenderTargetAttachment::texture(deferredColorTex),
                                    RenderTargetAttachment::texture(forwardColorTex),
                                    RenderTargetAttachment::texture(backgroundColorTex)},
                                   RenderTargetAttachment::texture(deferredDepthTex));

        commands.emplace_back(clearPass.begin(clearTarget));
        commands.emplace_back(clearPass.clearDepthAttachment(1));
        commands.emplace_back(clearPass.end());

        cBuffer.begin();
        cBuffer.add(commands);
        cBuffer.end();

        commands.clear();

        renderQueues.at(0).get().submit({cBuffer}, {}, {});

        clearTarget.setAttachments({RenderTargetAttachment::texture(screenColorTex),
                                    RenderTargetAttachment::texture(deferredColorTex),
                                    RenderTargetAttachment::texture(forwardColorTex),
                                    RenderTargetAttachment::texture(backgroundColorTex)},
                                   RenderTargetAttachment::texture(forwardDepthTex));

        commands.emplace_back(clearPass.begin(clearTarget));
        commands.emplace_back(clearPass.clearDepthAttachment(1));
        commands.emplace_back(clearPass.end());

        cBuffer.begin();
        cBuffer.add(commands);
        cBuffer.end();

        commands.clear();

        renderQueues.at(0).get().submit({cBuffer}, {}, {});

        clearTarget.clearAttachments();
    }

    std::type_index ClearPass::getTypeIndex() const {
        return typeid(ClearPass);
    }
}