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

#ifndef XENGINE_CANVASRENDERPASS_HPP
#define XENGINE_CANVASRENDERPASS_HPP

#include "xng/graphics/3d/renderpass.hpp"

#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/sharedresources/compositinglayers.hpp"
#include "xng/graphics/3d/sharedresources/rendercanvases.hpp"

namespace xng {
    /**
     * Renders the canvases created by the RenderPass2D.
     */
    class XENGINE_EXPORT CanvasRenderPass final : public RenderPass {
    public:
        CanvasRenderPass(std::shared_ptr<RenderConfiguration> config, std::shared_ptr<SharedResourceRegistry> registry);

        bool shouldRebuild(const Vec2i &backBufferSize) override;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

    private:
        void runPass(RenderGraphContext &ctx);

        static Shader createVertexShader();

        static Shader createFragmentShader();

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        CompositeLayer screenSpaceLayer;
        CompositeLayer worldSpaceLayer;
        Vec2i layerSize;

        RenderCanvases canvases;
    };
}

#endif //XENGINE_CANVASRENDERPASS_HPP
