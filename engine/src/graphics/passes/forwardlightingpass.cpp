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

#include "xng/graphics/passes/forwardlightingpass.hpp"
#include "xng/graphics/sharedresources/ibl.hpp"

namespace xng
{
    ForwardLightingPass::ForwardLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                                             std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(configuration)), registry(std::move(registry))
    {
    }

    void ForwardLightingPass::create(RenderGraphBuilder& builder)
    {
        auto vs = createVertexShader();
        auto fs = createFragmentShader();

        RenderGraphPipeline pipelineDesc;
        pipelineDesc.enableFaceCulling = true;
        pipelineDesc.enableDepthTest = true;
        pipelineDesc.depthTestWrite = true;
        pipelineDesc.enableBlending = true;
        pipelineDesc.colorBlendSourceMode = RenderGraphPipeline::SRC_ALPHA;
        pipelineDesc.colorBlendDestinationMode = RenderGraphPipeline::ONE_MINUS_SRC_ALPHA;
        pipelineDesc.alphaBlendSourceMode = RenderGraphPipeline::ONE;
        pipelineDesc.alphaBlendDestinationMode = RenderGraphPipeline::ONE_MINUS_SRC_ALPHA;
        pipelineDesc.shaders = {vs, fs};

        pipeline = builder.createPipeline(pipelineDesc);

        currentResolution = builder.getBackBufferSize() * config->getRenderScale();

        layer.containsTransparency = true;

        auto desc = RenderGraphTexture();
        desc.size = currentResolution;
        desc.format = RGBA;
        layer.color = builder.createTexture(desc);

        desc.format = DEPTH;
        layer.depth = builder.createTexture(desc);

        auto layers = registry->getOrCreate<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        shaderBuffer = builder.createShaderBuffer(sizeof(BufferLayout::CPU));
        boneBuffer = builder.createShaderBuffer(0);

        textureAtlas.onCreate(builder);

        pointLightBuffer = builder.createShaderBuffer(0);
        directionalLightBuffer = builder.createShaderBuffer(0);
        spotLightBuffer = builder.createShaderBuffer(0);

        shadowPointLightBuffer = builder.createShaderBuffer(0);
        shadowDirectionalLightBuffer = builder.createShaderBuffer(0);
        shadowSpotLightBuffer = builder.createShaderBuffer(0);

        shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(0);
        shadowSpotLightTransformBuffer = builder.createShaderBuffer(0);

        shadowMaps = registry->get<ShadowMaps>();

        auto pass = builder.addPass("ForwardLightingPass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });

        // If IBL maps are available, declare them as read resources so they are produced before this pass
        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) builder.read(pass, ibl.irradiance);
            if (ibl.prefilter) builder.read(pass, ibl.prefilter);
            if (ibl.brdfLUT) builder.read(pass, ibl.brdfLUT);
        }

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, pipeline);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    void ForwardLightingPass::recreate(RenderGraphBuilder& builder)
    {
        pipeline = builder.inheritResource(pipeline);

        const auto resolution = builder.getBackBufferSize() * config->getRenderScale();

        if (currentResolution != resolution)
        {
            currentResolution = resolution;

            auto desc = RenderGraphTexture();
            desc.size = currentResolution;
            desc.format = RGBA;
            layer.color = builder.createTexture(desc);

            desc.format = DEPTH;
            layer.depth = builder.createTexture(desc);
        }
        else
        {
            layer.color = builder.inheritResource(layer.color);
            layer.depth = builder.inheritResource(layer.depth);
        }

        auto layers = registry->getOrCreate<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        shaderBuffer = builder.inheritResource(shaderBuffer);

        boneBuffer = builder.createShaderBuffer(totalBoneBufferSize);
        currentBoneBufferSize = totalBoneBufferSize;

        textureAtlas.onRecreate(builder);

        if (recreateLightBuffers)
        {
            pointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData::CPU)
                * pointLights.size());
            directionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData::CPU)
                * directionalLights.size());
            spotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData::CPU)
                * spotLights.size());
            shadowPointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData::CPU)
                * shadowPointLights.size());
            shadowDirectionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData::CPU)
                * shadowDirectionalLights.size());
            shadowSpotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData::CPU)
                * shadowSpotLights.size());
            shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f)
                * shadowDirectionalLights.size());
            shadowSpotLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f) * shadowSpotLights.size());
        }
        else
        {
            pointLightBuffer = builder.inheritResource(pointLightBuffer);
            directionalLightBuffer = builder.inheritResource(directionalLightBuffer);
            spotLightBuffer = builder.inheritResource(spotLightBuffer);
            shadowPointLightBuffer = builder.inheritResource(shadowPointLightBuffer);
            shadowDirectionalLightBuffer = builder.inheritResource(shadowDirectionalLightBuffer);
            shadowSpotLightBuffer = builder.inheritResource(shadowSpotLightBuffer);
            shadowDirectionalLightTransformBuffer = builder.inheritResource(shadowDirectionalLightTransformBuffer);
            shadowSpotLightTransformBuffer = builder.inheritResource(shadowSpotLightTransformBuffer);
        }

        shadowMaps = registry->get<ShadowMaps>();

        auto pass = builder.addPass("ForwardLightingPass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });

        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) builder.read(pass, ibl.irradiance);
            if (ibl.prefilter) builder.read(pass, ibl.prefilter);
            if (ibl.brdfLUT) builder.read(pass, ibl.brdfLUT);
        }

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, pipeline);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    bool ForwardLightingPass::shouldRebuild(const Vec2i& backBufferSize)
    {
        std::vector<PointLightObject> pLights;
        std::vector<PointLightObject> psLights;
        for (auto& object : config->getScene().pointLights)
        {
            if (object.light.castShadows)
            {
                psLights.emplace_back(object);
            }
            else
            {
                pLights.emplace_back(object);
            }
        }

        std::vector<DirectionalLightObject> dLights;
        std::vector<DirectionalLightObject> dsLights;
        for (auto& object : config->getScene().directionalLights)
        {
            if (object.light.castShadows)
            {
                dsLights.emplace_back(object);
            }
            else
            {
                dLights.emplace_back(object);
            }
        }

        std::vector<SpotLightObject> sLights;
        std::vector<SpotLightObject> ssLights;
        for (auto& object : config->getScene().spotLights)
        {
            if (object.light.castShadows)
            {
                ssLights.emplace_back(object);
            }
            else
            {
                sLights.emplace_back(object);
            }
        }

        recreateLightBuffers = pLights != pointLights
            || psLights != shadowPointLights
            || dLights != directionalLights
            || dsLights != shadowDirectionalLights
            || sLights != spotLights
            || ssLights != shadowSpotLights;

        pointLights = pLights;
        shadowPointLights = psLights;
        directionalLights = dLights;
        shadowDirectionalLights = dsLights;
        spotLights = sLights;
        shadowSpotLights = ssLights;

        viewPosition = config->getScene().cameraTransform.getPosition();

        totalBoneBufferSize = 0;

        auto& scene = config->getScene();

        std::set<Uri> usedTextures;
        std::vector<ResourceHandle<SkinnedModel>> usedMeshes;
        std::set<Uri> usedMeshUris;

        objects.clear();
        auto tmp = scene.skinnedModels;
        for (auto& object : tmp)
        {
            if (!object.model.assigned())
            {
                continue;
            }
            meshAtlas.allocateMesh(object.model);
            usedMeshes.emplace_back(object.model);
            usedMeshUris.insert(object.model.getUri());
            objects.emplace_back(object);
            for (auto i = 0; i < object.model.get().subMeshes.size(); i++)
            {
                const auto& subMesh = object.model.get().subMeshes.at(i);

                Material mat = subMesh.material.get();

                auto mi = object.materials.find(i);
                if (mi != object.materials.end())
                {
                    mat = mi->second;
                }

                if (!mat.transparent)
                {
                    continue;
                }

                totalBoneBufferSize += subMesh.bones.size() * sizeof(Mat4f);

                if (mat.normal.assigned())
                {
                    if (textures.find(mat.normal.getUri()) == textures.end())
                    {
                        textures[mat.normal.getUri()] = textureAtlas.add(mat.normal.get().image.get());
                    }
                    usedTextures.insert(mat.normal.getUri());
                }
                if (mat.metallicTexture.assigned())
                {
                    if (textures.find(mat.metallicTexture.getUri()) == textures.end())
                    {
                        textures[mat.metallicTexture.getUri()] = textureAtlas.add(
                            mat.metallicTexture.get().image.get());
                    }
                    usedTextures.insert(mat.metallicTexture.getUri());
                }
                if (mat.roughnessTexture.assigned())
                {
                    if (textures.find(mat.roughnessTexture.getUri()) == textures.end())
                    {
                        textures[mat.roughnessTexture.getUri()] = textureAtlas.add(
                            mat.roughnessTexture.get().image.get());
                    }
                    usedTextures.insert(mat.roughnessTexture.getUri());
                }
                if (mat.ambientOcclusionTexture.assigned())
                {
                    if (textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end())
                    {
                        textures[mat.ambientOcclusionTexture.getUri()] = textureAtlas.add(
                            mat.ambientOcclusionTexture.get().image.get());
                    }
                    usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                }
                if (mat.albedoTexture.assigned())
                {
                    if (textures.find(mat.albedoTexture.getUri()) == textures.end())
                    {
                        textures[mat.albedoTexture.getUri()] = textureAtlas.
                            add(mat.albedoTexture.get().image.get());
                    }
                    usedTextures.insert(mat.albedoTexture.getUri());
                }
            }
        }

        cameraTransform = scene.cameraTransform;
        camera = scene.camera;

        // Deallocate unused meshes
        for (auto& mesh : allocatedMeshes)
        {
            if (usedMeshUris.find(mesh.getUri()) == usedMeshUris.end())
            {
                meshAtlas.deallocateMesh(mesh);
            }
        }
        allocatedMeshes = usedMeshes;

        // Deallocate unused textures
        std::set<Uri> dealloc;
        for (auto& pair : textures)
        {
            if (usedTextures.find(pair.first) == usedTextures.end())
            {
                dealloc.insert(pair.first);
            }
        }
        for (auto& uri : dealloc)
        {
            textureAtlas.remove(textures.at(uri));
            textures.erase(uri);
        }

        if (recreateLightBuffers)
        {
            return true;
        }

        if (layerSize != backBufferSize * config->getRenderScale())
        {
            return true;
        }

        if (textureAtlas.shouldRebuild())
        {
            return true;
        }

        if (meshAtlas.shouldRebuild())
        {
            return true;
        }

        return currentBoneBufferSize < totalBoneBufferSize;
    }

    void ForwardLightingPass::runPass(RenderGraphContext& ctx)
    {
        if (recreateLightBuffers)
        {
            std::vector<PointLightData::CPU> pointLightData;
            for (auto& lightObject : pointLights)
            {
                PointLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(pointLightBuffer,
                             reinterpret_cast<const uint8_t*>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData::CPU),
                             0);

            pointLightData.clear();
            for (auto& lightObject : shadowPointLights)
            {
                PointLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(shadowPointLightBuffer,
                             reinterpret_cast<const uint8_t*>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData::CPU),
                             0);

            std::vector<DirectionalLightData::CPU> directionalLightData;
            for (auto& lightObject : directionalLights)
            {
                DirectionalLightData::CPU data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                directionalLightData.emplace_back(data);
            }
            ctx.uploadBuffer(directionalLightBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData::CPU),
                             0);

            directionalLightData.clear();

            std::vector<Mat4f> directionalLightTransforms;
            for (auto& lightObject : shadowDirectionalLights)
            {
                DirectionalLightData::CPU data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                directionalLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowDirectionalLightBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData::CPU),
                             0);
            ctx.uploadBuffer(shadowDirectionalLightTransformBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);


            std::vector<SpotLightData::CPU> spotLightData;
            for (auto& lightObject : spotLights)
            {
                SpotLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear);
                spotLightData.emplace_back(data);
            }
            ctx.uploadBuffer(spotLightBuffer,
                             reinterpret_cast<const uint8_t*>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData::CPU),
                             0);

            spotLightData.clear();
            directionalLightTransforms.clear();

            for (auto& lightObject : shadowSpotLights)
            {
                SpotLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear);
                spotLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowSpotLightBuffer,
                             reinterpret_cast<const uint8_t*>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData::CPU),
                             0);
            ctx.uploadBuffer(shadowSpotLightTransformBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);
            recreateLightBuffers = false;
        }

        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        std::vector<DrawCall> drawCalls{};
        std::vector<size_t> baseVertices{};
        std::vector<BufferLayout::CPU> shaderData{};
        std::vector<Mat4f> boneMatrices{};

        auto& meshAllocations = meshAtlas.getMeshAllocations(ctx);

        for (auto objectIndex = 0; objectIndex < objects.size(); objectIndex++)
        {
            auto& object = objects.at(objectIndex);

            const auto& rig = object.model.get().rig;
            const auto& boneTransforms = object.boneTransforms;
            const auto& materials = object.materials;
            const auto& drawData = meshAllocations.at(object.model.getUri());

            for (auto i = 0; i < object.model.get().subMeshes.size(); i++)
            {
                auto model = object.transform.model();

                const auto& mesh = object.model.get().subMeshes.at(i);

                auto material = mesh.material.get();

                auto mIt = materials.find(i);
                if (mIt != materials.end())
                {
                    material = mIt->second;
                }

                if (!material.transparent)
                {
                    continue;
                }

                auto boneOffset = boneMatrices.size();
                if (mesh.bones.empty())
                {
                    boneOffset = -1;
                }
                else
                {
                    for (auto& bone : mesh.bones)
                    {
                        auto boneTransform = boneTransforms.find(bone);
                        if (boneTransform != boneTransforms.end())
                        {
                            boneMatrices.emplace_back(boneTransform->second);
                        }
                        else
                        {
                            boneMatrices.emplace_back(MatrixMath::identity());
                        }
                    }
                }

                bool receiveShadows = object.receiveShadows;

                auto data = BufferLayout::CPU();

                data.model = model;
                data.mvp = projection * view * model;
                data.objectID_boneOffset_shadows = Vec4i(objectIndex, static_cast<int>(boneOffset), receiveShadows, 0);
                data.metallic_roughness_ambientOcclusion = Vec4f(material.metallic, material.roughness,
                                                                 material.ambientOcclusion, 0);
                data.albedoColor = material.albedo.divide();
                data.normalIntensity = Vec4f(material.normalIntensity, 0, 0, 0);

                if (material.metallicTexture.assigned())
                {
                    auto tex = textures.at(material.metallicTexture.getUri());

                    data.metallic.level_index_filtering_assigned = Vec4i(tex.level,
                                                                         static_cast<int>(tex.index),
                                                                         material.metallicTexture.get().filter >
                                                                         Texture::NEAREST,
                                                                         1);

                    auto atlasScale = tex.size.convert<float>()
                        / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.metallic.atlasScale_texSize = Vec4f(atlasScale.x, atlasScale.y, static_cast<float>(tex.size.x),
                                                             static_cast<float>(tex.size.y));
                }

                if (material.roughnessTexture.assigned())
                {
                    auto tex = textures.at(material.roughnessTexture.getUri());

                    data.roughness.level_index_filtering_assigned = Vec4i(tex.level,
                                                                          static_cast<int>(tex.index),
                                                                          material.roughnessTexture.get().filter >
                                                                          Texture::NEAREST,
                                                                          1);

                    auto atlasScale = tex.size.convert<float>()
                        / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.roughness.atlasScale_texSize = Vec4f(atlasScale.x, atlasScale.y,
                                                              static_cast<float>(tex.size.x),
                                                              static_cast<float>(tex.size.y));
                }

                if (material.ambientOcclusionTexture.assigned())
                {
                    auto tex = textures.at(material.ambientOcclusionTexture.getUri());

                    data.ambientOcclusion.level_index_filtering_assigned = Vec4i(tex.level,
                        static_cast<int>(tex.index),
                        material.ambientOcclusionTexture.get().filter > Texture::NEAREST,
                        1);

                    auto atlasScale = tex.size.convert<float>()
                        / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.ambientOcclusion.atlasScale_texSize = Vec4f(atlasScale.x,
                                                                     atlasScale.y,
                                                                     static_cast<float>(tex.size.x),
                                                                     static_cast<float>(tex.size.y));
                }

                if (material.albedoTexture.assigned())
                {
                    auto tex = textures.at(material.albedoTexture.getUri());

                    data.albedo.level_index_filtering_assigned = Vec4i(tex.level,
                                                                       static_cast<int>(tex.index),
                                                                       material.albedoTexture.get().filter >
                                                                       Texture::NEAREST,
                                                                       1);

                    auto atlasScale = tex.size.convert<float>()
                        / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.albedo.atlasScale_texSize = Vec4f(atlasScale.x,
                                                           atlasScale.y,
                                                           static_cast<float>(tex.size.x),
                                                           static_cast<float>(tex.size.y));
                }

                if (material.normal.assigned())
                {
                    auto tex = textures.at(material.normal.getUri());

                    data.normal.level_index_filtering_assigned = Vec4i(tex.level,
                                                                       static_cast<int>(tex.index),
                                                                       material.normal.get().filter > Texture::NEAREST,
                                                                       1);

                    auto atlasScale = tex.size.convert<float>()
                        / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.normal.atlasScale_texSize = Vec4f(atlasScale.x,
                                                           atlasScale.y,
                                                           static_cast<float>(tex.size.x),
                                                           static_cast<float>(tex.size.y));
                }

                data.viewPosition_gamma = Vec4f(viewPosition.x, viewPosition.y, viewPosition.z, config->getGamma());

                if (registry->check<IBLMaps>())
                {
                    data.iblPresent_prefilterMipCount = Vec4i(true,
                                                              RenderGraphTexture::calculateMipLevels(
                                                                  config->iblPrefilterSize),
                                                              0,
                                                              0);
                }
                else
                {
                    data.iblPresent_prefilterMipCount = Vec4i(false, 0, 0, 0);
                }

                shaderData.emplace_back(data);

                auto& draw = drawData.data.at(i);
                drawCalls.emplace_back(draw.drawCall);
                baseVertices.emplace_back(draw.baseVertex);
            }
        }

        auto& atlasTextures = textureAtlas.getAtlasTextures(ctx);

        ctx.beginRenderPass({
                                RenderGraphAttachment(layer.color, ColorRGBA(0, 0, 0, 0)),
                            },
                            RenderGraphAttachment(layer.depth, 1, 0));

        ctx.setViewport({}, currentResolution);

        ctx.uploadBuffer(boneBuffer,
                         reinterpret_cast<const uint8_t*>(boneMatrices.data()),
                         boneMatrices.size() * sizeof(Mat4f),
                         0);

        ctx.bindPipeline(pipeline);
        ctx.bindVertexBuffer(meshAtlas.getVertexBuffer());
        ctx.bindIndexBuffer(meshAtlas.getIndexBuffer());

        std::vector<RenderGraphResource> bindTextures;
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; ++i)
        {
            bindTextures.emplace_back(atlasTextures.at(static_cast<TextureAtlasResolution>(i)));
        }
        ctx.bindTexture("atlasTextures", bindTextures);

        ctx.bindShaderBuffer("data", shaderBuffer);
        ctx.bindShaderBuffer("bones", boneBuffer);

        ctx.bindShaderBuffer("pointLights", pointLightBuffer);
        ctx.bindShaderBuffer("directionalLights", directionalLightBuffer);
        ctx.bindShaderBuffer("spotLights", spotLightBuffer);
        ctx.bindShaderBuffer("shadowPointLights", shadowPointLightBuffer);
        ctx.bindShaderBuffer("shadowDirectionalLights", shadowDirectionalLightBuffer);
        ctx.bindShaderBuffer("shadowSpotLights", shadowSpotLightBuffer);
        ctx.bindShaderBuffer("directionalLightShadowTransforms", shadowDirectionalLightTransformBuffer);
        ctx.bindShaderBuffer("spotLightShadowTransforms", shadowSpotLightTransformBuffer);

        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) ctx.bindTexture("iblIrradiance", ibl.irradiance);
            if (ibl.prefilter) ctx.bindTexture("iblPrefilter", ibl.prefilter);
            if (ibl.brdfLUT) ctx.bindTexture("iblBRDF", ibl.brdfLUT);
        }

        ctx.bindTexture("pointLightShadowMaps", shadowMaps.pointShadowMaps);
        ctx.bindTexture("directionalLightShadowMaps", shadowMaps.dirShadowMaps);
        ctx.bindTexture("spotLightShadowMaps", shadowMaps.spotShadowMaps);

        for (auto i = 0; i < shaderData.size(); ++i)
        {
            auto& data = shaderData.at(i);
            ctx.uploadBuffer(shaderBuffer,
                             reinterpret_cast<const uint8_t*>(&data),
                             sizeof(BufferLayout::CPU),
                             0);
            ctx.drawIndexed(drawCalls.at(i), baseVertices.at(i));
        }

        ctx.endRenderPass();
    }
}
