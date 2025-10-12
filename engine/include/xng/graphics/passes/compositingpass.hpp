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

#ifndef XENGINE_COMPOSITINGPASS_HPP
#define XENGINE_COMPOSITINGPASS_HPP

#include "xng/graphics/renderpass.hpp"
#include "xng/graphics/renderconfiguration.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"
#include "xng/graphics/sharedresources/compositinglayers.hpp"

namespace xng {
    class XENGINE_EXPORT CompositingPass final : public RenderPass {
    public:
        CompositingPass(std::shared_ptr<RenderConfiguration> config,
                        std::shared_ptr<SharedResourceRegistry> registry);

        ~CompositingPass() override = default;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        static Shader createVertexShader();

        static Shader createFragmentShader();

        void runPass(RenderGraphContext &ctx);

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        std::vector<CompositeLayer> layers;

        RenderGraphResource backBufferColor;
        RenderGraphResource backBufferDepth;

        RenderGraphResource pipeline;

        RenderGraphResource vertexBuffer;
        bool vertexBufferAllocated = false;

        RenderGraphResource defaultDepthTexture;
        bool defaultDepthTextureAllocated = false;

        Mesh normalizedQuad;

        Vec2i backBufferSize;
    };
}

#endif //XENGINE_COMPOSITINGPASS_HPP
