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

#include "xng/graphics/3d/passes/canvasrenderpass.hpp"

#include "xng/graphics/vertexstream.hpp"
#include "xng/graphics/3d/sharedresources/rendercanvases.hpp"

namespace xng {
    CanvasRenderPass::CanvasRenderPass(std::shared_ptr<RenderConfiguration> config,
                                       std::shared_ptr<SharedResourceRegistry> registry)
        : config(config), registry(registry) {
        screenSpaceLayer.name = LAYER_CANVASES_SCREEN;
        worldSpaceLayer.name = LAYER_CANVASES_WORLD;
        screenSpaceLayer.containsTransparency = true;
        worldSpaceLayer.containsTransparency = true;
    }

    bool CanvasRenderPass::shouldRebuild(const Vec2i &backBufferSize) {
        return backBufferSize != layerSize;
    }

    void CanvasRenderPass::create(RenderGraphBuilder &builder) {
        layerSize = builder.getBackBufferSize();

        RenderGraphTexture texture;
        texture.size = layerSize;
        texture.format = RGBA;
        screenSpaceLayer.color = builder.createTexture(texture);
        worldSpaceLayer.color = builder.createTexture(texture);

        texture.format = DEPTH;
        worldSpaceLayer.depth = builder.createTexture(texture);

        auto layers = registry->check<CompositingLayers>() ? registry->get<CompositingLayers>() : CompositingLayers();
        layers.layers.emplace_back(worldSpaceLayer);
        layers.layers.emplace_back(screenSpaceLayer);
        registry->set<CompositingLayers>(layers);

        canvases = registry->get<RenderCanvases>();
        auto pass = builder.addPass("CanvasRenderPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        for (auto &canvas: canvases.canvases) {
            builder.read(pass, canvas.texture);
        }
    }

    void CanvasRenderPass::recreate(RenderGraphBuilder &builder) {
        if (builder.getBackBufferSize() != layerSize) {
            layerSize = builder.getBackBufferSize();

            RenderGraphTexture texture;
            texture.size = layerSize;
            texture.format = RGBA;
            screenSpaceLayer.color = builder.createTexture(texture);
            worldSpaceLayer.color = builder.createTexture(texture);

            texture.format = DEPTH;
            worldSpaceLayer.depth = builder.createTexture(texture);
        } else {
            screenSpaceLayer.color = builder.inheritResource(screenSpaceLayer.color);
            worldSpaceLayer.color = builder.inheritResource(worldSpaceLayer.color);
            worldSpaceLayer.depth = builder.inheritResource(worldSpaceLayer.depth);
        }

        auto layers = registry->check<CompositingLayers>() ? registry->get<CompositingLayers>() : CompositingLayers();
        layers.layers.emplace_back(worldSpaceLayer);
        layers.layers.emplace_back(screenSpaceLayer);
        registry->set<CompositingLayers>(layers);

        canvases = registry->get<RenderCanvases>();
        auto pass = builder.addPass("CanvasRenderPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        for (auto &canvas: canvases.canvases) {
            builder.read(pass, canvas.texture);
        }
    }

    void CanvasRenderPass::runPass(RenderGraphContext &ctx) {
        ctx.clearTextureColor(screenSpaceLayer.color, ColorRGBA::black(1, 0));

        // Screen space canvases are always the same size as the back buffer, and there can only be one screen space canvas.
        for (auto &canvas: canvases.canvases) {
            if (canvas.worldSpace)
                continue;
            ctx.copyTexture(screenSpaceLayer.color, canvas.texture);
            break;
        }

        ctx.clearTextureColor(worldSpaceLayer.color, ColorRGBA::black(1, 0));
        ctx.clearTextureDepthStencil(worldSpaceLayer.depth, 1, 0);

        //TODO: Implement World Space canvas rendering
    }
}
