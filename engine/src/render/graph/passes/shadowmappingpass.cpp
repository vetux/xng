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
        resolution = builder.getSettings().get<Vec2i>(FrameGraphSettings::SETTING_SHADOW_MAPPING_POINT_RESOLUTION);

        std::vector<Node> meshNodes;
        std::vector<Node> pointLightNodes;

        for (auto &lightNode: builder.getScene().rootNode.findAll({typeid(PointLightProperty)})) {
            auto &light = lightNode.getProperty<PointLightProperty>();
            if (light.light.castShadows) {
                pointLightNodes.emplace_back(lightNode);
            }
        }

        TextureArrayBufferDesc desc;
        desc.textureDesc.size = resolution;
        desc.textureDesc.textureType = TEXTURE_CUBE_MAP;
        desc.textureDesc.format = DEPTH_STENCIL;
        desc.textureCount = pointLightNodes.size();

        auto pointLightShadowMapRes = builder.createTextureArrayBuffer(desc);

        builder.assignSlot(SLOT_SHADOW_MAP_POINT, pointLightShadowMapRes);

        if (!renderPipelineRes.assigned) {
            renderPipelineRes = builder.createRenderPipeline(RenderPipelineDesc{
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

        if (!vertexArrayObjectRes.assigned) {
            vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                    .vertexLayout = SkinnedMesh::getDefaultVertexLayout()
            });
        }

        builder.persist(vertexArrayObjectRes);

        std::set<Uri> usedMeshes;

        size_t totalShaderBufferSize = 0;
        size_t boneCount = 0;
        for (auto &node: builder.getScene().rootNode.findAll({typeid(SkinnedMeshProperty)})) {
            auto &meshProp = node.getProperty<SkinnedMeshProperty>();
            if (meshProp.mesh.assigned()) {
                meshAllocator.prepareMeshAllocation(meshProp.mesh);
                usedMeshes.insert(meshProp.mesh.getUri());

                for (auto i = 0; i < meshProp.mesh.get().subMeshes.size() + 1; i++) {
                    auto &mesh = i == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(i - 1);

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

        auto shaderBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = totalShaderBufferSize
        });

        auto lightBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = HOST_VISIBLE,
                .size = sizeof(LightData)
        });

        auto boneBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = sizeof(Mat4f) * boneCount
        });

        if (vertexBufferRes.assigned) {
            builder.persist(vertexBufferRes);
        }

        if (indexBufferRes.assigned) {
            builder.persist(indexBufferRes);
        }

        if (!vertexBufferRes.assigned || currentVertexBufferSize < meshAllocator.getRequestedVertexBufferSize()) {
            staleVertexBuffer = vertexBufferRes;
            auto d = VertexBufferDesc();
            d.size = meshAllocator.getRequestedVertexBufferSize();
            vertexBufferRes = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.persist(vertexBufferRes);
        }

        if (!indexBufferRes.assigned || currentIndexBufferSize < meshAllocator.getRequestedIndexBufferSize()) {
            staleIndexBuffer = indexBufferRes;
            auto d = IndexBufferDesc();
            d.size = meshAllocator.getRequestedIndexBufferSize();
            indexBufferRes = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.persist(indexBufferRes);
        }

        TextureArrayBufferDesc texDesc;
        texDesc.textureDesc.size = resolution;
        texDesc.textureCount = 1;
        auto textureRes = builder.createTextureArrayBuffer(texDesc);

        bool updateVao = false;
        if (staleVertexBuffer.assigned) {
            builder.copy(staleVertexBuffer, vertexBufferRes);
            staleVertexBuffer = {};
            updateVao = true;
        }

        if (staleIndexBuffer.assigned) {
            builder.copy(staleIndexBuffer, indexBufferRes);
            staleIndexBuffer = {};
            updateVao = true;
        }

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

        builder.clearTextureFloat({pointLightShadowMapRes});

        for (auto li = 0; li < pointLightNodes.size(); li++) {
            auto &lightNode = pointLightNodes.at(li);
            auto &light = lightNode.getProperty<PointLightProperty>().light;
            auto &transform = lightNode.getProperty<TransformProperty>().transform;
            float aspect = (float) resolution.x / (float) resolution.y;
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

            builder.upload(lightBufferRes,
                           [lightData]() {
                               return FrameGraphCommand::UploadBuffer(sizeof(LightData),
                                                                      reinterpret_cast<const uint8_t *>(&lightData));
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

                    std::map<size_t, ResourceHandle<Material>> mats;
                    it = node.properties.find(typeid(MaterialProperty));
                    if (it != node.properties.end()) {
                        mats = it->second->get<MaterialProperty>().materials;
                    }

                    auto drawData = meshAllocator.getAllocatedMesh(meshProp.mesh);

                    for (auto mi = 0; mi < meshProp.mesh.get().subMeshes.size() + 1; mi++) {
                        auto model = node.getProperty<TransformProperty>().transform.model();

                        auto &mesh = mi == 0 ? meshProp.mesh.get() : meshProp.mesh.get().subMeshes.at(mi - 1);

                        auto material = mesh.material.get();

                        auto mIt = mats.find(mi);
                        if (mIt != mats.end()) {
                            material = mIt->second.get();
                        }

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

                builder.beginPass({FrameGraphCommand::Attachment::textureArrayLayered(textureRes)},
                                  FrameGraphCommand::Attachment::textureArrayLayered(pointLightShadowMapRes));
                builder.setViewport({}, resolution);
                builder.bindPipeline(renderPipelineRes);
                builder.bindVertexArrayObject(vertexArrayObjectRes);

                builder.upload(shaderBufferRes,
                               [shaderData]() {
                                   return FrameGraphCommand::UploadBuffer(shaderData.size() * sizeof(ShaderDrawData),
                                                                          reinterpret_cast<const uint8_t *>(shaderData.data()));
                               });
                builder.upload(boneBufferRes,
                               [boneMatrices]() {
                                   return FrameGraphCommand::UploadBuffer(boneMatrices.size() * sizeof(ShaderDrawData),
                                                                          reinterpret_cast<const uint8_t *>(boneMatrices.data()));
                               });
                builder.bindShaderResources({
                                                    {shaderBufferRes, {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
                                                    {boneBufferRes,   {{VERTEX, ShaderResource::READ}}},
                                                    {lightBufferRes,  {{VERTEX, ShaderResource::READ}, {FRAGMENT, ShaderResource::READ}}},
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