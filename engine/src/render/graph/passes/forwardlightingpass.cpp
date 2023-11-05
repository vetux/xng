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

namespace xng {
#pragma pack(push, 1)
    struct PointLightData {
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
        for (auto &node: scene.rootNode.findAll({typeid(Scene::PointLightProperty)})) {
            auto l = node.getProperty<Scene::PointLightProperty>().light;
            auto t = node.getProperty<Scene::TransformProperty>().transform;
            auto v = l.color.divide();
            auto tmp = PointLightData{
                    .position =  Vec4f(t.getPosition().x,
                                       t.getPosition().y,
                                       t.getPosition().z,
                                       0).getMemory(),
                    .color = Vec4f(v.x * l.power, v.y * l.power, v.z * l.power, 1).getMemory(),
            };
            if (l.castShadows)
                shadowLights.emplace_back(tmp);
            else
                pointLights.emplace_back(tmp);
        }
        return {pointLights, shadowLights};
    }

    void ForwardLightingPass::setup(FrameGraphBuilder &builder) {
        renderSize = builder.getRenderSize();
        scene = builder.getScene();

        auto pointLightNodes = scene.rootNode.findAll({typeid(Scene::PointLightProperty)});

        size_t pointLights = 0;
        size_t shadowPointLights = 0;

        for (auto l: pointLightNodes) {
            if (l.getProperty<Scene::PointLightProperty>().light.castShadows)
                shadowPointLights++;
            else
                pointLights++;
        }

        pointLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * pointLights
        });
        builder.read(pointLightBufferRes);
        builder.write(pointLightBufferRes);

        shadowPointLightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * shadowPointLights
        });
        builder.read(shadowPointLightBufferRes);
        builder.write(shadowPointLightBufferRes);

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
                            BIND_TEXTURE_ARRAY_BUFFER
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

        size_t totalShaderBufferSize = sizeof(float[12]);

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

        if (builder.checkSlot(SLOT_SHADOW_MAP_POINT)) {
            defPointShadowMap = {};
            pointLightShadowMapRes = builder.getSlot(FrameGraphSlot::SLOT_SHADOW_MAP_POINT);
            builder.read(pointLightShadowMapRes);
        } else {
            pointLightShadowMapRes = {};
            defPointShadowMap = builder.createTextureArrayBuffer({});
            builder.read(defPointShadowMap);
        }
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

        auto &pointLightBuffer = resources.get<ShaderStorageBuffer>(pointLightBufferRes);
        auto &shadowPointLightBuffer = resources.get<ShaderStorageBuffer>(shadowPointLightBufferRes);

        auto &pointLightShadowMap = pointLightShadowMapRes.assigned ? resources.get<TextureArrayBuffer>(
                pointLightShadowMapRes) : resources.get<TextureArrayBuffer>(defPointShadowMap);

        auto &cBuffer = resources.get<CommandBuffer>(commandBuffer);

        auto atlasBuffers = atlas.getAtlasBuffers(resources, cBuffer, renderQueues.at(0));

        auto pointLights = getPointLights(scene);

        pointLightBuffer.upload(reinterpret_cast<const uint8_t *>(pointLights.first.data()),
                                pointLights.first.size() * sizeof(PointLightData));
        shadowPointLightBuffer.upload(reinterpret_cast<const uint8_t *>(pointLights.second.data()),
                                      pointLights.second.size() * sizeof(PointLightData));

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
        target.setAttachments({RenderTargetAttachment::texture(forwardColor)},
                              RenderTargetAttachment::texture(forwardDepth));

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

                        if (!material.transparent)
                            continue;

                        auto model = transformProp.transform.model();

                        bool shadows = true;

                        if (!pointLightShadowMapRes.assigned) {
                            shadows = false;
                        } else if (node.hasProperty<Scene::ShadowProperty>()) {
                            shadows = node.getProperty<Scene::ShadowProperty>().receiveShadows;
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

                auto viewPos = cameraTransform.getPosition();
                float viewArr[4] = {viewPos.x, viewPos.y, viewPos.z, 1};
                float viewSize[4] = {static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0, 0};
                float farPlane[4] = {1000, 0, 0, 1};
                shaderBuffer.upload(0, reinterpret_cast<const uint8_t *>(viewArr), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[4]), reinterpret_cast<const uint8_t *>(viewSize), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[8]), reinterpret_cast<const uint8_t *>(farPlane), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[12]),
                                    reinterpret_cast<const uint8_t *>(shaderData.data()),
                                    shaderData.size() * sizeof(ShaderDrawData));

                commands.emplace_back(pass.begin(target));
                commands.emplace_back(pass.setViewport({}, target.getDescription().size));
                commands.emplace_back(pipeline.bind());
                commands.emplace_back(vertexArrayObject.bind());

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
                        {shadowPointLightBuffer,                     {{{FRAGMENT, ShaderResource::READ}}}},
                        {pointLightShadowMap,                        {{{FRAGMENT, ShaderResource::READ}}}},
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

        target.clearAttachments();
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