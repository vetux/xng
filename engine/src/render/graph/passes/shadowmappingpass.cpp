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

#include "xng/render/graph/framegraphsettings.hpp"

#include "graph/shadowmappingpass_fs.hpp"
#include "graph/shadowmappingpass_vs.hpp"
#include "graph/shadowmappingpass_gs.hpp"

#include "xng/math/rotation.hpp"

//TODO: Implement Directional and Spot light shadow mapping

struct ShaderDrawData {
    std::array<int, 4> boneOffset{};
    Mat4f model;
};

struct LightData {
    std::array<float, 4> lightPosFarPlane{};
    std::array<int, 4> layer{};
    std::array<Mat4f, 6> shadowMatrices;
};

namespace xng {
    void ShadowMappingPass::setup(FrameGraphBuilder &builder) {
        auto pointShadowResolution = builder.getSettings().get<Vec2i>(FrameGraphSettings::SETTING_SHADOW_MAPPING_POINT_RESOLUTION);

        std::vector<Node> meshNodes;
        std::vector<Node> pointLightNodes;

        for (auto &lightNode: builder.getScene().rootNode.findAll({typeid(PointLightProperty)})) {
            auto &light = lightNode.getProperty<PointLightProperty>();
            if (light.light.castShadows) {
                pointLightNodes.emplace_back(lightNode);
            }
        }

        TextureArrayBufferDesc desc;
        desc.textureDesc.size = pointShadowResolution;
        desc.textureDesc.textureType = TEXTURE_CUBE_MAP;
        desc.textureDesc.format = DEPTH_STENCIL;
        desc.textureCount = pointLightNodes.size();

        auto pointLightShadowMap = builder.createTextureArrayBuffer(desc);

        builder.assignSlot(SLOT_SHADOW_MAP_POINT, pointLightShadowMap);

        if (!renderPipeline.assigned) {
            renderPipeline = builder.createRenderPipeline(RenderPipelineDesc{
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
                    .enableDepthTest = true,
                    .depthTestWrite = true,
                    .depthTestMode = DEPTH_TEST_LESS,
                    .enableFaceCulling = true,
                    .enableBlending = false
            });
        }

        builder.persist(renderPipeline);

        std::set<Uri> usedMeshes;

        size_t totalShaderBufferSize = 0;
        size_t boneCount = 0;
        for (auto &node: builder.getScene().rootNode.findAll({typeid(SkinnedMeshProperty)})) {
            auto &meshProp = node.getProperty<SkinnedMeshProperty>();
            if (meshProp.mesh.assigned()) {
                meshAllocator.prepareMeshAllocation(meshProp.mesh);
                usedMeshes.insert(meshProp.mesh.getUri());

                for (auto i = 0; i < meshProp.mesh.get().subMeshes.size() + 1; i++) {
                    Mesh mesh = i == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(i - 1);

                    if (node.hasProperty<ShadowProperty>()) {
                        if (!node.getProperty<ShadowProperty>().castShadows)
                            continue;
                    }

                    boneCount += mesh.bones.size();

                    totalShaderBufferSize += sizeof(ShaderDrawData);
                }
                meshNodes.emplace_back(node);
            }
        }

        auto shaderBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = totalShaderBufferSize
        });

        auto lightBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = HOST_VISIBLE,
                .size = sizeof(LightData)
        });

        auto boneBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = sizeof(Mat4f) * boneCount
        });

        if (vertexBuffer.assigned) {
            builder.persist(vertexBuffer);
        }

        if (indexBuffer.assigned) {
            builder.persist(indexBuffer);
        }

        auto copyVertexSize = currentVertexBufferSize;

        if (!vertexBuffer.assigned || currentVertexBufferSize < meshAllocator.getRequestedVertexBufferSize()) {
            staleVertexBuffer = vertexBuffer;
            auto d = VertexBufferDesc();
            d.size = meshAllocator.getRequestedVertexBufferSize();
            vertexBuffer = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.persist(vertexBuffer);
        }

        auto copyIndexSize = currentVertexBufferSize;

        if (!indexBuffer.assigned || currentIndexBufferSize < meshAllocator.getRequestedIndexBufferSize()) {
            staleIndexBuffer = indexBuffer;
            auto d = IndexBufferDesc();
            d.size = meshAllocator.getRequestedIndexBufferSize();
            indexBuffer = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.persist(indexBuffer);
        }

        TextureArrayBufferDesc texDesc;
        texDesc.textureDesc.size = pointShadowResolution;
        texDesc.textureCount = 1;
        auto textureRes = builder.createTextureArrayBuffer(texDesc);

        if (staleVertexBuffer.assigned) {
            builder.copy(staleVertexBuffer, vertexBuffer, 0, 0, copyVertexSize);
            staleVertexBuffer = {};
        }

        if (staleIndexBuffer.assigned) {
            builder.copy(staleIndexBuffer, indexBuffer, 0, 0, copyIndexSize);
            staleIndexBuffer = {};
        }

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

        builder.beginPass({}, FrameGraphAttachment::textureArrayLayered(pointLightShadowMap));
        builder.clearDepth(1);
        builder.finishPass();

        for (auto li = 0; li < pointLightNodes.size(); li++) {
            auto &lightNode = pointLightNodes.at(li);
            auto &light = lightNode.getProperty<PointLightProperty>().light;
            auto &transform = lightNode.getProperty<TransformProperty>().transform;
            float aspect = (float) pointShadowResolution.x / (float) pointShadowResolution.y;
            float near = light.shadowNearPlane;
            float far = light.shadowFarPlane;

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

            lightData.lightPosFarPlane = Vec4f(lightPos.x, lightPos.y, lightPos.z, far).getMemory();

            lightData.layer[0] = static_cast<int>(li);

            builder.upload(lightBuffer,
                           [lightData]() {
                               return FrameGraphUploadBuffer::createValue(lightData);
                           });

            if (!meshNodes.empty()) {
                std::vector<DrawCall> drawCalls;
                std::vector<size_t> baseVertices;
                std::vector<ShaderDrawData> shaderData;
                std::vector<Mat4f> boneMatrices;

                for (auto &node: meshNodes) {
                    auto &meshProp = node.getProperty<SkinnedMeshProperty>();

                    auto rig = meshProp.mesh.get().rig;

                    std::map<std::string, Mat4f> boneTransforms;
                    auto it = node.properties.find(typeid(BoneTransformsProperty));
                    if (it != node.properties.end()) {
                        boneTransforms = it->second->get<BoneTransformsProperty>().boneTransforms;
                    }

                    auto drawData = meshAllocator.getAllocatedMesh(meshProp.mesh);

                    for (auto mi = 0; mi < meshProp.mesh.get().subMeshes.size() + 1; mi++) {
                        auto model = node.getProperty<TransformProperty>().transform.model();

                        Mesh mesh = mi == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(mi - 1);

                        if (node.hasProperty<ShadowProperty>()) {
                            if (!node.getProperty<ShadowProperty>().castShadows)
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

                builder.beginPass({FrameGraphAttachment::textureArrayLayered(textureRes)},
                                  FrameGraphAttachment::textureArrayLayered(pointLightShadowMap));
                builder.setViewport({}, pointShadowResolution);
                builder.bindPipeline(renderPipeline);
                builder.bindVertexBuffers(vertexBuffer, indexBuffer, {}, SkinnedMesh::getDefaultVertexLayout(), {});

                builder.upload(shaderBuffer,
                               [shaderData]() {
                                   return FrameGraphUploadBuffer::createArray(shaderData);
                               });
                builder.upload(boneBuffer,
                               [boneMatrices]() {
                                   return FrameGraphUploadBuffer::createArray(boneMatrices);
                               });

                builder.bindShaderResources({
                                                    {shaderBuffer, {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                                                    {boneBuffer,   {{VERTEX, ShaderResource::READ}}},
                                                    {lightBuffer,  {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                                            });

                builder.multiDrawIndexed(drawCalls, baseVertices);

                builder.finishPass();
            }
        }
    }

    std::type_index ShadowMappingPass::getTypeIndex() const {
        return typeid(ShadowMappingPass);
    }
}