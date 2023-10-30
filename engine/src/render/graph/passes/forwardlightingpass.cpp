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

#include "xng/geometry/vertexstream.hpp"

namespace xng {
#pragma pack(push, 1)
    struct DirectionalLightData {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> direction;
    };

    struct PointLightData {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> position;
        std::array<float, 4> constant_linear_quadratic;
    };

    struct SpotLightData {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> position;
        std::array<float, 4> direction_quadratic;
        std::array<float, 4> cutOff_outerCutOff_constant_linear;
    };

    struct PBRPointLightData {
        std::array<float, 4> position;
        std::array<float, 4> color;
    };

    struct ShaderAtlasTexture {
        int level_index_filtering_assigned[4]{0, 0, 0, 0};
        float atlasScale_texSize[4]{0, 0, 0, 0};
    };

    struct ShaderDrawData {
        Mat4f model;
        Mat4f mvp;

        int shadeModel_objectID[4]{0, 0, 0, 0};
        float metallic_roughness_ambientOcclusion_shininess[4]{0, 0, 0, 0};

        float diffuseColor[4]{0, 0, 0, 0};
        float ambientColor[4]{0, 0, 0, 0};
        float specularColor[4]{0, 0, 0, 0};

        float normalIntensity[4]{0, 0, 0, 0};

        ShaderAtlasTexture normal;

        ShaderAtlasTexture metallic;
        ShaderAtlasTexture roughness;
        ShaderAtlasTexture ambientOcclusion;

        ShaderAtlasTexture diffuse;
        ShaderAtlasTexture ambient;
        ShaderAtlasTexture specular;
        ShaderAtlasTexture shininess;
    };
#pragma pack(pop)

    static std::vector<DirectionalLightData> getDirLights(const Scene &scene) {
        std::vector<DirectionalLightData> ret;
        for (auto &node: scene.rootNode.findAll({typeid(Scene::PhongDirectionalLightProperty)})) {
            auto l = node.getProperty<Scene::PhongDirectionalLightProperty>().light;
            auto t = node.getProperty<Scene::TransformProperty>().transform;
            auto tmp = DirectionalLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            auto euler = (Quaternion(l.direction) * t.getRotation()).getEulerAngles();
            tmp.direction = Vec4f(euler.x, euler.y, euler.z, 0).getMemory();
            ret.emplace_back(tmp);
        }
        return ret;
    }

    static std::vector<PointLightData> getPointLights(const Scene &scene) {
        std::vector<PointLightData> ret;
        for (auto &node: scene.rootNode.findAll({typeid(Scene::PhongPointLightProperty)})) {
            auto l = node.getProperty<Scene::PhongPointLightProperty>().light;
            auto t = node.getProperty<Scene::TransformProperty>().transform;
            auto tmp = PointLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            tmp.position = Vec4f(t.getPosition().x,
                                 t.getPosition().y,
                                 t.getPosition().z,
                                 0).getMemory();
            tmp.constant_linear_quadratic[0] = l.constant;
            tmp.constant_linear_quadratic[1] = l.linear;
            tmp.constant_linear_quadratic[2] = l.quadratic;
            ret.emplace_back(tmp);
        }
        return ret;
    }

    static std::vector<SpotLightData> getSpotLights(const Scene &scene) {
        std::vector<SpotLightData> ret;
        for (auto &node: scene.rootNode.findAll({typeid(Scene::PhongSpotLightProperty)})) {
            auto l = node.getProperty<Scene::PhongSpotLightProperty>().light;
            auto t = node.getProperty<Scene::TransformProperty>().transform;
            auto tmp = SpotLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            tmp.position = Vec4f(t.getPosition().x,
                                 t.getPosition().y,
                                 t.getPosition().z,
                                 0).getMemory();

            auto euler = (Quaternion(l.direction) * t.getRotation()).getEulerAngles();

            tmp.direction_quadratic[0] = euler.x;
            tmp.direction_quadratic[1] = euler.y;
            tmp.direction_quadratic[2] = euler.z;
            tmp.direction_quadratic[3] = l.quadratic;

            tmp.cutOff_outerCutOff_constant_linear[0] = l.cutOff;
            tmp.cutOff_outerCutOff_constant_linear[1] = l.outerCutOff;
            tmp.cutOff_outerCutOff_constant_linear[2] = l.constant;
            tmp.cutOff_outerCutOff_constant_linear[3] = l.linear;

            ret.emplace_back(tmp);
        }
        return ret;
    }

