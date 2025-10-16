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

namespace xng {
    //TODO: Move all type definitions and inline function definitions inside all of the translation units into anonymous namespaces to force "internal linkage" (https://en.cppreference.com/w/cpp/language/namespace.html).
    namespace {
#pragma pack(push, 1)
        struct PointLightData {
            std::array<float, 4> position;
            std::array<float, 4> color;
            std::array<float, 4> farPlane;
        };

        struct DirectionalLightData {
            std::array<float, 4> direction;
            std::array<float, 4> color;
            std::array<float, 4> farPlane;
        };

        struct SpotLightData {
            std::array<float, 4> position;
            std::array<float, 4> direction_quadratic;
            std::array<float, 4> color;
            std::array<float, 4> farPlane;
            std::array<float, 4> cutOff_outerCutOff_constant_linear;
        };

        struct ShaderAtlasTexture {
            int level_index_filtering_assigned[4]{0, 0, 0, 0};
            float atlasScale_texSize[4]{0, 0, 0, 0};
        };

        struct ShaderDrawData {
            Mat4f model;
            Mat4f mvp;

            int objectID_boneOffset_shadows[4]{0, 0, 0, 0};

            float metallic_roughness_ambientOcclusion[4]{0, 0, 0, 0};
            float albedoColor[4]{0, 0, 0, 0};

            ShaderAtlasTexture metallic;
            ShaderAtlasTexture roughness;
            ShaderAtlasTexture ambientOcclusion;
            ShaderAtlasTexture albedo;

            float viewPosition_gamma[4]{0, 0, 0, 0};

            ShaderAtlasTexture normal;
            float normalIntensity[4]{0, 0, 0, 0};
        };
#pragma pack(pop)

        static_assert(sizeof(ShaderDrawData) == 64 + 64 + 16 + 16 + 16 + 16 + 32 + 32 + 32 + 32 + 32 + 16);
    }

