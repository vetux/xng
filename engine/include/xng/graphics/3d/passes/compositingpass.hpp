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

#ifndef XENGINE_COMPOSITINGPASS_HPP
#define XENGINE_COMPOSITINGPASS_HPP

#include "xng/graphics/3d/renderpass.hpp"
#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/sharedresources/compositinglayers.hpp"

namespace xng {
    class CompositingPass final : public RenderPass {
    public:
        CompositingPass(std::shared_ptr<RenderConfiguration> config,
                        std::shared_ptr<SharedResourceRegistry> registry)
            : config(std::move(config)),
              registry(std::move(registry)) {
        }

        ~CompositingPass() override = default;

        void create(RenderGraphBuilder &builder) override {
            const auto resolution = builder.getBackBufferSize() * config->getRenderScale();

            currentResolution = resolution;

            backBufferColor = builder.getBackBufferColor();
            backBufferDepth = builder.getBackBufferDepthStencil();

            if (registry->check<CompositingLayers>()) {
                layers = registry->get<CompositingLayers>().layers;
            } else {
                layers = {};
            }

            auto pass = builder.addPass("CompositingPass", [this](RenderGraphContext &ctx) {
                ctx.clearTextureColor(backBufferColor, ColorRGBA::black(1, 0));
                ctx.clearTextureDepthStencil(backBufferDepth, 1, 0);
            });

            builder.write(pass, backBufferColor);
            builder.write(pass, backBufferDepth);

            for (auto &layer: layers) {
                builder.read(pass, layer.color);
                if (layer.depth) {
                    builder.read(pass, layer.depth);
                }
            }
        }

        void recreate(RenderGraphBuilder &builder) override {
            const auto resolution = builder.getBackBufferSize() * config->getRenderScale();

            backBufferColor = builder.getBackBufferColor();
            backBufferDepth = builder.getBackBufferDepthStencil();

            if (registry->check<CompositingLayers>()) {
                layers = registry->get<CompositingLayers>().layers;
            } else {
                layers = {};
            }

            auto pass = builder.addPass("CompositingPass", [this](RenderGraphContext &ctx) {
                ctx.clearTextureColor(backBufferColor, ColorRGBA::black(1, 0));
                ctx.clearTextureDepthStencil(backBufferDepth, 1, 0);
            });

            builder.write(pass, backBufferColor);
            builder.write(pass, backBufferDepth);

            for (auto &layer: layers) {
                builder.read(pass, layer.color);
                if (layer.depth) {
                    builder.read(pass, layer.depth);
                }
            }
        }

        bool shouldRebuild(const Vec2i &backBufferSize) override {
            return backBufferSize * config->getRenderScale() != currentResolution;
        }

    private:
        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        Vec2i currentResolution;

        std::vector<CompositeLayer> layers;

        RenderGraphResource backBufferColor;
        RenderGraphResource backBufferDepth;
    };
}

#endif //XENGINE_COMPOSITINGPASS_HPP
