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

#ifndef XENGINE_FORWARDLIGHTINGPASS_HPP
#define XENGINE_FORWARDLIGHTINGPASS_HPP

#include "xng/graphics/3d/meshatlas/meshatlas.hpp"
#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/renderpass.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/textureatlas/textureatlas.hpp"
#include "xng/graphics/3d/sharedresources/compositinglayers.hpp"
#include "xng/graphics/3d/sharedresources/shadowmaps.hpp"

namespace xng {
    class XENGINE_EXPORT ForwardLightingPass final : public RenderPass {
    public:
        ForwardLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                            std::shared_ptr<SharedResourceRegistry> registry);

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        static Shader createVertexShader();

        static Shader createFragmentShader();

        void runPass(RenderGraphContext &ctx);

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        RenderGraphResource pipeline;

        RenderGraphResource shaderBuffer;
        RenderGraphResource boneBuffer;

        TextureAtlas textureAtlas;
        MeshAtlas meshAtlas;

        std::map<Uri, TextureAtlasHandle> textures;
        std::vector<ResourceHandle<SkinnedMesh> > allocatedMeshes;

        size_t currentBoneBufferSize{};
        size_t totalBoneBufferSize{};

        Vec2i currentResolution;

        Transform cameraTransform;
        Camera camera;
        std::vector<SkinnedMeshObject> objects;

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

        ShadowMaps shadowMaps;
    };
}

#endif //XENGINE_FORWARDLIGHTINGPASS_HPP
