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

#ifndef XENGINE_DEFERREDLIGHTINGPASS_HPP
#define XENGINE_DEFERREDLIGHTINGPASS_HPP

#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/renderpass.hpp"
#include "xng/graphics/3d/sharedresources/compositinglayers.hpp"
#include "xng/graphics/3d/sharedresources/gbuffer.hpp"
#include "xng/graphics/3d/sharedresources/shadowmaps.hpp"
#include "xng/graphics/scene/mesh.hpp"

namespace xng {
    class XENGINE_EXPORT DeferredLightingPass : public RenderPass {
    public:
        DeferredLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                             std::shared_ptr<SharedResourceRegistry> registry);

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        static Shader createVertexShader();

        static Shader createFragmentShader();

        void runPass(RenderGraphContext &ctx);

        Mesh normalizedQuad = Mesh::normalizedQuad();
        bool normalizedQuadUploaded = false;

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        RenderGraphResource pipeline;

        RenderGraphResource vertexBuffer;

        RenderGraphResource shaderDataBuffer;

        RenderGraphResource pointLightBuffer;
        RenderGraphResource directionalLightBuffer;
        RenderGraphResource spotLightBuffer;

        std::vector<PointLightObject> pointLights;
        std::vector<DirectionalLightObject> directionalLights;
        std::vector<SpotLightObject> spotLights;

        RenderGraphResource shadowPointLightBuffer;
        RenderGraphResource shadowDirectionalLightBuffer;
        RenderGraphResource shadowSpotLightBuffer;

        std::vector<PointLightObject> shadowPointLights;
        std::vector<DirectionalLightObject> shadowDirectionalLights;
        std::vector<SpotLightObject> shadowSpotLights;

        RenderGraphResource shadowDirectionalLightTransformBuffer;
        RenderGraphResource shadowSpotLightTransformBuffer;

        bool recreateLightBuffers = false;

        Vec3f viewPosition;

        CompositeLayer layer;
        Vec2i layerSize;

        GBuffer gBuffer;
        ShadowMaps shadowMaps;
    };
}

#endif //XENGINE_DEFERREDLIGHTINGPASS_HPP
