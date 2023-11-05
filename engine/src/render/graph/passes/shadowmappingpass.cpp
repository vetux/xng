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

#include "xng/render/graph/passes/shadowmappingpass.hpp"
#include "xng/render/graph/framegraphbuilder.hpp"

#include "graph/shadowmappingpass_fs.hpp"
#include "graph/shadowmappingpass_vs.hpp"
#include "graph/shadowmappingpass_gs.hpp"

#include "xng/math/rotation.hpp"

#include "xng/render/graph/framegraphsettings.hpp"

struct ShaderDrawData {
    std::array<int, 4> boneOffset{};
    Mat4f model;
};

struct LightData {
    std::array<float, 4> lightPosFarPlane{};
    std::array<int, 4> layer{};
    std::array<Mat4f, 6> shadowMatrices;
};

void xng::ShadowMappingPass::setup(xng::FrameGraphBuilder &builder) {
    if (builder.getProperties().has(FrameGraphSettings::SHADOW_MAPPING_RESOLUTION)) {
        resolution = builder.getProperties().get<Vec2i>(FrameGraphSettings::SHADOW_MAPPING_RESOLUTION);
    }

    meshNodes.clear();
    pointLightNodes.clear();

    for (auto &lightNode: builder.getScene().rootNode.findAll({typeid(Scene::PointLightProperty)})) {
        auto &light = lightNode.getProperty<Scene::PointLightProperty>();
        if (light.light.castShadows) {
            pointLightNodes.emplace_back(lightNode);
        }
    }

    TextureArrayBufferDesc desc;
    desc.textureDesc.size = resolution;
    desc.textureDesc.textureType = TEXTURE_CUBE_MAP;
    desc.textureDesc.format = DEPTH_STENCIL;
    desc.textureCount = pointLightNodes.size();

    pointLightShadowMapRes = builder.createTextureArrayBuffer(desc);

    builder.write(pointLightShadowMapRes);

    builder.assignSlot(SLOT_SHADOW_MAP_POINT, pointLightShadowMapRes);

    targetRes = builder.createRenderTarget(RenderTargetDesc{
            .size = resolution,
            .multisample = false,
            .samples = 0,
            .numberOfColorAttachments = 1,
            .hasDepthStencilAttachment = true,
    });
    builder.read(targetRes);

    if (!renderPipelineRes.assigned) {
        renderPipelineRes = builder.createPipeline(RenderPipelineDesc{
                .shaders = {{VERTEX,   shadowmappingpass_vs},
                            {FRAGMENT, shadowmappingpass_fs},
                            {GEOMETRY, shadowmappingpass_gs}},
                .bindings = {
                        BIND_SHADER_STORAGE_BUFFER,
                        BIND_SHADER_STORAGE_BUFFER,
                        BIND_SHADER_STORAGE_BUFFER,
                        BIND_SHADER_STORAGE_BUFFER,
                },
                .vertexLayout = SkinnedMesh::getDefaultVertexLayout(),
                /*       .clearColorValue = ColorRGBA(0, 0, 0, 0),
                       .clearColor = false,
                       .clearDepth = false,
                       .clearStencil = false,*/
                .enableDepthTest = true,
                .depthTestWrite = true,
                .depthTestMode = DEPTH_TEST_LESS,
                .enableFaceCulling = true,
                .enableBlending = false
        });
    }

    builder.persist(renderPipelineRes);
    builder.read(renderPipelineRes);

    if (!vertexArrayObjectRes.assigned) {
        vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                .vertexLayout = SkinnedMesh::getDefaultVertexLayout()
        });
    }

    builder.read(vertexArrayObjectRes);
    builder.write(vertexArrayObjectRes);
    builder.persist(vertexArrayObjectRes);

    renderPassRes = builder.createRenderPass(RenderPassDesc{
            .numberOfColorAttachments = 1,
            .hasDepthStencilAttachment = true
    });
    builder.read(renderPassRes);

    size_t totalShaderBufferSize = 0;
    size_t boneCount = 0;
    for (auto &node: builder.getScene().rootNode.findAll({typeid(Scene::SkinnedMeshProperty)})) {
        auto &meshProp = node.getProperty<Scene::SkinnedMeshProperty>();
        if (meshProp.mesh.assigned()) {
            meshAllocator.prepareMeshAllocation(meshProp.mesh);
            usedMeshes.insert(meshProp.mesh.getUri());

            for (auto i = 0; i < meshProp.mesh.get().subMeshes.size() + 1; i++) {
                auto &mesh = i == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(i - 1);

                if (node.hasProperty<Scene::ShadowProperty>()) {
                    if (!node.getProperty<Scene::ShadowProperty>().castShadows)
                        continue;
                }

                boneCount += mesh.bones.size();

                totalShaderBufferSize += sizeof(ShaderDrawData);
            }
            meshNodes.emplace_back(node);
        }
    }

    shaderBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
            .bufferType = RenderBufferType::HOST_VISIBLE,
            .size = totalShaderBufferSize
    });
    builder.write(shaderBufferRes);

    lightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
            .bufferType = HOST_VISIBLE,
            .size = sizeof(LightData)
    });
    builder.write(lightBufferRes);

    boneBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
            .bufferType = RenderBufferType::HOST_VISIBLE,
            .size = sizeof(Mat4f) * boneCount
    });
    builder.write(boneBufferRes);

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

    commandBufferRes = builder.createCommandBuffer();
    builder.write(commandBufferRes);

    TextureArrayBufferDesc texDesc;
    texDesc.textureDesc.size = resolution;
    texDesc.textureCount = 1;
    textureRes = builder.createTextureArrayBuffer(texDesc);

    builder.write(textureRes);
}

