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

#include "xng/graphics/passes/constructionpass.hpp"

namespace xng {
    void ConstructionPass::create(RenderGraphBuilder &builder) {
        // Create pipelines
        auto vs = createVertexShader();
        auto fs = createFragmentShader();
        auto vs_skinned = createSkinnedVertexShader();

        RenderGraphPipeline pipelineDesc;
        pipelineDesc.enableFaceCulling = true;
        pipelineDesc.enableDepthTest = true;
        pipelineDesc.depthTestWrite = true;
        pipelineDesc.shaders = {vs_skinned, fs};

        renderPipelineSkinned = builder.createPipeline(pipelineDesc);

        // Create GBuffer Textures
        currentResolution = builder.getBackBufferSize() * config->getRenderScale();

        auto desc = RenderGraphTextureBuffer();
        desc.size = currentResolution;
        desc.format = RGBA32F;
        gBuffer.gBufferPosition = builder.createTexture(desc);
        gBuffer.gBufferNormal = builder.createTexture(desc);
        gBuffer.gBufferTangent = builder.createTexture(desc);
        gBuffer.gBufferRoughnessMetallicAmbientOcclusion = builder.createTexture(desc);

        desc.format = RGBA;
        gBuffer.gBufferAlbedo = builder.createTexture(desc);

        desc.format = RGBA32I;
        gBuffer.gBufferObjectShadows = builder.createTexture(desc);

        desc.format = DEPTH_STENCIL;
        gBuffer.gBufferDepth = builder.createTexture(desc);

        shaderBuffer = builder.createShaderBuffer(sizeof(BufferLayout::CPU));
        boneBuffer = builder.createShaderBuffer(0);

        textureAtlas.onCreate(builder);

        registry->set(gBuffer);

        auto pass = builder.addPass("ConstructionPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, renderPipelineSkinned);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    void ConstructionPass::recreate(RenderGraphBuilder &builder) {
        renderPipelineSkinned = builder.inheritResource(renderPipelineSkinned);

        const auto resolution = builder.getBackBufferSize() * config->getRenderScale();

        if (currentResolution != resolution) {
            // Recreate GBuffer textures
            currentResolution = resolution;

            auto desc = RenderGraphTextureBuffer();
            desc.size = currentResolution;
            desc.format = RGBA32F;
            gBuffer.gBufferPosition = builder.createTexture(desc);
            gBuffer.gBufferNormal = builder.createTexture(desc);
            gBuffer.gBufferTangent = builder.createTexture(desc);
            gBuffer.gBufferRoughnessMetallicAmbientOcclusion = builder.createTexture(desc);

            desc.format = RGBA;
            gBuffer.gBufferAlbedo = builder.createTexture(desc);

            desc.format = RGBA32I;
            gBuffer.gBufferObjectShadows = builder.createTexture(desc);

            desc.format = DEPTH_STENCIL;
            gBuffer.gBufferDepth = builder.createTexture(desc);
        } else {
            gBuffer.gBufferPosition = builder.inheritResource(gBuffer.gBufferPosition);
            gBuffer.gBufferNormal = builder.inheritResource(gBuffer.gBufferNormal);
            gBuffer.gBufferTangent = builder.inheritResource(gBuffer.gBufferTangent);
            gBuffer.gBufferRoughnessMetallicAmbientOcclusion = builder.inheritResource(
                gBuffer.gBufferRoughnessMetallicAmbientOcclusion);
            gBuffer.gBufferAlbedo = builder.inheritResource(gBuffer.gBufferAlbedo);
            gBuffer.gBufferObjectShadows = builder.inheritResource(gBuffer.gBufferObjectShadows);
            gBuffer.gBufferDepth = builder.inheritResource(gBuffer.gBufferDepth);
        }

        registry->set(gBuffer);;

        shaderBuffer = builder.inheritResource(shaderBuffer);

        boneBuffer = builder.createShaderBuffer(totalBoneBufferSize);
        currentBoneBufferSize = totalBoneBufferSize;

        textureAtlas.onRecreate(builder);

        auto pass = builder.addPass("ConstructionPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshAtlas.update(builder, pass);

        textureAtlas.declareReadWrite(builder, pass);

        builder.read(pass, renderPipelineSkinned);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    bool ConstructionPass::shouldRebuild(const Vec2i &backBufferSize) {
        totalBoneBufferSize = 0;

        auto &scene = config->getScene();

        usedTextures.clear();
        usedMeshes.clear();
        usedMeshUris.clear();

        for (auto &object: scene.skinnedModels) {
            if (!object.model.assigned()) {
                continue;
            }
            for (auto i = 0; i < object.model.get().subMeshes.size(); i++) {
                const auto &subMesh = object.model.get().subMeshes.at(i);

                const auto it = object.materials.find(i);
                const auto &mat = it == object.materials.end() ? subMesh.material.get() : it->second;

                if (mat.transparent) {
                    continue;
                }

                totalBoneBufferSize += subMesh.bones.size() * sizeof(Mat4f);

                if (mat.normal.assigned()) {
                    if (textures.find(mat.normal.getUri()) == textures.end()) {
                        textures[mat.normal.getUri()] = textureAtlas.add(mat.normal.get());
                    }
                    usedTextures.insert(mat.normal.getUri());
                }
                if (mat.metallicTexture.assigned()) {
                    if (textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                        textures[mat.metallicTexture.getUri()] = textureAtlas.
                                add(mat.metallicTexture.get());
                    }
                    usedTextures.insert(mat.metallicTexture.getUri());
                }
                if (mat.roughnessTexture.assigned()) {
                    if (textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                        textures[mat.roughnessTexture.getUri()] = textureAtlas.add(
                            mat.roughnessTexture.get());
                    }
                    usedTextures.insert(mat.roughnessTexture.getUri());
                }
                if (mat.ambientOcclusionTexture.assigned()) {
                    if (textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                        textures[mat.ambientOcclusionTexture.getUri()] = textureAtlas.add(
                            mat.ambientOcclusionTexture.get());
                    }
                    usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                }
                if (mat.albedoTexture.assigned()) {
                    if (textures.find(mat.albedoTexture.getUri()) == textures.end()) {
                        textures[mat.albedoTexture.getUri()] = textureAtlas.add(mat.albedoTexture.get());
                    }
                    usedTextures.insert(mat.albedoTexture.getUri());
                }
            }
            meshAtlas.allocateMesh(object.model);
            usedMeshes.emplace_back(object.model);
            usedMeshUris.insert(object.model.getUri());
        }

        // Deallocate unused meshes
        for (auto &mesh: allocatedMeshes) {
            if (usedMeshUris.find(mesh.getUri()) == usedMeshUris.end()) {
                meshAtlas.deallocateMesh(mesh);
            }
        }
        allocatedMeshes = usedMeshes;

        // Deallocate unused textures
        dealloc.clear();
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            textureAtlas.remove(textures.at(uri));
            textures.erase(uri);
        }

        if (textureAtlas.shouldRebuild()) {
            return true;
        }

        if (meshAtlas.shouldRebuild()) {
            return true;
        }

        return currentBoneBufferSize < totalBoneBufferSize;
    }

    void ConstructionPass::runPass(RenderGraphContext &ctx) {
        // Draw geometry buffer
        auto &scene = config->getScene();

        auto projection = scene.camera.projection();
        auto view = Camera::view(scene.cameraTransform);

        drawCalls.clear();
        baseVertices.clear();
        shaderData.clear();
        boneMatrices.clear();

        auto &meshAllocations = meshAtlas.getMeshAllocations(ctx);

        for (auto objectIndex = 0; objectIndex < scene.skinnedModels.size(); objectIndex++) {
            auto &object = scene.skinnedModels.at(objectIndex);

            const auto &rig = object.model.get().rig;
            const auto &boneTransforms = object.boneTransforms;
            const auto &materials = object.materials;
            const auto &drawData = meshAllocations.at(object.model.getUri());

            for (auto i = 0; i < object.model.get().subMeshes.size(); i++) {
                const auto &model = object.transform.model();

                const auto &mesh = object.model.get().subMeshes.at(i);

                auto mIt = materials.find(i);

                const auto &material = mIt == materials.end() ? mesh.material.get() : mIt->second;

                if (material.transparent) {
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

                auto data = BufferLayout::CPU();

                data.model = model;
                data.mvp = projection * view * model;
                data.objectID_boneOffset_shadows = Vec4i(objectIndex, static_cast<int>(boneOffset), receiveShadows, 0);
                data.metallic_roughness_ambientOcclusion = Vec4f(material.metallic, material.roughness,
                                                                 material.ambientOcclusion, 0);
                data.albedoColor = material.albedo.divide();
                data.normalIntensity = Vec4f(material.normalIntensity, 0, 0, 0);

                if (material.metallicTexture.assigned()) {
                    auto tex = textures.at(material.metallicTexture.getUri());

                    data.metallic.level_index_filtering_assigned = Vec4i(tex.level,
                                                                         static_cast<int>(tex.index),
                                                                         material.metallicFiltering > NEAREST,
                                                                         1);

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.metallic.atlasScale_texSize = Vec4f(atlasScale.x, atlasScale.y, static_cast<float>(tex.size.x),
                                                             static_cast<float>(tex.size.y));
                }

                if (material.roughnessTexture.assigned()) {
                    auto tex = textures.at(material.roughnessTexture.getUri());

                    data.roughness.level_index_filtering_assigned = Vec4i(tex.level,
                                                                          static_cast<int>(tex.index),
                                                                          material.roughnessFiltering > NEAREST,
                                                                          1);

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.roughness.atlasScale_texSize = Vec4f(atlasScale.x, atlasScale.y,
                                                              static_cast<float>(tex.size.x),
                                                              static_cast<float>(tex.size.y));
                }

                if (material.ambientOcclusionTexture.assigned()) {
                    auto tex = textures.at(material.ambientOcclusionTexture.getUri());

                    data.ambientOcclusion.level_index_filtering_assigned = Vec4i(tex.level,
                        static_cast<int>(tex.index),
                        material.ambientOcclusionFiltering > NEAREST,
                        1);

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.ambientOcclusion.atlasScale_texSize = Vec4f(atlasScale.x,
                                                                     atlasScale.y,
                                                                     static_cast<float>(tex.size.x),
                                                                     static_cast<float>(tex.size.y));
                }

                if (material.albedoTexture.assigned()) {
                    auto tex = textures.at(material.albedoTexture.getUri());

                    data.albedo.level_index_filtering_assigned = Vec4i(tex.level,
                                                                       static_cast<int>(tex.index),
                                                                       material.albedoFiltering > NEAREST,
                                                                       1);

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.albedo.atlasScale_texSize = Vec4f(atlasScale.x,
                                                           atlasScale.y,
                                                           static_cast<float>(tex.size.x),
                                                           static_cast<float>(tex.size.y));
                }

                if (material.normal.assigned()) {
                    auto tex = textures.at(material.normal.getUri());

                    data.normal.level_index_filtering_assigned = Vec4i(tex.level,
                                                                       static_cast<int>(tex.index),
                                                                       false,
                                                                       1);

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.normal.atlasScale_texSize = Vec4f(atlasScale.x,
                                                           atlasScale.y,
                                                           static_cast<float>(tex.size.x),
                                                           static_cast<float>(tex.size.y));
                }

                shaderData.emplace_back(data);

                auto &draw = drawData.data.at(i);
                drawCalls.emplace_back(draw.drawCall);
                baseVertices.emplace_back(draw.baseVertex);
            }
        }

        auto &atlasTextures = textureAtlas.getAtlasTextures(ctx);

        ctx.beginRenderPass({
                                RenderGraphAttachment(gBuffer.gBufferPosition, Vec4f(0)),
                                RenderGraphAttachment(gBuffer.gBufferNormal, Vec4f(0)),
                                RenderGraphAttachment(gBuffer.gBufferTangent, Vec4f(0)),
                                RenderGraphAttachment(gBuffer.gBufferRoughnessMetallicAmbientOcclusion, Vec4f(0)),
                                RenderGraphAttachment(gBuffer.gBufferAlbedo, ColorRGBA::black(1, 0)),
                                RenderGraphAttachment(gBuffer.gBufferObjectShadows, Vec4i(0)),
                            },
                            RenderGraphAttachment(gBuffer.gBufferDepth, 1, 0));

        ctx.setViewport({}, currentResolution);

        ctx.uploadBuffer(boneBuffer,
                         reinterpret_cast<const uint8_t *>(boneMatrices.data()),
                         boneMatrices.size() * sizeof(Mat4f),
                         0);

        ctx.bindPipeline(renderPipelineSkinned);
        ctx.bindVertexBuffer(meshAtlas.getVertexBuffer());
        ctx.bindIndexBuffer(meshAtlas.getIndexBuffer());

        std::vector<RenderGraphResourceHandle> bindTextures;
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; ++i) {
            bindTextures.emplace_back(atlasTextures.at(static_cast<TextureAtlasResolution>(i)));
        }
        ctx.bindTexture("atlasTextures", bindTextures);

        ctx.bindShaderBuffer("data", shaderBuffer);
        ctx.bindShaderBuffer("bones", boneBuffer);

        for (auto i = 0; i < shaderData.size(); ++i) {
            auto &data = shaderData.at(i);
            ctx.uploadBuffer(shaderBuffer,
                             reinterpret_cast<const uint8_t *>(&data),
                             sizeof(BufferLayout::CPU),
                             0);
            ctx.drawIndexed(drawCalls.at(i), baseVertices.at(i));
        }

        ctx.endRenderPass();
    }
}
