/**
 *  This file is part of xEngine, a C++ game engine library.
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

    struct ShaderAtlasTexture {
        int level_index_filtering_assigned[4]{0, 0, 0, 0};
        float atlasScale_texSize[4]{0, 0, 0, 0};
    };

    struct ShaderDrawData {
        Mat4f model;
        Mat4f mvp;

        int shadeModel_objectID[4]{0, 0, 0, 0};
        float albedoColor[4]{0, 0, 0, 0};
        float metallic_roughness_ambientOcclusion_shininess[4]{0, 0, 0, 0};

        float diffuseColor[4]{0, 0, 0, 0};
        float ambientColor[4]{0, 0, 0, 0};
        float specularColor[4]{0, 0, 0, 0};

        ShaderAtlasTexture normal;

        ShaderAtlasTexture albedo;
        ShaderAtlasTexture metallic;
        ShaderAtlasTexture roughness;
        ShaderAtlasTexture ambientOcclusion;

        ShaderAtlasTexture diffuse;
        ShaderAtlasTexture ambient;
        ShaderAtlasTexture specular;
        ShaderAtlasTexture shininess;
    };
#pragma pack(pop)

    static std::vector<DirectionalLightData> getDirLights(const std::vector<DirectionalLight> &lights) {
        std::vector<DirectionalLightData> ret;
        for (auto &l: lights) {
            auto tmp = DirectionalLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            auto euler = (Quaternion(l.direction) * l.transform.getRotation()).getEulerAngles();
            tmp.direction = Vec4f(euler.x, euler.y, euler.z, 0).getMemory();
            ret.emplace_back();
        }
        return ret;
    }

    static std::vector<PointLightData> getPointLights(const std::vector<PointLight> &lights) {
        std::vector<PointLightData> ret;
        for (auto &l: lights) {
            auto tmp = PointLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            tmp.position = Vec4f(l.transform.getPosition().x,
                                 l.transform.getPosition().y,
                                 l.transform.getPosition().z,
                                 0).getMemory();
            tmp.constant_linear_quadratic[0] = l.constant;
            tmp.constant_linear_quadratic[1] = l.linear;
            tmp.constant_linear_quadratic[2] = l.quadratic;
            ret.emplace_back(tmp);
        }
        return ret;
    }

    static std::vector<SpotLightData> getSpotLights(const std::vector<SpotLight> &lights) {
        std::vector<SpotLightData> ret;
        for (auto &l: lights) {
            auto tmp = SpotLightData{
                    .ambient = Vec4f(l.ambient.x, l.ambient.y, l.ambient.z, 1).getMemory(),
                    .diffuse = Vec4f(l.diffuse.x, l.diffuse.y, l.diffuse.z, 1).getMemory(),
                    .specular = Vec4f(l.specular.x, l.specular.y, l.specular.z, 1).getMemory(),
            };
            tmp.position = Vec4f(l.transform.getPosition().x,
                                 l.transform.getPosition().y,
                                 l.transform.getPosition().z,
                                 0).getMemory();

            auto euler = (Quaternion(l.direction) * l.transform.getRotation()).getEulerAngles();

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

    void ForwardLightingPass::setup(FrameGraphBuilder &builder) {
        renderSize = builder.getRenderSize();

        pointLights = builder.getScene().pointLights;
        spotLights = builder.getScene().spotLights;
        directionalLights = builder.getScene().directionalLights;

        pointLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLightData) * pointLights.size()
        });
        builder.read(pointLightsBufferRes);
        builder.write(pointLightsBufferRes);

        spotLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLightData) * spotLights.size()
        });
        builder.read(spotLightsBufferRes);
        builder.write(spotLightsBufferRes);

        directionalLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLightData) * directionalLights.size()
        });
        builder.read(directionalLightsBufferRes);
        builder.write(directionalLightsBufferRes);

        requestedVertexBufferSize = currentVertexBufferSize;
        requestedIndexBufferSize = currentIndexBufferSize;

        RenderTargetDesc targetDesc;
        targetDesc.size = builder.getRenderSize();
        targetDesc.numberOfColorAttachments = 1;
        targetDesc.hasDepthStencilAttachment = true;
        targetRes = builder.createRenderTarget(targetDesc);

        builder.read(targetRes);

        if (!pipelineRes.assigned) {
            pipelineRes = builder.createPipeline(RenderPipelineDesc{
                    .shaders = {{VERTEX,   forwardlightingpass_vs.getShader()},
                                {FRAGMENT, forwardlightingpass_fs.getShader()}},
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
                    },
                    .vertexLayout = Mesh::getDefaultVertexLayout(),
                    .clearColorValue = ColorRGBA(0, 0, 0, 0),
                    .clearColor = false,
                    .clearDepth = false,
                    .clearStencil = false,
                    .enableDepthTest = true,
                    .depthTestWrite = true,
                    .depthTestMode = DEPTH_TEST_LESS,
                    .enableFaceCulling = true,
                    .enableBlending = true,
                    .alphaBlendEquation = BLEND_MAX
            });
        }

        builder.persist(pipelineRes);
        builder.read(pipelineRes);

        if (!vertexArrayObjectRes.assigned) {
            vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                    .vertexLayout = Mesh::getDefaultVertexLayout()
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

        objects.clear();

        size_t totalShaderBufferSize = sizeof(float[8]);

        usedTextures.clear();
        usedMeshes.clear();
        auto &tmp = builder.getScene().objects;
        for (auto id = 0; id < tmp.size(); id++) {
            auto &object = tmp.at(id);
            if (object.mesh.assigned()) {
                if (object.material.assigned()) {
                    auto &mat = object.material.get();
                    if (!mat.transparent || mat.shader.assigned())
                        continue;

                    usedMeshes.insert(object.mesh.getUri());

                    prepareMeshAllocation(object.mesh);

                    usedTextures.insert(mat.normal.getUri());
                    usedTextures.insert(mat.albedoTexture.getUri());
                    usedTextures.insert(mat.metallicTexture.getUri());
                    usedTextures.insert(mat.roughnessTexture.getUri());
                    usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                    usedTextures.insert(mat.diffuseTexture.getUri());
                    usedTextures.insert(mat.ambientTexture.getUri());
                    usedTextures.insert(mat.specularTexture.getUri());
                    usedTextures.insert(mat.shininessTexture.getUri());

                    if (mat.normal.assigned()
                        && textures.find(mat.normal.getUri()) == textures.end()) {
                        textures[mat.normal.getUri()] = atlas.add(mat.normal.get().getImage().get());
                    }
                    if (mat.albedoTexture.assigned()
                        && textures.find(mat.albedoTexture.getUri()) == textures.end()) {
                        textures[mat.albedoTexture.getUri()] = atlas.add(mat.albedoTexture.get().getImage().get());
                    }
                    if (mat.metallicTexture.assigned()
                        && textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                        textures[mat.metallicTexture.getUri()] = atlas.add(mat.metallicTexture.get().getImage().get());
                    }
                    if (mat.roughnessTexture.assigned()
                        && textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                        textures[mat.roughnessTexture.getUri()] = atlas.add(
                                mat.roughnessTexture.get().getImage().get());
                    }
                    if (mat.ambientOcclusionTexture.assigned()
                        && textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                        textures[mat.ambientOcclusionTexture.getUri()] = atlas.add(
                                mat.ambientOcclusionTexture.get().getImage().get());
                    }
                    if (mat.diffuseTexture.assigned()
                        && textures.find(mat.diffuseTexture.getUri()) == textures.end()) {
                        textures[mat.diffuseTexture.getUri()] = atlas.add(mat.diffuseTexture.get().getImage().get());
                    }
                    if (mat.ambientTexture.assigned()
                        && textures.find(mat.ambientTexture.getUri()) == textures.end()) {
                        textures[mat.ambientTexture.getUri()] = atlas.add(mat.ambientTexture.get().getImage().get());
                    }
                    if (mat.specularTexture.assigned()
                        && textures.find(mat.specularTexture.getUri()) == textures.end()) {
                        textures[mat.specularTexture.getUri()] = atlas.add(mat.specularTexture.get().getImage().get());
                    }
                    if (mat.shininessTexture.assigned()
                        && textures.find(mat.shininessTexture.getUri()) == textures.end()) {
                        textures[mat.shininessTexture.getUri()] = atlas.add(
                                mat.shininessTexture.get().getImage().get());
                    }

                    totalShaderBufferSize += sizeof(ShaderDrawData);

                    objects.emplace_back(object);
                }
            }
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

        mergeFreeVertexBufferRanges();
        mergeFreeIndexBufferRanges();

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

        if (!vertexBufferRes.assigned || currentVertexBufferSize < requestedVertexBufferSize) {
            staleVertexBuffer = vertexBufferRes;
            auto d = VertexBufferDesc();
            d.size = requestedVertexBufferSize;
            vertexBufferRes = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.read(vertexBufferRes);
            builder.write(vertexBufferRes);
            builder.persist(vertexBufferRes);
        }

        if (!indexBufferRes.assigned || currentIndexBufferSize < requestedIndexBufferSize) {
            staleIndexBuffer = indexBufferRes;
            auto d = IndexBufferDesc();
            d.size = requestedIndexBufferSize;
            indexBufferRes = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.read(indexBufferRes);
            builder.write(indexBufferRes);
            builder.persist(indexBufferRes);
        }

        camera = builder.getScene().camera;
        cameraTransform = builder.getScene().cameraTransform;

        forwardColorRes = builder.getSlot(SLOT_FORWARD_COLOR);
        forwardDepthRes = builder.getSlot(SLOT_FORWARD_DEPTH);
        deferredDepthRes = builder.getSlot(SLOT_DEFERRED_DEPTH);

        builder.write(forwardColorRes);
        builder.write(forwardDepthRes);
        builder.read(deferredDepthRes);
    }

    void ForwardLightingPass::execute(FrameGraphPassResources &resources) {
        auto atlasBuffers = atlas.getAtlasBuffers(resources);

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

        auto plights = getPointLights(pointLights);
        auto slights = getSpotLights(spotLights);
        auto dlights = getDirLights(directionalLights);

        pointLightBuffer.upload(reinterpret_cast<const uint8_t *>(plights.data()),
                                plights.size() * sizeof(PointLightData));
        spotLightBuffer.upload(reinterpret_cast<const uint8_t *>(slights.data()),
                               slights.size() * sizeof(SpotLightData));
        dirLightBuffer.upload(reinterpret_cast<const uint8_t *>(dlights.data()),
                              dlights.size() * sizeof(DirectionalLightData));

        bool updateVao = false;
        if (staleVertexBuffer.assigned) {
            auto &staleBuffer = resources.get<VertexBuffer>(staleVertexBuffer);
            vertexBuffer.copy(staleBuffer);
            staleVertexBuffer = {};
            updateVao = true;
        }

        if (staleIndexBuffer.assigned) {
            auto &staleBuffer = resources.get<IndexBuffer>(staleIndexBuffer);
            indexBuffer.copy(staleBuffer);
            staleIndexBuffer = {};
            updateVao = true;
        }

        if (updateVao || bindVao) {
            bindVao = false;
            vertexArrayObject.bindBuffers(vertexBuffer, indexBuffer);
        }

        allocateMeshes(vertexBuffer, indexBuffer);

        // Deallocate unused meshes
        std::set<Uri> dealloc;
        for (auto &pair: meshAllocations) {
            if (usedMeshes.find(pair.first) == usedMeshes.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            deallocateMesh(ResourceHandle<Mesh>(uri));
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
        target.setColorAttachments({forwardColor});

        target.setDepthStencilAttachment(forwardDepth);

        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        pass.beginRenderPass(target, {}, target.getDescription().size);
        pass.bindPipeline(pipeline);
        pass.bindVertexArrayObject(vertexArrayObject);

        pass.clearColorAttachments(ColorRGBA(0));
        pass.clearDepthAttachment(1);

        if (!objects.empty()) {
            auto passesPerDrawCycle = objects.size() / drawCycles;

            for (auto i = 0; i < drawCycles; i++) {
                std::vector<RenderPass::DrawCall> drawCalls;
                std::vector<size_t> baseVertices;
                std::vector<ShaderDrawData> shaderData;
                for (auto oi = 0; oi < passesPerDrawCycle && oi < objects.size(); oi++) {
                    auto &object = objects.at(oi + (i * passesPerDrawCycle));
                    auto model = object.transform.model();
                    auto &material = object.material.get();

                    auto data = ShaderDrawData();

                    data.model = model;
                    data.mvp = projection * view * model;
                    data.shadeModel_objectID[0] = material.shadingModel;
                    data.shadeModel_objectID[1] = static_cast<int>(oi);

                    auto col = material.albedo.divide().getMemory();
                    data.albedoColor[0] = col[0];
                    data.albedoColor[1] = col[1];
                    data.albedoColor[2] = col[2];
                    data.albedoColor[3] = col[3];

                    data.metallic_roughness_ambientOcclusion_shininess[0] = material.metallic;
                    data.metallic_roughness_ambientOcclusion_shininess[1] = material.roughness;
                    data.metallic_roughness_ambientOcclusion_shininess[2] = material.ambientOcclusion;
                    data.metallic_roughness_ambientOcclusion_shininess[3] = material.shininess;

                    col = material.diffuse.divide().getMemory();
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

                    auto mesh = getMesh(object.mesh);

                    drawCalls.emplace_back(mesh.drawCall);
                    baseVertices.emplace_back(mesh.baseVertex);
                }
                auto viewPos = cameraTransform.getPosition();
                float viewArr[4] = {viewPos.x, viewPos.y, viewPos.z, 1};
                float viewSize[4] = {static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0, 0};
                shaderBuffer.upload(0, reinterpret_cast<const uint8_t *>(viewArr), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[4]), reinterpret_cast<const uint8_t *>(viewSize), sizeof(float[4]));
                shaderBuffer.upload(sizeof(float[8]),
                                    reinterpret_cast<const uint8_t *>(shaderData.data()),
                                    shaderData.size() * sizeof(ShaderDrawData));
                pass.bindShaderData({shaderBuffer,
                                     atlasBuffers.at(TEXTURE_ATLAS_8x8),
                                     atlasBuffers.at(TEXTURE_ATLAS_16x16),
                                     atlasBuffers.at(TEXTURE_ATLAS_32x32),
                                     atlasBuffers.at(TEXTURE_ATLAS_64x64),
                                     atlasBuffers.at(TEXTURE_ATLAS_128x128),
                                     atlasBuffers.at(TEXTURE_ATLAS_256x256),
                                     atlasBuffers.at(TEXTURE_ATLAS_512x512),
                                     atlasBuffers.at(TEXTURE_ATLAS_1024x1024),
                                     atlasBuffers.at(TEXTURE_ATLAS_2048x2048),
                                     atlasBuffers.at(TEXTURE_ATLAS_4096x4096),
                                     atlasBuffers.at(TEXTURE_ATLAS_8192x8192),
                                     atlasBuffers.at(TEXTURE_ATLAS_16384x16384),
                                     deferredDepth,
                                     pointLightBuffer,
                                     spotLightBuffer,
                                     dirLightBuffer});
                pass.multiDrawIndexed(drawCalls, baseVertices);
            }
        }

        pass.endRenderPass();

        target.setColorAttachments({});
        target.clearDepthStencilAttachment();
    }

    std::type_index ForwardLightingPass::getTypeIndex() const {
        return typeid(ForwardLightingPass);
    }

    TextureAtlasHandle ForwardLightingPass::getTexture(const ResourceHandle<Texture> &texture,
                                                       std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers) {
        if (textures.find(texture.getUri()) == textures.end()) {
            auto handle = atlas.add(texture.get().getImage().get());
            textures[texture.getUri()] = handle;
        }
        return textures.at(texture.getUri());
    }

    ForwardLightingPass::MeshDrawData ForwardLightingPass::getMesh(const ResourceHandle<Mesh> &mesh) {
        return meshAllocations.at(mesh.getUri());
    }

    void ForwardLightingPass::prepareMeshAllocation(const ResourceHandle<Mesh> &mesh) {
        if (mesh.get().primitive != TRIANGLES) {
            throw std::runtime_error("Unsupported mesh primitive");
        } else if (mesh.get().vertexLayout != Mesh::getDefaultVertexLayout()) {
            throw std::runtime_error("Unsupported mesh vertex layout");
        } else if (mesh.get().indices.empty()) {
            throw std::runtime_error("Arrayed mesh not supported, must be indexed");
        }
        if (meshAllocations.find(mesh.getUri()) == meshAllocations.end()
            && pendingMeshAllocations.find(mesh.getUri()) == pendingMeshAllocations.end()) {
            MeshDrawData data;
            data.primitive = mesh.get().primitive;
            data.drawCall.count = mesh.get().indices.size();
            data.drawCall.offset = allocateIndexData(mesh.get().indices.size() * sizeof(unsigned int));
            data.baseVertex = allocateVertexData(mesh.get().vertices.size() * mesh.get().vertexLayout.getSize())
                              / mesh.get().vertexLayout.getSize();
            pendingMeshAllocations[mesh.getUri()] = data;
            pendingMeshHandles[mesh.getUri()] = mesh;
        }
    }

    void ForwardLightingPass::allocateMeshes(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer) {
        for (auto &pair: pendingMeshAllocations) {
            auto mesh = pendingMeshHandles.at(pair.first);
            auto buf = VertexStream().addVertices(mesh.get().vertices).getVertexBuffer();
            indexBuffer.upload(pair.second.drawCall.offset,
                               reinterpret_cast<const uint8_t *>(mesh.get().indices.data()),
                               mesh.get().indices.size() * sizeof(unsigned int));
            vertexBuffer.upload(pair.second.baseVertex * mesh.get().vertexLayout.getSize(),
                                buf.data(),
                                buf.size());
            meshAllocations[pair.first] = pair.second;
        }
        pendingMeshAllocations.clear();
        pendingMeshHandles.clear();
    }

    void ForwardLightingPass::deallocateMesh(const ResourceHandle<Mesh> &mesh) {
        auto alloc = meshAllocations.at(mesh.getUri());
        meshAllocations.erase(mesh.getUri());
        deallocateVertexData(alloc.baseVertex * mesh.get().vertexLayout.getSize());
        deallocateIndexData(alloc.drawCall.offset);
    }

    void ForwardLightingPass::deallocateTexture(const ResourceHandle<Texture> &texture) {
        atlas.remove(textures.at(texture.getUri()));
        textures.erase(texture.getUri());
    }

    size_t ForwardLightingPass::allocateVertexData(size_t size) {
        assert(size % Mesh::getDefaultVertexLayout().getSize() == 0);

        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeVertexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }

        if (foundFreeRange) {
            auto rangeSize = freeVertexBufferRanges.at(ret) -= size;
            freeVertexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeVertexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedVertexBufferSize;
            requestedVertexBufferSize += size;
        }

        allocatedVertexRanges[ret] = size;

        return ret;
    }

    void ForwardLightingPass::deallocateVertexData(size_t offset) {
        auto size = allocatedVertexRanges.at(offset);
        allocatedVertexRanges.erase(offset);
        freeVertexBufferRanges[offset] = size;
    }

    size_t ForwardLightingPass::allocateIndexData(size_t size) {
        assert(size % sizeof(unsigned int) == 0);

        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeIndexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }
        if (foundFreeRange) {
            auto rangeSize = freeIndexBufferRanges.at(ret) -= size;
            freeIndexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeIndexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedIndexBufferSize;
            requestedIndexBufferSize += size;
        }

        allocatedIndexRanges[ret] = size;

        return ret;
    }

    void ForwardLightingPass::deallocateIndexData(size_t offset) {
        auto size = allocatedIndexRanges.at(offset);
        allocatedIndexRanges.erase(offset);
        freeIndexBufferRanges[offset] = size;
    }

    void ForwardLightingPass::mergeFreeVertexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto vertexRanges = freeVertexBufferRanges;
            for (auto range = freeVertexBufferRanges.begin(); range != freeVertexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeVertexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && vertexRanges.find(range->first) != vertexRanges.end()
                        && vertexRanges.find(next->first) != vertexRanges.end()) {
                        merged = true;
                        vertexRanges.at(range->first) += next->second;
                        vertexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeVertexBufferRanges = vertexRanges;
        }
    }

    void ForwardLightingPass::mergeFreeIndexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto indexRanges = freeIndexBufferRanges;
            for (auto range = freeIndexBufferRanges.begin(); range != freeIndexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeIndexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && indexRanges.find(range->first) != indexRanges.end()
                        && indexRanges.find(next->first) != indexRanges.end()) {
                        merged = true;
                        indexRanges.at(range->first) += next->second;
                        indexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeIndexBufferRanges = indexRanges;
        }
    }
}