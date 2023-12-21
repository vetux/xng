/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "xng/render/graph/passes/forwardlightingpass.hpp"

#include "xng/render/graph/framegraphbuilder.hpp"

#include "xng/render/geometry/vertexstream.hpp"

#include "graph/forwardlightingpass_vs.hpp"
#include "graph/forwardlightingpass_fs.hpp"

namespace xng {
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

    struct ShaderViewData {
        std::array<float, 4> viewPosition;
        std::array<float, 4> viewSize;
    };

    struct ShaderDrawData {
        Mat4f model;
        Mat4f mvp;

        int objectID_shadows[4]{0, 0, 0, 0};

        float metallic_roughness_ambientOcclusion[4]{0, 0, 0, 0};
        float albedoColor[4]{0, 0, 0, 0};

        float normalIntensity[4]{0, 0, 0, 0};

        ShaderAtlasTexture normal;

        ShaderAtlasTexture metallic;
        ShaderAtlasTexture roughness;
        ShaderAtlasTexture ambientOcclusion;
        ShaderAtlasTexture albedo;
    };
#pragma pack(pop)

    static std::pair<std::vector<PointLightData>, std::vector<PointLightData>>
    getPointLights(const Scene &scene) {
        std::vector<PointLightData> pointLights;
        std::vector<PointLightData> shadowLights;
        for (auto &node: scene.rootNode.findAll({typeid(PointLightProperty)})) {
            auto l = node.getProperty<PointLightProperty>().light;
            auto t = node.getProperty<TransformProperty>().transform;
            auto v = l.color.divide();
            auto tmp = PointLightData{
                    .position =  Vec4f(t.getPosition().x,
                                       t.getPosition().y,
                                       t.getPosition().z,
                                       0).getMemory(),
                    .color = Vec4f(v.x * l.power, v.y * l.power, v.z * l.power, 1).getMemory(),
                    .farPlane = Vec4f(l.shadowFarPlane, 0, 0, 0).getMemory(),
            };
            if (l.castShadows)
                shadowLights.emplace_back(tmp);
            else
                pointLights.emplace_back(tmp);
        }
        return {pointLights, shadowLights};
    }

    static std::pair<std::vector<DirectionalLightData>, std::vector<DirectionalLightData>>
    getDirLights(const Scene &scene) {
        std::vector<DirectionalLightData> lights;
        std::vector<DirectionalLightData> shadowLights;
        for (auto &node: scene.rootNode.findAll({typeid(DirectionalLightProperty)})) {
            auto l = node.getProperty<DirectionalLightProperty>().light;
            auto v = l.color.divide();
            auto tmp = DirectionalLightData{
                    .direction =  Vec4f(l.direction.x,
                                        l.direction.y,
                                        l.direction.z,
                                        0).getMemory(),
                    .color = Vec4f(v.x * l.power, v.y * l.power, v.z * l.power, 1).getMemory(),
                    .farPlane = Vec4f(l.shadowFarPlane, 0, 0, 0).getMemory()
            };
            if (l.castShadows)
                shadowLights.emplace_back(tmp);
            else
                lights.emplace_back(tmp);
        }
        return {lights, shadowLights};
    }

    static float getCutOff(float angleDegrees) {
        return std::cos(degreesToRadians(angleDegrees));
    }

