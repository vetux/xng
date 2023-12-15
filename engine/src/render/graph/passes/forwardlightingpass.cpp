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

#include "graph/forwardlightingpass_vs.hpp"
#include "graph/forwardlightingpass_fs.hpp"

#include "xng/render/geometry/vertexstream.hpp"
#include "xng/render/graph/framegraphsettings.hpp"

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
        auto renderSize = builder.getBackBufferDescription().size
                          * builder.getSettings().get<float>(FrameGraphSettings::SETTING_RENDER_SCALE);
        auto scene = builder.getScene();

        auto pointLightNodes = scene.rootNode.findAll({typeid(PointLightProperty)});

        size_t pointLights = 0;
        size_t shadowPointLights = 0;

        for (auto l: pointLightNodes) {
            if (l.getProperty<PointLightProperty>().light.castShadows)
                shadowPointLights++;
            else
                pointLights++;
        }

        auto dirLightNodes = scene.rootNode.findAll({typeid(DirectionalLightProperty)});

        size_t dirLights = 0;
        size_t shadowDirLights = 0;

        for (auto l: dirLightNodes) {
            if (l.getProperty<DirectionalLightProperty>().light.castShadows)
                shadowDirLights++;
            else
                dirLights++;
        }

        auto spotLightNodes = scene.rootNode.findAll({typeid(SpotLightProperty)});

        size_t spotLights = 0;
        size_t shadowSpotLights = 0;

        for (auto l: spotLightNodes) {
            if (l.getProperty<SpotLightProperty>().light.castShadows)
                shadowSpotLights++;
            else
                spotLights++;
        }

        auto  pointLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * pointLights
        });

        auto shadowPointLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * shadowPointLights
        });

        auto dirLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * dirLights
        });

        auto shadowDirLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * shadowDirLights
        });

        auto spotLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * spotLights
        });

        auto shadowSpotLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * shadowSpotLights
        });

        if (!pipelineRes.assigned) {
            pipelineRes = builder.createRenderPipeline(RenderPipelineDesc{
                    .shaders = {{VERTEX,   forwardlightingpass_vs},
                                {FRAGMENT, forwardlightingpass_fs}},
                    .bindings = {
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
                            BIND_TEXTURE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_SHADER_STORAGE_BUFFER,
                    },
                    .vertexLayout = SkinnedMesh::getDefaultVertexLayout(),
                    /*      .clearColorValue = ColorRGBA(0, 0, 0, 0),
                          .clearColor = false,
                          .clearDepth = false,
                          .clearStencil = false,*/
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

        builder.persist(pipelineRes);

        if (!vertexArrayObjectRes.assigned) {
            vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                    .vertexLayout = SkinnedMesh::getDefaultVertexLayout()
            });
        }

        builder.persist(vertexArrayObjectRes);

        size_t totalShaderBufferSize = sizeof(float[12]);

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

        auto shaderBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = bufSize
        });

        atlas.setup(builder);

        if (vertexBufferRes.assigned) {
            builder.persist(vertexBufferRes);
        }

        if (indexBufferRes.assigned) {
            builder.persist(indexBufferRes);
        }

        bool updateVao = false;

        if (!vertexBufferRes.assigned || currentVertexBufferSize < meshAllocator.getRequestedVertexBufferSize()) {
            auto staleVertexBuffer = vertexBufferRes;
            auto d = VertexBufferDesc();
            d.size = meshAllocator.getRequestedVertexBufferSize();
            vertexBufferRes = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.persist(vertexBufferRes);
            builder.copy(staleVertexBuffer, vertexBufferRes);
            updateVao = true;
        }

        if (!indexBufferRes.assigned || currentIndexBufferSize < meshAllocator.getRequestedIndexBufferSize()) {
            auto staleIndexBuffer = indexBufferRes;
            auto d = IndexBufferDesc();
            d.size = meshAllocator.getRequestedIndexBufferSize();
            indexBufferRes = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.persist(indexBufferRes);
            builder.copy(staleIndexBuffer, indexBufferRes);
            updateVao = true;
        }

        auto camera = builder.getScene().rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera;
        auto cameraTransform = builder.getScene().rootNode.find<CameraProperty>().getProperty<TransformProperty>().transform;

        auto forwardColorRes = builder.getSlot(SLOT_FORWARD_COLOR);
        auto forwardDepthRes = builder.getSlot(SLOT_FORWARD_DEPTH);
        auto deferredDepthRes = builder.getSlot(SLOT_DEFERRED_DEPTH);

        FrameGraphResource pointLightShadowMapRes;
        if (builder.checkSlot(SLOT_SHADOW_MAP_POINT)) {
            pointLightShadowMapRes = builder.getSlot(FrameGraphSlot::SLOT_SHADOW_MAP_POINT);
        } else {
            pointLightShadowMapRes = builder.createTextureArrayBuffer({});
        }

        auto atlasBuffers = atlas.getAtlasBuffers(builder);

        builder.upload(pointLightBufferRes,
                       [scene]() {
                           auto lights = getPointLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(PointLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });
        builder.upload(shadowPointLightBufferRes,
                       [scene]() {
                           auto lights = getPointLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(PointLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });

        builder.upload(dirLightBufferRes,
                       [scene]() {
                           auto lights = getDirLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(DirectionalLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });
        builder.upload(shadowDirLightBufferRes,
                       [scene]() {
                           auto lights = getDirLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(DirectionalLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });

        builder.upload(spotLightBufferRes,
                       [scene]() {
                           auto lights = getSpotLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(SpotLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });
        builder.upload(shadowSpotLightBufferRes,
                       [scene]() {
                           auto lights = getSpotLights(scene);
                           return FrameGraphCommand::UploadBuffer(lights.first.size() * sizeof(SpotLightData),
                                                                  reinterpret_cast<const uint8_t *>(lights.first.data()));
                       });

        // Clear textures
        builder.clearTextureColor({forwardColorRes}, ColorRGBA::black());
        builder.clearTextureFloat({forwardDepthRes}, 1);

        if (updateVao || bindVao) {
            bindVao = false;
            builder.setVertexArrayObjectBuffers(vertexArrayObjectRes, vertexBufferRes, indexBufferRes, {});
        }

        meshAllocator.uploadMeshes(builder, vertexBufferRes, indexBufferRes);

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

                        if (!pointLightShadowMapRes.assigned) {
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

                float viewSize[4] = {static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0, 0};

                builder.upload(shaderBufferRes,
                               [cameraTransform, shaderData, renderSize]() {
                                   auto viewPos = cameraTransform.getPosition();
                                   float viewArr[4] = {viewPos.x, viewPos.y, viewPos.z, 1};
                                   float viewSize[4] = {static_cast<float>(renderSize.x),
                                                        static_cast<float>(renderSize.y), 0, 0};

                                   std::vector<uint8_t> ret((sizeof(float) * 8)
                                                            + (shaderData.size()) * sizeof(ShaderDrawData));

                                   for (auto i = 0; i < sizeof(viewArr); i++) {
                                       for (auto y = 0; y < sizeof(float); y++)
                                           ret[i + y] = reinterpret_cast<const uint8_t *>(&viewArr[i])[i + y];
                                   }

                                   for (auto i = sizeof(viewArr); i < sizeof(viewArr) + sizeof(viewSize); i++) {
                                       for (auto y = 0; y < sizeof(float); y++)
                                           ret[i + y] = reinterpret_cast<const uint8_t *>(&viewSize[i])[i + y];
                                   }

                                   return FrameGraphCommand::UploadBuffer(ret.size(), ret.data());
                               });

                builder.beginPass({FrameGraphCommand::Attachment::texture(forwardColorRes)},
                                  FrameGraphCommand::Attachment::texture(forwardDepthRes));
                builder.setViewport({}, renderSize);
                builder.bindPipeline(pipelineRes);
                builder.bindVertexArrayObject(vertexArrayObjectRes);
                builder.bindShaderResources({
                                                    {shaderBufferRes,                   {{VERTEX,   ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
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
                                                    {deferredDepthRes,                  {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {pointLightBufferRes,               {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {shadowPointLightBufferRes,         {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {pointLightShadowMapRes,            {{{FRAGMENT, ShaderResource::READ}}}},
                                                    {dirLightBufferRes,                 {{FRAGMENT, ShaderResource::READ}}},
                                                    {shadowDirLightBufferRes,           {{FRAGMENT, ShaderResource::READ}}},
                                                    {spotLightBufferRes,                {{FRAGMENT, ShaderResource::READ}}},
                                                    {shadowSpotLightBufferRes,          {{FRAGMENT, ShaderResource::READ}}},
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