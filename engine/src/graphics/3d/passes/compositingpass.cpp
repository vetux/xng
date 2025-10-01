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

#include "xng/graphics/3d/passes/compositingpass.hpp"

#include "xng/graphics/vertexstream.hpp"

namespace xng {
    CompositingPass::CompositingPass(std::shared_ptr<RenderConfiguration> config,
                                     std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(config)),
          registry(std::move(registry)) {
        normalizedQuad = Mesh::normalizedQuad();
    }

    void CompositingPass::create(RenderGraphBuilder &builder) {
        RenderGraphTexture depthTex;
        depthTex.size = {1, 1};
        depthTex.format = DEPTH;
        defaultDepthTexture = builder.createTexture(depthTex);

        RenderGraphPipeline pip;
        pip.shaders.emplace_back(createVertexShader());
        pip.shaders.emplace_back(createFragmentShader());
        pip.enableBlending = true;
        pip.enableDepthTest = true;
        pip.depthTestWrite = true;
        //https://stackoverflow.com/a/16938711
        pip.colorBlendSourceMode = RenderGraphPipeline::SRC_ALPHA;
        pip.colorBlendDestinationMode = RenderGraphPipeline::ONE_MINUS_SRC_ALPHA;
        pip.alphaBlendSourceMode = RenderGraphPipeline::ONE;
        pip.alphaBlendDestinationMode = RenderGraphPipeline::ONE_MINUS_SRC_ALPHA;
        pipeline = builder.createPipeline(pip);

        vertexBuffer = builder.createVertexBuffer(normalizedQuad.vertexLayout.getLayoutSize()
                                                  * normalizedQuad.vertices.size());

        backBufferColor = builder.getBackBufferColor();
        backBufferDepth = builder.getBackBufferDepthStencil();

        if (registry->check<CompositingLayers>()) {
            layers = registry->get<CompositingLayers>().layers;
        } else {
            layers = {};
        }

        registry->set<CompositingLayers>({});

        auto pass = builder.addPass("CompositingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        builder.readWrite(pass, defaultDepthTexture);
        builder.readWrite(pass, vertexBuffer);
        builder.read(pass, pipeline);

        builder.write(pass, backBufferColor);
        builder.write(pass, backBufferDepth);

        for (auto &layer: layers) {
            builder.read(pass, layer.color);
            if (layer.depth) {
                builder.read(pass, layer.depth);
            }
        }
    }

    void CompositingPass::recreate(RenderGraphBuilder &builder) {
        defaultDepthTexture = builder.inheritResource(defaultDepthTexture);

        pipeline = builder.inheritResource(pipeline);
        vertexBuffer = builder.inheritResource(vertexBuffer);

        backBufferColor = builder.getBackBufferColor();
        backBufferDepth = builder.getBackBufferDepthStencil();

        if (registry->check<CompositingLayers>()) {
            layers = registry->get<CompositingLayers>().layers;
        } else {
            layers = {};
        }

        registry->set<CompositingLayers>({});

        auto pass = builder.addPass("CompositingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        builder.readWrite(pass, defaultDepthTexture);
        builder.read(pass, pipeline);
        builder.readWrite(pass, vertexBuffer);

        builder.write(pass, backBufferColor);
        builder.write(pass, backBufferDepth);

        for (auto &layer: layers) {
            builder.read(pass, layer.color);
            if (layer.depth) {
                builder.read(pass, layer.depth);
            }
        }
    }

    bool CompositingPass::shouldRebuild(const Vec2i &backBufferSize) {
        return false;
    }

    void CompositingPass::runPass(RenderGraphContext &ctx) {
        ctx.clearTextureColor(backBufferColor, ColorRGBA::black(1, 0));
        ctx.clearTextureDepthStencil(backBufferDepth, 1, 0);

        if (!vertexBufferAllocated) {
            vertexBufferAllocated = true;
            VertexStream stream;
            stream.addVertices(normalizedQuad.vertices);
            auto data = stream.getVertexBuffer();
            ctx.uploadBuffer(vertexBuffer, data.data(), data.size(), 0);
        }

        if (!defaultDepthTextureAllocated) {
            ctx.clearTextureDepthStencil(defaultDepthTexture, 0, 0);
        }

        ctx.beginRenderPass({RenderGraphAttachment(backBufferColor)}, RenderGraphAttachment(backBufferDepth));
        ctx.bindPipeline(pipeline);
        ctx.bindVertexBuffer(vertexBuffer);
        for (auto &layer: layers) {
            ctx.bindTexture("layerColor", layer.color);
            ctx.bindTexture("layerDepth", layer.depth ? layer.depth : defaultDepthTexture);
            ctx.drawArray(DrawCall(0, normalizedQuad.vertices.size()));
        }
        ctx.endRenderPass();
    }
}