    static std::pair<std::vector<SpotLightData>, std::vector<SpotLightData>> getSpotLights(const Scene &scene) {
        std::vector<SpotLightData> lights;
        std::vector<SpotLightData> shadowLights;
        for (auto &node: scene.rootNode.findAll({typeid(SpotLightProperty)})) {
            auto l = node.getProperty<SpotLightProperty>().light;
            auto t = node.getProperty<TransformProperty>().transform;
            auto v = l.color.divide();
            auto tmp = SpotLightData{
                    .position =  Vec4f(t.getPosition().x,
                                       t.getPosition().y,
                                       t.getPosition().z,
                                       0).getMemory(),
                    .direction_quadratic =  Vec4f(l.direction.x,
                                                  l.direction.y,
                                                  l.direction.z,
                                                  l.quadratic).getMemory(),
                    .color = Vec4f(v.x * l.power, v.y * l.power, v.z * l.power, 1).getMemory(),
                    .farPlane = Vec4f(l.shadowFarPlane, 0, 0, 0).getMemory(),
                    .cutOff_outerCutOff_constant_linear = Vec4f(getCutOff(l.cutOff),
                                                                getCutOff(l.outerCutOff),
                                                                l.constant,
                                                                l.linear).getMemory()
            };
            if (l.castShadows)
                shadowLights.emplace_back(tmp);
            else
                lights.emplace_back(tmp);
        }
        return {lights, shadowLights};
    }

