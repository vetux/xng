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

#ifndef XENGINE_SHADOWMAPPINGPASS_HPP
#define XENGINE_SHADOWMAPPINGPASS_HPP

#include "xng/graphics/3d/meshatlas/meshatlas.hpp"
#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/renderpass.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/sharedresources/shadowmaps.hpp"

namespace xng {
    class XENGINE_EXPORT ShadowMappingPass final : public RenderPass {
    public:
        ShadowMappingPass(std::shared_ptr<RenderConfiguration> config,
                          std::shared_ptr<SharedResourceRegistry> registry);

        ~ShadowMappingPass() override = default;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        static Shader createVertexShader();

        static Shader createGeometryShader();

        static Shader createFragmentShader();

        static Shader createDirVertexShader();

        static Shader createDirGeometryShader();

        static Shader createDirFragmentShader();

        void runPass(RenderGraphContext &ctx);

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        MeshAtlas meshAtlas;

        RenderGraphResource pointPipeline;
        RenderGraphResource dirPipeline;

        ShadowMaps shadowMaps;

        Vec2i pointShadowMapResolution;
        Vec2i dirShadowMapResolution;
        Vec2i spotShadowMapResolution;

        size_t pointShadowMapCount{};
        size_t dirShadowMapCount{};
        size_t spotShadowMapCount{};

        RenderGraphResource shaderBuffer;
        RenderGraphResource boneBuffer;
        RenderGraphResource pointLightBuffer;
        RenderGraphResource dirLightBuffer;

        size_t currentBoneBufferSize{};
        size_t requiredBoneBufferSize{};

        std::vector<ResourceHandle<SkinnedMesh> > allocatedMeshes;

        std::vector<SkinnedMeshObject> meshObjects;
        std::vector<PointLightObject> pointLights;
        std::vector<DirectionalLightObject> dirLights;
        std::vector<SpotLightObject> spotLights;
    };
}

#endif //XENGINE_SHADOWMAPPINGPASS_HPP
