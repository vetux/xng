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

#include "xng/graphics/meshatlas/meshatlas.hpp"
#include "xng/graphics/renderconfiguration.hpp"
#include "xng/graphics/renderpass.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"
#include "xng/graphics/textureatlas/textureatlas.hpp"
#include "xng/graphics/sharedresources/compositinglayers.hpp"
#include "xng/graphics/sharedresources/shadowmaps.hpp"
#include "xng/rendergraph/shaderscript/shaderstruct.hpp"

namespace xng
{
    class XENGINE_EXPORT ForwardLightingPass final : public RenderPass
    {
    public:
        ForwardLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                            std::shared_ptr<SharedResourceRegistry> registry);

        void create(RenderGraphBuilder& builder) override;

        void recreate(RenderGraphBuilder& builder) override;

        bool shouldRebuild(const Vec2i& backBufferSize) override;

    private:
        static Shader createVertexShader();

        static Shader createFragmentShader();

        void runPass(RenderGraphContext& ctx);

        ShaderStruct(AtlasTexture,
                     Vec4i, level_index_filtering_assigned,
                     Vec4f, atlasScale_texSize)

        ShaderStruct(BufferLayout,
                     Mat4f, model,
                     Mat4f, mvp,
                     Vec4i, objectID_boneOffset_shadows,
                     Vec4f, metallic_roughness_ambientOcclusion,
                     Vec4f, albedoColor,
                     AtlasTexture, metallic,
                     AtlasTexture, roughness,
                     AtlasTexture, ambientOcclusion,
                     AtlasTexture, albedo,
                     Vec4f, viewPosition_gamma,
                     AtlasTexture, normal,
                     Vec4f, normalIntensity,
                     Vec4i, iblPresent_prefilterMipCount)

        ShaderStruct(BoneBufferLayout, Mat4f, matrix)

        ShaderStruct(PointLightData,
                     Vec4f, position,
                     Vec4f, color,
                     Vec4f, farPlane)

        ShaderStruct(DirectionalLightData,
                     Vec4f, direction,
                     Vec4f, color,
                     Vec4f, farPlane)

        ShaderStruct(SpotLightData,
                     Vec4f, position,
                     Vec4f, direction_quadratic,
                     Vec4f, color,
                     Vec4f, farPlane,
                     Vec4f, cutOff_outerCutOff_constant_linear)

        ShaderStruct(TransformData, Mat4f, transform)

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        RenderGraphResource pipeline;

        RenderGraphResource shaderBuffer;
        RenderGraphResource boneBuffer;

        TextureAtlas textureAtlas;
        MeshAtlas meshAtlas;

        std::map<Uri, TextureAtlasHandle> textures;
        std::vector<ResourceHandle<SkinnedModel>> allocatedMeshes;

        size_t currentBoneBufferSize{};
        size_t totalBoneBufferSize{};

        Vec2i currentResolution;

        Transform cameraTransform;
        Camera camera;
        std::vector<SkinnedModelObject> objects;

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
