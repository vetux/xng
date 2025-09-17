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

#ifndef XENGINE_CLEARPASS_HPP
#define XENGINE_CLEARPASS_HPP

#include "xng/render/renderpass.hpp"
#include "xng/render/renderconfiguration.hpp"
#include "xng/render/renderregistry.hpp"

namespace xng {
    class ClearPass final : public RenderPass {
    public:
        ClearPass(const std::shared_ptr<RenderConfiguration> &config,
                  const std::shared_ptr<RenderRegistry> &registry)
            : config(config),
              registry(registry) {
        }

        ~ClearPass() override = default;

        void create(RenderGraphBuilder &builder) override {
            auto resolution = config->getRenderResolution();

            currentResolution = resolution;

            RenderGraphTexture desc;
            desc.size = resolution;
            desc.format = RGBA;

            screenColor = builder.createTexture(desc);
            deferredColor = builder.createTexture(desc);
            forwardColor = builder.createTexture(desc);
            backgroundColor = builder.createTexture(desc);

            desc.format = DEPTH_STENCIL;

            screenDepth = builder.createTexture(desc);
            deferredDepth = builder.createTexture(desc);
            forwardDepth = builder.createTexture(desc);

            registry->setEntry(SCREEN_COLOR, screenColor);
            registry->setEntry(SCREEN_DEPTH, screenDepth);
            registry->setEntry(DEFERRED_COLOR, deferredColor);
            registry->setEntry(DEFERRED_DEPTH, deferredDepth);
            registry->setEntry(FORWARD_COLOR, forwardColor);
            registry->setEntry(FORWARD_DEPTH, forwardDepth);
            registry->setEntry(BACKGROUND_COLOR, backgroundColor);

            auto pass = builder.addPass("ClearPass", [this](RenderGraphContext &ctx) {
                ctx.clearTextureColor(screenColor, ColorRGBA::black(1, 0));
                ctx.clearTextureColor(deferredColor, ColorRGBA::black(1, 0));
                ctx.clearTextureColor(forwardColor, ColorRGBA::black(1, 0));
                ctx.clearTextureColor(backgroundColor, ColorRGBA::black(1, 0));
                ctx.clearTextureDepthStencil(screenDepth, 1, 0);
                ctx.clearTextureDepthStencil(deferredDepth, 1, 0);
                ctx.clearTextureDepthStencil(forwardDepth, 1, 0);
            });

            builder.write(pass, screenColor);
            builder.write(pass, deferredColor);
            builder.write(pass, forwardColor);
            builder.write(pass, backgroundColor);
            builder.write(pass, screenDepth);
            builder.write(pass, deferredDepth);
            builder.write(pass, forwardDepth);
        }

        void recreate(RenderGraphBuilder &builder) override {
            create(builder);
        }

        bool shouldRebuild() override {
            return config->getRenderResolution() != currentResolution;
        }

    private:
        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<RenderRegistry> registry;

        Vec2i currentResolution;

        RenderGraphResource screenColor;
        RenderGraphResource screenDepth;
        RenderGraphResource deferredColor;
        RenderGraphResource deferredDepth;
        RenderGraphResource forwardColor;
        RenderGraphResource forwardDepth;
        RenderGraphResource backgroundColor;
    };
}

#endif //XENGINE_CLEARPASS_HPP