    void ForwardLightingPass::setup(FrameGraphBuilder &builder) {
        auto resolution = builder.getRenderResolution();
        auto scene = builder.getScene();

        auto pointLightNodes = scene.rootNode.findAll({typeid(PointLightProperty)});

        size_t pointLightCount = 0;
        size_t shadowPointLightCount = 0;

        for (auto l: pointLightNodes) {
            if (l.getProperty<PointLightProperty>().light.castShadows)
                shadowPointLightCount++;
            else
                pointLightCount++;
        }

        auto dirLightNodes = scene.rootNode.findAll({typeid(DirectionalLightProperty)});

        size_t dirLightCount = 0;
        size_t shadowDirLightCount = 0;

        for (auto l: dirLightNodes) {
            if (l.getProperty<DirectionalLightProperty>().light.castShadows)
                shadowDirLightCount++;
            else
                dirLightCount++;
        }

        auto spotLightNodes = scene.rootNode.findAll({typeid(SpotLightProperty)});

        size_t spotLightCount = 0;
        size_t shadowSpotLightCount = 0;

        for (auto l: spotLightNodes) {
            if (l.getProperty<SpotLightProperty>().light.castShadows)
                shadowSpotLightCount++;
            else
                spotLightCount++;
        }

        auto pointLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * pointLightCount
        });

        auto shadowPointLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * shadowPointLightCount
        });

        auto dirLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * dirLightCount
        });

        auto shadowDirLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * shadowDirLightCount
        });

        auto spotLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * spotLightCount
        });

        auto shadowSpotLightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * shadowSpotLightCount
        });

        if (!pipeline.assigned) {
            pipeline = builder.createRenderPipeline(RenderPipelineDesc{
                    .shaders = {{VERTEX,   forwardlightingpass_vs},
                                {FRAGMENT, forwardlightingpass_fs}},
                    .bindings = {
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_TEXTURE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                    },
                    .vertexLayout = SkinnedMesh::getDefaultVertexLayout(),
                    .enableDepthTest = true,
                    .depthTestWrite = true,
                    .depthTestMode = DEPTH_TEST_LESS,
                    .enableFaceCulling = true,
                    .enableBlending = true,
                    // https://stackoverflow.com/a/16938711
                    .colorBlendSourceMode = SRC_ALPHA,
                    .colorBlendDestinationMode = ONE_MINUS_SRC_ALPHA,
                    .alphaBlendSourceMode = ONE,
                    .alphaBlendDestinationMode = ONE_MINUS_SRC_ALPHA
            });
        }

        builder.persist(pipeline);

        size_t totalShaderBufferSize = 0;

        std::vector<Node> nodes;
        std::set<Uri> usedTextures;
        std::set<Uri> usedMeshes;
        for (auto &node: scene.rootNode.findAll({typeid(SkinnedMeshProperty)})) {
            auto &meshProp = node.getProperty<SkinnedMeshProperty>();
            if (!meshProp.mesh.assigned()) {
                continue;
            }

            usedMeshes.insert(meshProp.mesh.getUri());
            meshAllocator.prepareMeshAllocation(meshProp.mesh);

            auto &mesh = meshProp.mesh.get();

            auto it = node.properties.find(typeid(MaterialProperty));
            MaterialProperty matProp;
            if (it != node.properties.end()) {
                matProp = it->second->get<MaterialProperty>();
            }

            bool gotMesh = false;
            for (auto i = 0; i < mesh.subMeshes.size() + 1; i++) {
                auto &cMesh = i <= 0 ? mesh : mesh.subMeshes.at(i - 1);

                Material mat;
                if (cMesh.material.assigned()) {
                    mat = cMesh.material.get();
                }

                auto mi = matProp.materials.find(i);
                if (mi != matProp.materials.end()) {
                    mat = mi->second.get();
                }

                if (!mat.transparent)
                    continue;

                gotMesh = true;

                usedTextures.insert(mat.normal.getUri());
                usedTextures.insert(mat.metallicTexture.getUri());
                usedTextures.insert(mat.roughnessTexture.getUri());
                usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                usedTextures.insert(mat.albedoTexture.getUri());

                if (mat.normal.assigned()
                    && textures.find(mat.normal.getUri()) == textures.end()) {
                    textures[mat.normal.getUri()] = atlas.add(mat.normal.get().image.get());
                }
                if (mat.metallicTexture.assigned()
                    && textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                    textures[mat.metallicTexture.getUri()] = atlas.add(
                            mat.metallicTexture.get().image.get());
                }
                if (mat.roughnessTexture.assigned()
                    && textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                    textures[mat.roughnessTexture.getUri()] = atlas.add(
                            mat.roughnessTexture.get().image.get());
                }
                if (mat.ambientOcclusionTexture.assigned()
                    && textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                    textures[mat.ambientOcclusionTexture.getUri()] = atlas.add(
                            mat.ambientOcclusionTexture.get().image.get());
                }
                if (mat.albedoTexture.assigned()
                    && textures.find(mat.albedoTexture.getUri()) == textures.end()) {
                    textures[mat.albedoTexture.getUri()] = atlas.add(
                            mat.albedoTexture.get().image.get());
                }

                totalShaderBufferSize += sizeof(ShaderDrawData);
            }

            if (gotMesh)
                nodes.emplace_back(node);
        }

        size_t maxBufferSize = builder.getDeviceInfo().storageBufferMaxSize;

        size_t drawCycles = 0;
        if (totalShaderBufferSize > maxBufferSize) {
            drawCycles = totalShaderBufferSize / maxBufferSize;
            auto remainder = totalShaderBufferSize % maxBufferSize;
            if (remainder > 0) {
                drawCycles += 1;
            }
        } else if (totalShaderBufferSize > 0) {
            drawCycles = 1;
        }

        size_t numberOfPassesPerCycle = maxBufferSize / sizeof(ShaderDrawData);

        auto bufSize = (sizeof(ShaderDrawData) * numberOfPassesPerCycle);
        if (bufSize > totalShaderBufferSize)
            bufSize = totalShaderBufferSize;

        auto shaderBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = bufSize
        });

        auto shaderViewBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = HOST_VISIBLE,
                .size = sizeof(float[12])
        });

        atlas.setup(builder);

        if (vertexBuffer.assigned) {
            builder.persist(vertexBuffer);
        }

        if (indexBuffer.assigned) {
            builder.persist(indexBuffer);
        }

        if (!vertexBuffer.assigned || currentVertexBufferSize < meshAllocator.getRequestedVertexBufferSize()) {
            auto staleVertexBuffer = vertexBuffer;
            auto d = VertexBufferDesc();
            d.size = meshAllocator.getRequestedVertexBufferSize();
            vertexBuffer = builder.createVertexBuffer(d);
            builder.persist(vertexBuffer);
            if (staleVertexBuffer.assigned)
                builder.copy(staleVertexBuffer, vertexBuffer, 0, 0, currentVertexBufferSize);
            currentVertexBufferSize = d.size;
        }

        if (!indexBuffer.assigned || currentIndexBufferSize < meshAllocator.getRequestedIndexBufferSize()) {
            auto staleIndexBuffer = indexBuffer;
            auto d = IndexBufferDesc();
            d.size = meshAllocator.getRequestedIndexBufferSize();
            indexBuffer = builder.createIndexBuffer(d);
            builder.persist(indexBuffer);
            if (staleIndexBuffer.assigned)
                builder.copy(staleIndexBuffer, indexBuffer, 0, 0, currentIndexBufferSize);
            currentIndexBufferSize = d.size;
        }

        auto camera = builder.getScene().rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera;
        auto cameraTransform = builder.getScene().rootNode.find<CameraProperty>().getProperty<TransformProperty>().transform;

        auto forwardColor = builder.getSlot(SLOT_FORWARD_COLOR);
        auto forwardDepth = builder.getSlot(SLOT_FORWARD_DEPTH);
        auto deferredDepth = builder.getSlot(SLOT_DEFERRED_DEPTH);

        FrameGraphResource pointLightShadowMap{};
        if (builder.checkSlot(SLOT_SHADOW_MAP_POINT)) {
            pointLightShadowMap = builder.getSlot(FrameGraphSlot::SLOT_SHADOW_MAP_POINT);
        }

        auto defaultPointLightShadowMap = builder.createTextureArrayBuffer({});

        auto atlasBuffers = atlas.getAtlasBuffers(builder);

        auto pointLights = getPointLights(scene);
        auto dirLights = getDirLights(scene);
        auto spotLights = getSpotLights(scene);

        builder.upload(pointLightBuffer,
                       [pointLights]() {
                           return FrameGraphCommand::UploadBuffer(pointLights.first.size() * sizeof(PointLightData),
                                                                  reinterpret_cast<const uint8_t *>(pointLights.first.data()));
                       });
        builder.upload(shadowPointLightBuffer,
                       [pointLights]() {
                           return FrameGraphCommand::UploadBuffer(pointLights.second.size() * sizeof(PointLightData),
                                                                  reinterpret_cast<const uint8_t *>(pointLights.second.data()));
                       });

        builder.upload(dirLightBuffer,
                       [dirLights]() {
                           return FrameGraphCommand::UploadBuffer(dirLights.first.size() * sizeof(DirectionalLightData),
                                                                  reinterpret_cast<const uint8_t *>(dirLights.first.data()));
                       });
        builder.upload(shadowDirLightBuffer,
                       [dirLights]() {
                           return FrameGraphCommand::UploadBuffer(dirLights.second.size() * sizeof(DirectionalLightData),
                                                                  reinterpret_cast<const uint8_t *>(dirLights.second.data()));
                       });

        builder.upload(spotLightBuffer,
                       [spotLights]() {
                           return FrameGraphCommand::UploadBuffer(spotLights.first.size() * sizeof(SpotLightData),
                                                                  reinterpret_cast<const uint8_t *>(spotLights.first.data()));
                       });
        builder.upload(shadowSpotLightBuffer,
                       [spotLights]() {
                           return FrameGraphCommand::UploadBuffer(spotLights.second.size() * sizeof(SpotLightData),
                                                                  reinterpret_cast<const uint8_t *>(spotLights.second.data()));
                       });

        meshAllocator.uploadMeshes(builder, vertexBuffer, indexBuffer);

        // Deallocate unused meshes
        std::set<Uri> dealloc;
        for (auto &pair: meshAllocator.getMeshAllocations()) {
            if (usedMeshes.find(pair.first) == usedMeshes.end()) {
                dealloc.insert(pair.first);
            }
        }

        for (auto &uri: dealloc) {
            meshAllocator.deallocateMesh(ResourceHandle<SkinnedMesh>(uri));
        }

        // Deallocate unused textures
        dealloc.clear();
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            deallocateTexture(ResourceHandle<Texture>(uri));
        }

        // Draw objects
        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        if (!nodes.empty()) {
            auto passesPerDrawCycle = nodes.size() / drawCycles;

            for (auto drawCycle = 0; drawCycle < drawCycles; drawCycle++) {
                std::vector<DrawCall> drawCalls;
                std::vector<size_t> baseVertices;
                std::vector<ShaderDrawData> shaderData;
                for (auto oi = 0; oi < passesPerDrawCycle && oi < nodes.size(); oi++) {
                    auto &node = nodes.at(oi + (drawCycle * passesPerDrawCycle));
                    auto &transformProp = node.getProperty<TransformProperty>();
                    auto &meshProp = node.getProperty<SkinnedMeshProperty>();

                    auto it = node.properties.find(typeid(MaterialProperty));
                    MaterialProperty matProp;
                    if (it != node.properties.end()) {
                        matProp = it->second->get<MaterialProperty>();
                    }

                    for (auto i = 0; i < meshProp.mesh.get().subMeshes.size() + 1; i++) {
                        auto &mesh = i <= 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(i - 1);

                        auto material = mesh.material.get();
                        auto mi = matProp.materials.find(i);
                        if (mi != matProp.materials.end()) {
                            material = mi->second.get();
                        }

                        if (!material.transparent)
                            continue;

                        auto model = transformProp.transform.model();

                        bool shadows = true;

                        if (!pointLightShadowMap.assigned) {
                            shadows = false;
                        } else if (node.hasProperty<ShadowProperty>()) {
                            shadows = node.getProperty<ShadowProperty>().receiveShadows;
                        }

                        auto data = ShaderDrawData();

                        data.model = model;
                        data.mvp = projection * view * model;
                        data.objectID_shadows[0] = static_cast<int>(oi);
                        data.objectID_shadows[1] = shadows;

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
                            auto tex = getTexture(material.metallicTexture, atlasBuffers);

                            data.metallic.level_index_filtering_assigned[0] = tex.level;
                            data.metallic.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.metallic.level_index_filtering_assigned[2] = material.metallicTexture.get().description.filterMag;
                            data.metallic.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.metallic.atlasScale_texSize[0] = atlasScale.x;
                            data.metallic.atlasScale_texSize[1] = atlasScale.y;
                            data.metallic.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.metallic.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.roughnessTexture.assigned()) {
                            auto tex = getTexture(material.roughnessTexture, atlasBuffers);

                            data.roughness.level_index_filtering_assigned[0] = tex.level;
                            data.roughness.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.roughness.level_index_filtering_assigned[2] = material.roughnessTexture.get().description.filterMag;
                            data.roughness.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.roughness.atlasScale_texSize[0] = atlasScale.x;
                            data.roughness.atlasScale_texSize[1] = atlasScale.y;
                            data.roughness.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.roughness.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.ambientOcclusionTexture.assigned()) {
                            auto tex = getTexture(material.ambientOcclusionTexture, atlasBuffers);

                            data.ambientOcclusion.level_index_filtering_assigned[0] = tex.level;
                            data.ambientOcclusion.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.ambientOcclusion.level_index_filtering_assigned[2] = material.ambientOcclusionTexture.get().description.filterMag;
                            data.ambientOcclusion.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.ambientOcclusion.atlasScale_texSize[0] = atlasScale.x;
                            data.ambientOcclusion.atlasScale_texSize[1] = atlasScale.y;
                            data.ambientOcclusion.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.ambientOcclusion.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.albedoTexture.assigned()) {
                            auto tex = getTexture(material.albedoTexture, atlasBuffers);

                            data.albedo.level_index_filtering_assigned[0] = tex.level;
                            data.albedo.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.albedo.level_index_filtering_assigned[2] = material.albedoTexture.get().description.filterMag;
                            data.albedo.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.albedo.atlasScale_texSize[0] = atlasScale.x;
                            data.albedo.atlasScale_texSize[1] = atlasScale.y;
                            data.albedo.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.albedo.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.normal.assigned()) {
                            auto tex = getTexture(material.normal, atlasBuffers);

                            data.normal.level_index_filtering_assigned[0] = tex.level;
                            data.normal.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.normal.level_index_filtering_assigned[2] = material.normal.get().description.filterMag;
                            data.normal.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.normal.atlasScale_texSize[0] = atlasScale.x;
                            data.normal.atlasScale_texSize[1] = atlasScale.y;
                            data.normal.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.normal.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        shaderData.emplace_back(data);

                        auto drawData = meshAllocator.getAllocatedMesh(meshProp.mesh);
                        auto &draw = drawData.data.at(i);

                        drawCalls.emplace_back(draw.drawCall);
                        baseVertices.emplace_back(draw.baseVertex);
                    }
                }

                builder.upload(shaderBuffer,
                               [shaderData]() {
                                   return FrameGraphCommand::UploadBuffer(shaderData.size() * sizeof(ShaderDrawData),
                                                                          reinterpret_cast<const uint8_t *>(shaderData.data()));
                               });
                builder.upload(shaderViewBuffer,
                               [cameraTransform, resolution]() {
                                   auto viewPos = cameraTransform.getPosition();
                                   ShaderViewData data{};
                                   data.viewPosition = {viewPos.x, viewPos.y, viewPos.z, 1};
                                   data.viewSize = {static_cast<float>(resolution.x),
                                                    static_cast<float>(resolution.y), 0, 0};
                                   return FrameGraphCommand::UploadBuffer(sizeof(ShaderViewData),
                                                                          reinterpret_cast<const uint8_t *>(&data));

                               });

                auto pointMap = pointLightShadowMap.assigned ? pointLightShadowMap : defaultPointLightShadowMap;

                builder.beginPass({FrameGraphAttachment::texture(forwardColor)},
                                  FrameGraphAttachment::texture(forwardDepth));
                builder.setViewport({}, resolution);
                builder.bindPipeline(pipeline);
                builder.bindVertexBuffers(vertexBuffer, indexBuffer, {}, SkinnedMesh::getDefaultVertexLayout(), {});
                builder.bindShaderResources({
                                                    {shaderViewBuffer,                  {{VERTEX,   ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                                                    {shaderBuffer,                      {{VERTEX,   ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                                                    {deferredDepth,                     {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {pointLightBuffer,                  {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {shadowPointLightBuffer,            {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {pointMap,                          {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {dirLightBuffer,                    {{FRAGMENT, ShaderResource::READ}}},
                                                    {shadowDirLightBuffer,              {{FRAGMENT, ShaderResource::READ}}},
                                                    {spotLightBuffer,                   {{FRAGMENT, ShaderResource::READ}}},
                                                    {shadowSpotLightBuffer,             {{FRAGMENT, ShaderResource::READ}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_8x8),         {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_16x16),       {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_32x32),       {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_64x64),       {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_128x128),     {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_256x256),     {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_512x512),     {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_1024x1024),   {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_2048x2048),   {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_4096x4096),   {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_8192x8192),   {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {atlasBuffers.at(
                                                            TEXTURE_ATLAS_16384x16384), {{{FRAGMENT, ShaderResource::READ}}}},
                                            });
                builder.multiDrawIndexed(drawCalls, baseVertices);
                builder.finishPass();
            }
        }
    }

    std::type_index ForwardLightingPass::getTypeIndex() const {
        return typeid(ForwardLightingPass);
    }

    TextureAtlasHandle ForwardLightingPass::getTexture(const ResourceHandle<Texture> &texture,
                                                       std::map<TextureAtlasResolution, FrameGraphResource> &atlasBuffers) {
        if (textures.find(texture.getUri()) == textures.end()) {
            auto handle = atlas.add(texture.get().image.get());
            textures[texture.getUri()] = handle;
        }
        return textures.at(texture.getUri());
    }

    void ForwardLightingPass::deallocateTexture(const ResourceHandle<Texture> &texture) {
        atlas.remove(textures.at(texture.getUri()));
        textures.erase(texture.getUri());
    }
}