    static std::vector<PBRPointLightData> getPBRPointLights(const Scene &scene) {
        std::vector<PBRPointLightData> ret;
        for (auto &node: scene.rootNode.findAll({typeid(Scene::PBRPointLightProperty)})) {
            auto l = node.getProperty<Scene::PBRPointLightProperty>().light;
            auto t = node.getProperty<Scene::TransformProperty>().transform;
            auto v = l.color.divide();
            auto tmp = PBRPointLightData{
                    .position =  Vec4f(t.getPosition().x,
                                       t.getPosition().y,
                                       t.getPosition().z,
                                       0).getMemory(),
                    .color = Vec4f(v.x * l.energy, v.y * l.energy, v.z * l.energy, 1).getMemory(),
            };
            ret.emplace_back(tmp);
        }
        return ret;
    }

    void ForwardLightingPass::setup(FrameGraphBuilder &builder) {
        renderSize = builder.getRenderSize();
        scene = builder.getScene();

        size_t pointLights = scene.rootNode.findAll({typeid(Scene::PhongPointLightProperty)}).size();
        size_t spotLights = scene.rootNode.findAll({typeid(Scene::PhongSpotLightProperty)}).size();
        size_t directionalLights = scene.rootNode.findAll({typeid(Scene::PhongDirectionalLightProperty)}).size();
        size_t pbrPointLights = scene.rootNode.findAll({typeid(Scene::PBRPointLightProperty)}).size();

        pointLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * pointLights
        });
        builder.read(pointLightsBufferRes);
        builder.write(pointLightsBufferRes);

        spotLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * spotLights
        });
        builder.read(spotLightsBufferRes);
        builder.write(spotLightsBufferRes);

        directionalLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * directionalLights
        });
        builder.read(directionalLightsBufferRes);
        builder.write(directionalLightsBufferRes);

        pbrPointLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PBRPointLightData) * pbrPointLights
        });
        builder.read(pbrPointLightsBufferRes);
        builder.write(pbrPointLightsBufferRes);

        RenderTargetDesc targetDesc;
        targetDesc.size = builder.getRenderSize();
        targetDesc.numberOfColorAttachments = 1;
        targetDesc.hasDepthStencilAttachment = true;
        targetRes = builder.createRenderTarget(targetDesc);

        builder.read(targetRes);

        if (!pipelineRes.assigned) {
            pipelineRes = builder.createPipeline(RenderPipelineDesc{
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
        builder.read(pipelineRes);

        if (!vertexArrayObjectRes.assigned) {
            vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                    .vertexLayout = SkinnedMesh::getDefaultVertexLayout()
            });
        }

        builder.read(vertexArrayObjectRes);
        builder.write(vertexArrayObjectRes);
        builder.persist(vertexArrayObjectRes);

        RenderPassDesc passDesc;
        passDesc.numberOfColorAttachments = 1;
        passDesc.hasDepthStencilAttachment = true;
        passRes = builder.createRenderPass(passDesc);
        builder.read(passRes);

        nodes.clear();

        size_t totalShaderBufferSize = sizeof(float[8]);

        usedTextures.clear();
        usedMeshes.clear();
        for (auto &node: scene.rootNode.findAll({typeid(Scene::SkinnedMeshProperty)})) {
            auto &meshProp = node.getProperty<Scene::SkinnedMeshProperty>();
            if (!meshProp.mesh.assigned()) {
                continue;
            }

            usedMeshes.insert(meshProp.mesh.getUri());
            meshAllocator.prepareMeshAllocation(meshProp.mesh);

            auto &mesh = meshProp.mesh.get();

            auto it = node.properties.find(typeid(Scene::MaterialProperty));
            Scene::MaterialProperty matProp;
            if (it != node.properties.end()) {
                matProp = it->second->get<Scene::MaterialProperty>();
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

                if (!mat.transparent || mat.shader.assigned())
                    continue;

                usedTextures.insert(mat.normal.getUri());
                usedTextures.insert(mat.metallicTexture.getUri());
                usedTextures.insert(mat.roughnessTexture.getUri());
                usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                usedTextures.insert(mat.diffuseTexture.getUri());
                usedTextures.insert(mat.ambientTexture.getUri());
                usedTextures.insert(mat.specularTexture.getUri());
                usedTextures.insert(mat.shininessTexture.getUri());

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
                if (mat.diffuseTexture.assigned()
                    && textures.find(mat.diffuseTexture.getUri()) == textures.end()) {
                    textures[mat.diffuseTexture.getUri()] = atlas.add(
                            mat.diffuseTexture.get().image.get());
                }
                if (mat.ambientTexture.assigned()
                    && textures.find(mat.ambientTexture.getUri()) == textures.end()) {
                    textures[mat.ambientTexture.getUri()] = atlas.add(
                            mat.ambientTexture.get().image.get());
                }
                if (mat.specularTexture.assigned()
                    && textures.find(mat.specularTexture.getUri()) == textures.end()) {
                    textures[mat.specularTexture.getUri()] = atlas.add(
                            mat.specularTexture.get().image.get());
                }
                if (mat.shininessTexture.assigned()
                    && textures.find(mat.shininessTexture.getUri()) == textures.end()) {
                    textures[mat.shininessTexture.getUri()] = atlas.add(
                            mat.shininessTexture.get().image.get());
                }

                totalShaderBufferSize += sizeof(ShaderDrawData);
            }
            nodes.emplace_back(node);
        }

        size_t maxBufferSize = builder.getDeviceInfo().storageBufferMaxSize;

        drawCycles = 0;
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

        shaderBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = bufSize
        });
        builder.write(shaderBufferRes);

        atlas.setup(builder);

        if (vertexBufferRes.assigned) {
            builder.read(vertexBufferRes);
            builder.write(vertexBufferRes);
            builder.persist(vertexBufferRes);
        }

        if (indexBufferRes.assigned) {
            builder.read(indexBufferRes);
            builder.write(indexBufferRes);
            builder.persist(indexBufferRes);
        }

        if (!vertexBufferRes.assigned || currentVertexBufferSize < meshAllocator.getRequestedVertexBufferSize()) {
            staleVertexBuffer = vertexBufferRes;
            auto d = VertexBufferDesc();
            d.size = meshAllocator.getRequestedVertexBufferSize();
            vertexBufferRes = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.read(vertexBufferRes);
            builder.write(vertexBufferRes);
            builder.persist(vertexBufferRes);
        }

        if (!indexBufferRes.assigned || currentIndexBufferSize < meshAllocator.getRequestedIndexBufferSize()) {
            staleIndexBuffer = indexBufferRes;
            auto d = IndexBufferDesc();
            d.size = meshAllocator.getRequestedIndexBufferSize();
            indexBufferRes = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.read(indexBufferRes);
            builder.write(indexBufferRes);
            builder.persist(indexBufferRes);
        }

        camera = scene.rootNode.getProperty<Scene::CameraProperty>().camera;
        cameraTransform = scene.rootNode.getProperty<Scene::CameraProperty>().cameraTransform;

        forwardColorRes = builder.getSlot(SLOT_FORWARD_COLOR);
        forwardDepthRes = builder.getSlot(SLOT_FORWARD_DEPTH);
        deferredDepthRes = builder.getSlot(SLOT_DEFERRED_DEPTH);

        builder.write(forwardColorRes);
        builder.write(forwardDepthRes);
        builder.read(deferredDepthRes);

        commandBuffer = builder.createCommandBuffer();

        builder.write(commandBuffer);
    }

    void ForwardLightingPass::execute(FrameGraphPassResources &resources,
                                      const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                                      const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                                      const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) {
        auto &target = resources.get<RenderTarget>(targetRes);
        auto &pipeline = resources.get<RenderPipeline>(pipelineRes);
        auto &pass = resources.get<RenderPass>(passRes);

        auto &shaderBuffer = resources.get<ShaderStorageBuffer>(shaderBufferRes);
        auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);
        auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
        auto &indexBuffer = resources.get<IndexBuffer>(indexBufferRes);

        auto &forwardColor = resources.get<TextureBuffer>(forwardColorRes);
        auto &forwardDepth = resources.get<TextureBuffer>(forwardDepthRes);
        auto &deferredDepth = resources.get<TextureBuffer>(deferredDepthRes);

        auto &pointLightBuffer = resources.get<ShaderStorageBuffer>(pointLightsBufferRes);
        auto &spotLightBuffer = resources.get<ShaderStorageBuffer>(spotLightsBufferRes);
        auto &dirLightBuffer = resources.get<ShaderStorageBuffer>(directionalLightsBufferRes);
        auto &pbrLightBuffer = resources.get<ShaderStorageBuffer>(pbrPointLightsBufferRes);

        auto &cBuffer = resources.get<CommandBuffer>(commandBuffer);

        auto atlasBuffers = atlas.getAtlasBuffers(resources, cBuffer, renderQueues.at(0));

        auto plights = getPointLights(scene);
        auto slights = getSpotLights(scene);
        auto dlights = getDirLights(scene);
        auto pbrlights = getPBRPointLights(scene);

        pointLightBuffer.upload(reinterpret_cast<const uint8_t *>(plights.data()),
                                plights.size() * sizeof(PointLightData));
        spotLightBuffer.upload(reinterpret_cast<const uint8_t *>(slights.data()),
                               slights.size() * sizeof(SpotLightData));
        dirLightBuffer.upload(reinterpret_cast<const uint8_t *>(dlights.data()),
                              dlights.size() * sizeof(DirectionalLightData));
        pbrLightBuffer.upload(reinterpret_cast<const uint8_t *>(pbrlights.data()),
                              pbrlights.size() * sizeof(PBRPointLightData));

        std::vector<Command> commands;

        bool updateVao = false;
        if (staleVertexBuffer.assigned) {
            auto &staleBuffer = resources.get<VertexBuffer>(staleVertexBuffer);
            commands.emplace_back(vertexBuffer.copy(staleBuffer));
            staleVertexBuffer = {};
            updateVao = true;
        }

        if (staleIndexBuffer.assigned) {
            auto &staleBuffer = resources.get<IndexBuffer>(staleIndexBuffer);
            commands.emplace_back(indexBuffer.copy(staleBuffer));
            staleIndexBuffer = {};
            updateVao = true;
        }

        // Clear textures
        target.setAttachments({forwardColor}, forwardDepth);

        commands.emplace_back(pass.begin(target));
        commands.emplace_back(pass.clearColorAttachments(ColorRGBA(0)));
        commands.emplace_back(pass.clearDepthAttachment(1));
        commands.emplace_back(pass.end());

        cBuffer.begin();
        cBuffer.add(commands);
        cBuffer.end();

        commands.clear();

        renderQueues.at(0).get().submit({cBuffer}, {}, {});

        if (updateVao || bindVao) {
            bindVao = false;
            vertexArrayObject.setBuffers(vertexBuffer, indexBuffer);
        }

        meshAllocator.uploadMeshes(vertexBuffer, indexBuffer);

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
                    auto &transformProp = node.getProperty<Scene::TransformProperty>();
                    auto &meshProp = node.getProperty<Scene::SkinnedMeshProperty>();

                    auto it = node.properties.find(typeid(Scene::MaterialProperty));
                    Scene::MaterialProperty matProp;
                    if (it != node.properties.end()) {
                        matProp = it->second->get<Scene::MaterialProperty>();
                    }

                    for (auto i = 0; i < meshProp.mesh.get().subMeshes.size() + 1; i++) {
                        auto &mesh = i <= 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(i - 1);

                        auto material = mesh.material.get();
                        auto mi = matProp.materials.find(i);
                        if (mi != matProp.materials.end()) {
                            material = mi->second.get();
                        }

                        if (!material.transparent || material.shader.assigned())
                            continue;

                        auto model = transformProp.transform.model();

                        auto data = ShaderDrawData();

                        data.model = model;
                        data.mvp = projection * view * model;
                        data.shadeModel_objectID[0] = material.shadingModel;
                        data.shadeModel_objectID[1] = static_cast<int>(oi);

                        data.metallic_roughness_ambientOcclusion_shininess[0] = material.metallic;
                        data.metallic_roughness_ambientOcclusion_shininess[1] = material.roughness;
                        data.metallic_roughness_ambientOcclusion_shininess[2] = material.ambientOcclusion;
                        data.metallic_roughness_ambientOcclusion_shininess[3] = material.shininess;

                        auto col = material.diffuse.divide().getMemory();
                        data.diffuseColor[0] = col[0];
                        data.diffuseColor[1] = col[1];
                        data.diffuseColor[2] = col[2];
                        data.diffuseColor[3] = col[3];

                        col = material.ambient.divide().getMemory();
                        data.ambientColor[0] = col[0];
                        data.ambientColor[1] = col[1];
                        data.ambientColor[2] = col[2];
                        data.ambientColor[3] = col[3];

                        col = material.specular.divide().getMemory();
                        data.specularColor[0] = col[0];
                        data.specularColor[1] = col[1];
                        data.specularColor[2] = col[2];
                        data.specularColor[3] = col[3];

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

                        if (material.diffuseTexture.assigned()) {
                            auto tex = getTexture(material.diffuseTexture, atlasBuffers);

                            data.diffuse.level_index_filtering_assigned[0] = tex.level;
                            data.diffuse.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.diffuse.level_index_filtering_assigned[2] = material.diffuseTexture.get().description.filterMag;
                            data.diffuse.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.diffuse.atlasScale_texSize[0] = atlasScale.x;
                            data.diffuse.atlasScale_texSize[1] = atlasScale.y;
                            data.diffuse.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.diffuse.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.ambientTexture.assigned()) {
                            auto tex = getTexture(material.ambientTexture, atlasBuffers);

                            data.ambient.level_index_filtering_assigned[0] = tex.level;
                            data.ambient.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.ambient.level_index_filtering_assigned[2] = material.ambientTexture.get().description.filterMag;
                            data.ambient.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.ambient.atlasScale_texSize[0] = atlasScale.x;
                            data.ambient.atlasScale_texSize[1] = atlasScale.y;
                            data.ambient.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.ambient.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.specularTexture.assigned()) {
                            auto tex = getTexture(material.specularTexture, atlasBuffers);

                            data.specular.level_index_filtering_assigned[0] = tex.level;
                            data.specular.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.specular.level_index_filtering_assigned[2] = material.specularTexture.get().description.filterMag;
                            data.specular.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.specular.atlasScale_texSize[0] = atlasScale.x;
                            data.specular.atlasScale_texSize[1] = atlasScale.y;
                            data.specular.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.specular.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                        }

                        if (material.shininessTexture.assigned()) {
                            auto tex = getTexture(material.shininessTexture, atlasBuffers);

                            data.shininess.level_index_filtering_assigned[0] = tex.level;
                            data.shininess.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                            data.shininess.level_index_filtering_assigned[2] = material.shininessTexture.get().description.filterMag;
                            data.shininess.level_index_filtering_assigned[3] = 1;

                            auto atlasScale = tex.size.convert<float>()
                                              / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                            data.shininess.atlasScale_texSize[0] = atlasScale.x;
                            data.shininess.atlasScale_texSize[1] = atlasScale.y;
                            data.shininess.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                            data.shininess.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
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

                auto viewPos = cameraTransform.getPosition();
                float viewArr[4] = {viewPos.x, viewPos.y, viewPos.z, 1};
                float viewSize[4] = {static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0, 0};
                shaderBuffer.upload(0, reinterpret_cast<const uint8_t *>(viewArr), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[4]), reinterpret_cast<const uint8_t *>(viewSize), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[8]),
                                    reinterpret_cast<const uint8_t *>(shaderData.data()),
                                    shaderData.size() * sizeof(ShaderDrawData));

                commands.emplace_back(pass.begin(target));
                commands.emplace_back(pass.setViewport({}, target.getDescription().size));
                commands.emplace_back(pipeline.bind());
                commands.emplace_back(vertexArrayObject.bind());

                commands.emplace_back(pass.clearColorAttachments(ColorRGBA(0)));
                commands.emplace_back(pass.clearDepthAttachment(1));

                auto shaderRes = std::vector<ShaderResource>{
                        {shaderBuffer,                               {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_8x8),         {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_16x16),       {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_32x32),       {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_64x64),       {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_128x128),     {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_256x256),     {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_512x512),     {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_1024x1024),   {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_2048x2048),   {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_4096x4096),   {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_8192x8192),   {{{FRAGMENT, ShaderResource::READ}}}},
                        {atlasBuffers.at(TEXTURE_ATLAS_16384x16384), {{{FRAGMENT, ShaderResource::READ}}}},
                        {deferredDepth,                              {{{FRAGMENT, ShaderResource::READ}}}},
                        {pointLightBuffer,                           {{{FRAGMENT, ShaderResource::READ}}}},
                        {spotLightBuffer,                            {{{FRAGMENT, ShaderResource::READ}}}},
                        {dirLightBuffer,                             {{{FRAGMENT, ShaderResource::READ}}}},
                        {pbrLightBuffer,                             {{{FRAGMENT, ShaderResource::READ}}}},
                };
                commands.emplace_back(RenderPipeline::bindShaderResources(shaderRes));
                commands.emplace_back(pass.multiDrawIndexed(drawCalls, baseVertices));

                commands.emplace_back(pass.end());

                cBuffer.begin();
                cBuffer.add(commands);
                cBuffer.end();

                renderQueues.at(0).get().submit(cBuffer);

                commands.clear();
            }
        }

        target.setAttachments({});
    }

    std::type_index ForwardLightingPass::getTypeIndex() const {
        return typeid(ForwardLightingPass);
    }

    TextureAtlasHandle ForwardLightingPass::getTexture(const ResourceHandle<Texture> &texture,
                                                       std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers) {
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