    ForwardLightingPass::ForwardLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                                             std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(configuration)), registry(std::move(registry)) {
    }

    void ForwardLightingPass::create(RenderGraphBuilder &builder) {
        assert(sizeof(ShaderDrawData) == 64 + 64 + 16 + 16 + 16 + 16 + 32 + 32 + 32 + 32 + 32 + 16);

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

        shaderBuffer = builder.createShaderBuffer(sizeof(ShaderDrawData));
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

        auto pass = builder.addPass("ForwardLightingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, pipeline);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    void ForwardLightingPass::recreate(RenderGraphBuilder &builder) {
        pipeline = builder.inheritResource(pipeline);

        const auto resolution = builder.getBackBufferSize() * config->getRenderScale();

        if (currentResolution != resolution) {
            currentResolution = resolution;

            auto desc = RenderGraphTexture();
            desc.size = currentResolution;
            desc.format = RGBA;
            layer.color = builder.createTexture(desc);

            desc.format = DEPTH;
            layer.depth = builder.createTexture(desc);
        } else {
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

        if (recreateLightBuffers) {
            pointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData)
                                                          * pointLights.size());
            directionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData)
                                                                * directionalLights.size());
            spotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData)
                                                         * spotLights.size());
            shadowPointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData)
                                                                * shadowPointLights.size());
            shadowDirectionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData)
                                                                      * shadowDirectionalLights.size());
            shadowSpotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData)
                                                               * shadowSpotLights.size());
            shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f)
                                                                               * shadowDirectionalLights.size());
            shadowSpotLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f) * shadowSpotLights.size());
        } else {
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

        auto pass = builder.addPass("ForwardLightingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, pipeline);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    bool ForwardLightingPass::shouldRebuild(const Vec2i &backBufferSize) {
        std::vector<PointLightObject> pLights;
        std::vector<PointLightObject> psLights;
        for (auto &object: config->getScene().pointLights) {
            if (object.light.castShadows) {
                psLights.emplace_back(object);
            } else {
                pLights.emplace_back(object);
            }
        }

        std::vector<DirectionalLightObject> dLights;
        std::vector<DirectionalLightObject> dsLights;
        for (auto &object: config->getScene().directionalLights) {
            if (object.light.castShadows) {
                dsLights.emplace_back(object);
            } else {
                dLights.emplace_back(object);
            }
        }

        std::vector<SpotLightObject> sLights;
        std::vector<SpotLightObject> ssLights;
        for (auto &object: config->getScene().spotLights) {
            if (object.light.castShadows) {
                ssLights.emplace_back(object);
            } else {
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

        auto &scene = config->getScene();

        std::set<Uri> usedTextures;
        std::vector<ResourceHandle<SkinnedModel> > usedMeshes;
        std::set<Uri> usedMeshUris;

        objects.clear();
        auto tmp = scene.skinnedModels;
        for (auto &object: tmp) {
            if (!object.model.assigned()) {
                continue;
            }
            meshAtlas.allocateMesh(object.model);
            usedMeshes.emplace_back(object.model);
            usedMeshUris.insert(object.model.getUri());
            objects.emplace_back(object);
            for (auto i = 0; i < object.model.get().subMeshes.size(); i++) {
                const auto &subMesh = object.model.get().subMeshes.at(i);

                Material mat = subMesh.material.get();

                auto mi = object.materials.find(i);
                if (mi != object.materials.end()) {
                    mat = mi->second;
                }

                if (!mat.transparent) {
                    continue;
                }

                totalBoneBufferSize += subMesh.bones.size() * sizeof(Mat4f);

                if (mat.normal.assigned()) {
                    if (textures.find(mat.normal.getUri()) == textures.end()) {
                        textures[mat.normal.getUri()] = textureAtlas.add(mat.normal.get().image.get());
                    }
                    usedTextures.insert(mat.normal.getUri());
                }
                if (mat.metallicTexture.assigned()) {
                    if (textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                        textures[mat.metallicTexture.getUri()] = textureAtlas.add(
                            mat.metallicTexture.get().image.get());
                    }
                    usedTextures.insert(mat.metallicTexture.getUri());
                }
                if (mat.roughnessTexture.assigned()) {
                    if (textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                        textures[mat.roughnessTexture.getUri()] = textureAtlas.add(
                            mat.roughnessTexture.get().image.get());
                    }
                    usedTextures.insert(mat.roughnessTexture.getUri());
                }
                if (mat.ambientOcclusionTexture.assigned()) {
                    if (textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                        textures[mat.ambientOcclusionTexture.getUri()] = textureAtlas.add(
                            mat.ambientOcclusionTexture.get().image.get());
                    }
                    usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                }
                if (mat.albedoTexture.assigned()) {
                    if (textures.find(mat.albedoTexture.getUri()) == textures.end()) {
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
        for (auto &mesh: allocatedMeshes) {
            if (usedMeshUris.find(mesh.getUri()) == usedMeshUris.end()) {
                meshAtlas.deallocateMesh(mesh);
            }
        }
        allocatedMeshes = usedMeshes;

        // Deallocate unused textures
        std::set<Uri> dealloc;
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            textureAtlas.remove(textures.at(uri));
            textures.erase(uri);
        }

        if (recreateLightBuffers) {
            return true;
        }

        if (layerSize != backBufferSize * config->getRenderScale()) {
            return true;
        }

        if (textureAtlas.shouldRebuild()) {
            return true;
        }

        if (meshAtlas.shouldRebuild()) {
            return true;
        }

        return currentBoneBufferSize < totalBoneBufferSize;
    }

    void ForwardLightingPass::runPass(RenderGraphContext &ctx) {
        if (recreateLightBuffers) {
            std::vector<PointLightData> pointLightData;
            for (auto &lightObject: pointLights) {
                PointLightData data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(pointLightBuffer,
                             reinterpret_cast<const uint8_t *>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData),
                             0);

            pointLightData.clear();
            for (auto &lightObject: shadowPointLights) {
                PointLightData data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(shadowPointLightBuffer,
                             reinterpret_cast<const uint8_t *>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData),
                             0);

            std::vector<DirectionalLightData> directionalLightData;
            for (auto &lightObject: directionalLights) {
                DirectionalLightData data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                directionalLightData.emplace_back(data);
            }
            ctx.uploadBuffer(directionalLightBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData),
                             0);

            directionalLightData.clear();

            std::vector<Mat4f> directionalLightTransforms;
            for (auto &lightObject: shadowDirectionalLights) {
                DirectionalLightData data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                directionalLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowDirectionalLightBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData),
                             0);
            ctx.uploadBuffer(shadowDirectionalLightTransformBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);


            std::vector<SpotLightData> spotLightData;
            for (auto &lightObject: spotLights) {
                SpotLightData data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0).getMemory();

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear).getMemory();
                spotLightData.emplace_back(data);
            }
            ctx.uploadBuffer(spotLightBuffer,
                             reinterpret_cast<const uint8_t *>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData),
                             0);

            spotLightData.clear();
            directionalLightTransforms.clear();

            for (auto &lightObject: shadowSpotLights) {
                SpotLightData data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0).getMemory();

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear).getMemory();
                spotLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowSpotLightBuffer,
                             reinterpret_cast<const uint8_t *>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData),
                             0);
            ctx.uploadBuffer(shadowSpotLightTransformBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);
            recreateLightBuffers = false;
        }

        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        std::vector<DrawCall> drawCalls{};
        std::vector<size_t> baseVertices{};
        std::vector<ShaderDrawData> shaderData{};
        std::vector<Mat4f> boneMatrices{};

        auto &meshAllocations = meshAtlas.getMeshAllocations(ctx);

        for (auto objectIndex = 0; objectIndex < objects.size(); objectIndex++) {
            auto &object = objects.at(objectIndex);

            const auto &rig = object.model.get().rig;
            const auto &boneTransforms = object.boneTransforms;
            const auto &materials = object.materials;
            const auto &drawData = meshAllocations.at(object.model.getUri());

            for (auto i = 0; i < object.model.get().subMeshes.size(); i++) {
                auto model = object.transform.model();

                const auto &mesh = object.model.get().subMeshes.at(i);

                auto material = mesh.material.get();

                auto mIt = materials.find(i);
                if (mIt != materials.end()) {
                    material = mIt->second;
                }

                if (!material.transparent) {
                    continue;
                }

                auto boneOffset = boneMatrices.size();
                if (mesh.bones.empty()) {
                    boneOffset = -1;
                } else {
                    for (auto &bone: mesh.bones) {
                        auto boneTransform = boneTransforms.find(bone);
                        if (boneTransform != boneTransforms.end()) {
                            boneMatrices.emplace_back(boneTransform->second);
                        } else {
                            boneMatrices.emplace_back(MatrixMath::identity());
                        }
                    }
                }

                bool receiveShadows = object.receiveShadows;

                auto data = ShaderDrawData();

                data.model = model;
                data.mvp = projection * view * model;
                data.objectID_boneOffset_shadows[0] = objectIndex;
                data.objectID_boneOffset_shadows[1] = static_cast<int>(boneOffset);
                data.objectID_boneOffset_shadows[2] = receiveShadows;

                data.metallic_roughness_ambientOcclusion[0] = material.metallic;
                data.metallic_roughness_ambientOcclusion[1] = material.roughness;
                data.metallic_roughness_ambientOcclusion[2] = material.ambientOcclusion;

                auto col = material.albedo.divide().getMemory();
                data.albedoColor[0] = col[0];
                data.albedoColor[1] = col[1];
                data.albedoColor[2] = col[2];
                data.albedoColor[3] = col[3];

                data.normalIntensity[0] = material.normalIntensity;

                if (material.metallicTexture.assigned()) {
                    auto tex = textures.at(material.metallicTexture.getUri());

                    data.metallic.level_index_filtering_assigned[0] = tex.level;
                    data.metallic.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.metallic.level_index_filtering_assigned[2] =
                            material.metallicTexture.get().filter > Texture::NEAREST;
                    data.metallic.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.metallic.atlasScale_texSize[0] = atlasScale.x;
                    data.metallic.atlasScale_texSize[1] = atlasScale.y;
                    data.metallic.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.metallic.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                }

                if (material.roughnessTexture.assigned()) {
                    auto tex = textures.at(material.roughnessTexture.getUri());

                    data.roughness.level_index_filtering_assigned[0] = tex.level;
                    data.roughness.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.roughness.level_index_filtering_assigned[2] =
                            material.roughnessTexture.get().filter > Texture::NEAREST;
                    data.roughness.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.roughness.atlasScale_texSize[0] = atlasScale.x;
                    data.roughness.atlasScale_texSize[1] = atlasScale.y;
                    data.roughness.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.roughness.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                }

                if (material.ambientOcclusionTexture.assigned()) {
                    auto tex = textures.at(material.ambientOcclusionTexture.getUri());

                    data.ambientOcclusion.level_index_filtering_assigned[0] = tex.level;
                    data.ambientOcclusion.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.ambientOcclusion.level_index_filtering_assigned[2] =
                            material.ambientOcclusionTexture.get().filter > Texture::NEAREST;
                    data.ambientOcclusion.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.ambientOcclusion.atlasScale_texSize[0] = atlasScale.x;
                    data.ambientOcclusion.atlasScale_texSize[1] = atlasScale.y;
                    data.ambientOcclusion.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.ambientOcclusion.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                }

                if (material.albedoTexture.assigned()) {
                    auto tex = textures.at(material.albedoTexture.getUri());

                    data.albedo.level_index_filtering_assigned[0] = tex.level;
                    data.albedo.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.albedo.level_index_filtering_assigned[2] =
                            material.albedoTexture.get().filter > Texture::NEAREST;
                    data.albedo.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.albedo.atlasScale_texSize[0] = atlasScale.x;
                    data.albedo.atlasScale_texSize[1] = atlasScale.y;
                    data.albedo.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.albedo.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                }

                if (material.normal.assigned()) {
                    auto tex = textures.at(material.normal.getUri());

                    data.normal.level_index_filtering_assigned[0] = tex.level;
                    data.normal.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.normal.level_index_filtering_assigned[2] = material.normal.get().filter > Texture::NEAREST;
                    data.normal.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.normal.atlasScale_texSize[0] = atlasScale.x;
                    data.normal.atlasScale_texSize[1] = atlasScale.y;
                    data.normal.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.normal.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                }

                data.viewPosition_gamma[0] = viewPosition.x;
                data.viewPosition_gamma[1] = viewPosition.y;
                data.viewPosition_gamma[2] = viewPosition.z;
                data.viewPosition_gamma[3] = config->getGamma();

                shaderData.emplace_back(data);

                auto &draw = drawData.data.at(i);
                drawCalls.emplace_back(draw.drawCall);
                baseVertices.emplace_back(draw.baseVertex);
            }
        }

        auto &atlasTextures = textureAtlas.getAtlasTextures(ctx);

        ctx.beginRenderPass({
                                RenderGraphAttachment(layer.color, ColorRGBA(0, 0, 0, 0)),
                            },
                            RenderGraphAttachment(layer.depth, 1, 0));

        ctx.setViewport({}, currentResolution);

        ctx.uploadBuffer(boneBuffer,
                         reinterpret_cast<const uint8_t *>(boneMatrices.data()),
                         boneMatrices.size() * sizeof(Mat4f),
                         0);

        ctx.bindPipeline(pipeline);
        ctx.bindVertexBuffer(meshAtlas.getVertexBuffer());
        ctx.bindIndexBuffer(meshAtlas.getIndexBuffer());

        std::vector<RenderGraphResource> bindTextures;
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; ++i) {
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

        ctx.bindTexture("pointLightShadowMaps", shadowMaps.pointShadowMaps);
        ctx.bindTexture("directionalLightShadowMaps", shadowMaps.dirShadowMaps);
        ctx.bindTexture("spotLightShadowMaps", shadowMaps.spotShadowMaps);

        for (auto i = 0; i < shaderData.size(); ++i) {
            auto &data = shaderData.at(i);
            ctx.uploadBuffer(shaderBuffer,
                             reinterpret_cast<const uint8_t *>(&data),
                             sizeof(ShaderDrawData),
                             0);
            ctx.drawIndexed(drawCalls.at(i), baseVertices.at(i));
        }

        ctx.endRenderPass();
    }
}