void xng::ShadowMappingPass::execute(FrameGraphPassResources &resources,
                                     const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                                     const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                                     const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) {

    auto &target = resources.get<RenderTarget>(targetRes);
    auto &pipeline = resources.get<RenderPipeline>(renderPipelineRes);
    auto &skinnedPipeline = resources.get<RenderPipeline>(renderPipelineRes);
    auto &pass = resources.get<RenderPass>(renderPassRes);

    auto &shaderBuffer = resources.get<ShaderStorageBuffer>(shaderBufferRes);
    auto &lightBuffer = resources.get<ShaderStorageBuffer>(lightBufferRes);
    auto &boneBuffer = resources.get<ShaderStorageBuffer>(boneBufferRes);
    auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);
    auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
    auto &indexBuffer = resources.get<IndexBuffer>(indexBufferRes);

    auto &cBuffer = resources.get<CommandBuffer>(commandBufferRes);

    auto &pointLightShadowMap = resources.get<TextureArrayBuffer>(pointLightShadowMapRes);

    auto &tex = resources.get<TextureArrayBuffer>(textureRes);

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


    // Draw Point Light Shadow Map
    target.setAttachments({RenderTargetAttachment::textureArrayLayered(tex)},
                          RenderTargetAttachment::textureArrayLayered(pointLightShadowMap));

    commands.emplace_back(pass.begin(target));
    commands.emplace_back(pass.clearDepthAttachment(1));
    commands.emplace_back(pass.end());

    cBuffer.begin();
    cBuffer.add(commands);
    cBuffer.end();

    renderQueues.at(0).get().submit(cBuffer);

    commands.clear();

    for (auto li = 0; li < pointLightNodes.size(); li++) {
        auto &lightNode = pointLightNodes.at(li);
        auto &light = lightNode.getProperty<Scene::PointLightProperty>().light;
        auto &transform = lightNode.getProperty<Scene::TransformProperty>().transform;
        float aspect = (float) resolution.x / (float) resolution.y;
        float near = nearPlane;
        float far = farPlane;

        Mat4f shadowProj = MatrixMath::perspective(90.0f, aspect, near, far);

        auto &lightPos = transform.getPosition();

        LightData lightData;

        lightData.shadowMatrices[0] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(1.0, 0.0, 0.0),
                                                          Vec3f(0.0, -1.0, 0.0)));
        lightData.shadowMatrices[1] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(-1.0, 0.0, 0.0),
                                                          Vec3f(0.0, -1.0, 0.0)));
        lightData.shadowMatrices[2] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(0.0, 1.0, 0.0),
                                                          Vec3f(0.0, 0.0, 1.0)));
        lightData.shadowMatrices[3] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(0.0, -1.0, 0.0),
                                                          Vec3f(0.0, 0.0, -1.0)));
        lightData.shadowMatrices[4] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(0.0, 0.0, 1.0),
                                                          Vec3f(0.0, -1.0, 0.0)));
        lightData.shadowMatrices[5] = (shadowProj *
                                       MatrixMath::lookAt(lightPos, lightPos + Vec3f(0.0, 0.0, -1.0),
                                                          Vec3f(0.0, -1.0, 0.0)));

        lightData.lightPosFarPlane = Vec4f(lightPos.x, lightPos.y, lightPos.z, farPlane).getMemory();

        lightData.layer[0] = static_cast<int>(li);

        lightBuffer.upload(0,
                           reinterpret_cast<const uint8_t *>(&lightData),
                           sizeof(LightData));

        if (!meshNodes.empty()) {
            std::vector<DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            std::vector<ShaderDrawData> shaderData;
            std::vector<Mat4f> boneMatrices;

            for (auto &node: meshNodes) {
                auto &meshProp = node.getProperty<Scene::SkinnedMeshProperty>();

                auto rig = meshProp.mesh.get().rig;

                std::map<std::string, Mat4f> boneTransforms;
                auto it = node.properties.find(typeid(Scene::BoneTransformsProperty));
                if (it != node.properties.end()) {
                    boneTransforms = it->second->get<Scene::BoneTransformsProperty>().boneTransforms;
                }

                std::map<size_t, ResourceHandle<Material>> mats;
                it = node.properties.find(typeid(Scene::MaterialProperty));
                if (it != node.properties.end()) {
                    mats = it->second->get<Scene::MaterialProperty>().materials;
                }

                auto drawData = meshAllocator.getAllocatedMesh(meshProp.mesh);

                for (auto mi = 0; mi < meshProp.mesh.get().subMeshes.size() + 1; mi++) {
                    auto model = node.getProperty<Scene::TransformProperty>().transform.model();

                    auto &mesh = mi == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(mi - 1);

                    auto material = mesh.material.get();

                    auto mIt = mats.find(mi);
                    if (mIt != mats.end()) {
                        material = mIt->second.get();
                    }

                    if (node.hasProperty<Scene::ShadowProperty>()) {
                        if (!node.getProperty<Scene::ShadowProperty>().castShadows)
                            continue;
                    }

                    auto boneOffset = boneMatrices.size();
                    if (mesh.bones.empty()) {
                        boneOffset = -1;
                    } else {
                        for (auto &bone: mesh.bones) {
                            auto bt = boneTransforms.find(bone);
                            if (bt != boneTransforms.end()) {
                                boneMatrices.emplace_back(bt->second);
                            } else {
                                boneMatrices.emplace_back(MatrixMath::identity());
                            }
                        }
                    }

                    auto data = ShaderDrawData();

                    data.model = model;
                    data.boneOffset[0] = static_cast<int>(boneOffset);

                    shaderData.emplace_back(data);

                    auto &draw = drawData.data.at(mi);
                    drawCalls.emplace_back(draw.drawCall);
                    baseVertices.emplace_back(draw.baseVertex);
                }
            }

            commands.emplace_back(pass.begin(target));
            commands.emplace_back(pass.setViewport({}, target.getDescription().size));
            commands.emplace_back(skinnedPipeline.bind());
            commands.emplace_back(vertexArrayObject.bind());

            shaderBuffer.upload(0,
                                reinterpret_cast<const uint8_t *>(shaderData.data()),
                                shaderData.size() * sizeof(ShaderDrawData));
            boneBuffer.upload(0,
                              reinterpret_cast<const uint8_t *>(boneMatrices.data()),
                              boneMatrices.size() * sizeof(Mat4f));

            auto shaderRes = std::vector<ShaderResource>{
                    {shaderBuffer, {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                    {boneBuffer,   {{VERTEX, ShaderResource::READ}}},
                    {lightBuffer,  {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
            };

            commands.emplace_back(RenderPipeline::bindShaderResources(shaderRes));
            commands.emplace_back(pass.multiDrawIndexed(drawCalls, baseVertices));
            commands.emplace_back(pass.end());

            cBuffer.begin();
            cBuffer.add(commands);
            cBuffer.end();

            renderQueues.at(0).get().submit(cBuffer);
        }
    }
    target.clearAttachments();
}

std::type_index xng::ShadowMappingPass::getTypeIndex() const {
    return typeid(ShadowMappingPass);
}

float xng::ShadowMappingPass::getNearPlane() const {
    return nearPlane;
}

void xng::ShadowMappingPass::setNearPlane(float nearPlane) {
    ShadowMappingPass::nearPlane = nearPlane;
}

float xng::ShadowMappingPass::getFarPlane() const {
    return farPlane;
}

void xng::ShadowMappingPass::setFarPlane(float farPlane) {
    ShadowMappingPass::farPlane = farPlane;
}
