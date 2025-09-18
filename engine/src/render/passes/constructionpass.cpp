/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/render/passes/constructionpass.hpp"

namespace xng {
#pragma pack(push, 1)
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

        float normalIntensity[4]{0, 0, 0, 0};

        ShaderAtlasTexture normal;

        ShaderAtlasTexture metallic;
        ShaderAtlasTexture roughness;
        ShaderAtlasTexture ambientOcclusion;
        ShaderAtlasTexture albedo;
    };
#pragma pack(pop)

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

        auto desc = RenderGraphTexture();
        desc.size = currentResolution;
        desc.format = RGBA32F;
        gBufferPosition = builder.createTexture(desc);
        gBufferNormal = builder.createTexture(desc);
        gBufferTangent = builder.createTexture(desc);
        gBufferRoughnessMetallicAmbientOcclusion = builder.createTexture(desc);

        desc.format = RGBA;
        gBufferAlbedo = builder.createTexture(desc);

        desc.format = RGBA32I;
        gBufferObjectShadows = builder.createTexture(desc);

        desc.format = DEPTH_STENCIL;
        gBufferDepth = builder.createTexture(desc);

        shaderBuffer = builder.createShaderBuffer(0);
        boneBuffer = builder.createShaderBuffer(0);

        atlas.onCreate(builder);
        meshAllocator.onCreate(builder);

        registry->setEntry(GBUFFER_POSITION, gBufferPosition);
        registry->setEntry(GBUFFER_NORMAL, gBufferNormal);
        registry->setEntry(GBUFFER_TANGENT, gBufferTangent);
        registry->setEntry(GBUFFER_ROUGHNESS_METALLIC_AO, gBufferRoughnessMetallicAmbientOcclusion);
        registry->setEntry(GBUFFER_ALBEDO, gBufferAlbedo);
        registry->setEntry(GBUFFER_OBJECT_SHADOWS, gBufferObjectShadows);
        registry->setEntry(GBUFFER_DEPTH, gBufferDepth);

        auto pass = builder.addPass("ConstructionPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        atlas.declareReadWrite(builder, pass);
        meshAllocator.declareReadWrite(builder, pass);

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

            auto desc = RenderGraphTexture();
            desc.size = currentResolution;
            desc.format = RGBA32F;
            gBufferPosition = builder.createTexture(desc);
            gBufferNormal = builder.createTexture(desc);
            gBufferTangent = builder.createTexture(desc);
            gBufferRoughnessMetallicAmbientOcclusion = builder.createTexture(desc);

            desc.format = RGBA;
            gBufferAlbedo = builder.createTexture(desc);

            desc.format = RGBA32I;
            gBufferObjectShadows = builder.createTexture(desc);

            desc.format = DEPTH_STENCIL;
            gBufferDepth = builder.createTexture(desc);
        } else {
            gBufferPosition = builder.inheritResource(gBufferPosition);
            gBufferNormal = builder.inheritResource(gBufferNormal);
            gBufferTangent = builder.inheritResource(gBufferTangent);
            gBufferRoughnessMetallicAmbientOcclusion = builder.inheritResource(gBufferRoughnessMetallicAmbientOcclusion);
            gBufferAlbedo = builder.inheritResource(gBufferAlbedo);
            gBufferObjectShadows = builder.inheritResource(gBufferObjectShadows);
            gBufferDepth = builder.inheritResource(gBufferDepth);
        }

        registry->setEntry(GBUFFER_POSITION, gBufferPosition);
        registry->setEntry(GBUFFER_NORMAL, gBufferNormal);
        registry->setEntry(GBUFFER_TANGENT, gBufferTangent);
        registry->setEntry(GBUFFER_ROUGHNESS_METALLIC_AO, gBufferRoughnessMetallicAmbientOcclusion);
        registry->setEntry(GBUFFER_ALBEDO, gBufferAlbedo);
        registry->setEntry(GBUFFER_OBJECT_SHADOWS, gBufferObjectShadows);
        registry->setEntry(GBUFFER_DEPTH, gBufferDepth);

        shaderBuffer = builder.createShaderBuffer(totalShaderBufferSize);
        currentShaderBufferSize = totalShaderBufferSize;

        boneBuffer = builder.createShaderBuffer(totalBoneBufferSize);
        currentBoneBufferSize = totalBoneBufferSize;

        atlas.onRecreate(builder);
        meshAllocator.onRecreate(builder);

        auto pass = builder.addPass("ConstructionPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        atlas.declareReadWrite(builder, pass);
        meshAllocator.declareReadWrite(builder, pass);

        builder.read(pass, renderPipelineSkinned);
        builder.readWrite(pass, shaderBuffer);
        builder.readWrite(pass, boneBuffer);
    }

    bool ConstructionPass::shouldRebuild(const Vec2i &backBufferSize) {
        totalShaderBufferSize = 0;
        totalBoneBufferSize = 0;

        auto &scene = config->getScene();

        std::set<Uri> usedTextures;
        std::vector<ResourceHandle<SkinnedMesh> > usedMeshes;

        auto tmp = scene.skinnedMeshes;
        for (auto id = 0; id < tmp.size(); id++) {
            auto &object = tmp.at(id);
            auto &meshResource = object.mesh;
            if (meshResource.assigned()) {
                meshAllocator.allocateMesh(meshResource);
                usedMeshes.emplace_back(meshResource);

                for (auto i = 0; i < meshResource.get().subMeshes.size() + 1; i++) {
                    const Mesh &mesh = i == 0 ? meshResource.get() : meshResource.get().subMeshes.at(i - 1);

                    Material mat = mesh.material.get();

                    auto mi = object.materials.find(i);
                    if (mi != object.materials.end()) {
                        mat = mi->second.get();
                    }

                    if (mat.transparent) {
                        continue;
                    }

                    totalBoneBufferSize += mesh.bones.size() * sizeof(Mat4f);

                    if (mat.normal.assigned()) {
                        if (textures.find(mat.normal.getUri()) == textures.end()) {
                            textures[mat.normal.getUri()] = atlas.add(mat.normal.get().image.get());
                        }
                        usedTextures.insert(mat.normal.getUri());
                    }
                    if (mat.metallicTexture.assigned()) {
                        if (textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                            textures[mat.metallicTexture.getUri()] = atlas.add(mat.metallicTexture.get().image.get());
                        }
                        usedTextures.insert(mat.metallicTexture.getUri());
                    }
                    if (mat.roughnessTexture.assigned()) {
                        if (textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                            textures[mat.roughnessTexture.getUri()] = atlas.add(mat.roughnessTexture.get().image.get());
                        }
                        usedTextures.insert(mat.roughnessTexture.getUri());
                    }
                    if (mat.ambientOcclusionTexture.assigned()) {
                        if (textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                            textures[mat.ambientOcclusionTexture.getUri()] = atlas.add(
                                mat.ambientOcclusionTexture.get().image.get());
                        }
                        usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                    }
                    if (mat.albedoTexture.assigned()) {
                        if (textures.find(mat.albedoTexture.getUri()) == textures.end()) {
                            textures[mat.albedoTexture.getUri()] = atlas.add(mat.albedoTexture.get().image.get());
                        }
                        usedTextures.insert(mat.albedoTexture.getUri());
                    }

                    totalShaderBufferSize += sizeof(ShaderDrawData);
                }
                objects.emplace_back(object);
            }
        }

        cameraTransform = scene.cameraTransform;
        camera = scene.camera;

        // Deallocate unused meshes
        std::vector<ResourceHandle<SkinnedMesh> > meshDealloc;
        for (auto &mesh: meshes) {
            auto it = std::find(usedMeshes.begin(), usedMeshes.end(), mesh);
            if (it == usedMeshes.end()) {
                meshDealloc.emplace_back(mesh);
            }
        }
        for (auto &mesh: meshDealloc) {
            meshAllocator.deallocateMesh(mesh);
        }

        // Deallocate unused textures
        std::set<Uri> dealloc;
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            atlas.remove(textures.at(uri));
            textures.erase(uri);
        }

        if (atlas.shouldRebuild()) {
            return true;
        }

        if (meshAllocator.shouldRebuild()) {
            return true;
        }

        return currentShaderBufferSize < totalShaderBufferSize
               || currentBoneBufferSize < totalBoneBufferSize;
    }

    void ConstructionPass::runPass(RenderGraphContext &ctx) {
        // Draw geometry buffer
        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        std::vector<DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        std::vector<ShaderDrawData> shaderData;
        std::vector<Mat4f> boneMatrices;

        auto &allocatedMeshes = meshAllocator.getMeshAllocations(ctx);

        for (auto objectIndex = 0; objectIndex < objects.size(); objectIndex++) {
            auto &object = objects.at(objectIndex);
            auto &meshResource = object.mesh;

            const auto &rig = meshResource.get().rig;
            const auto &boneTransforms = object.boneTransforms;
            const auto &materials = object.materials;
            const auto &drawData = allocatedMeshes.at(meshResource.getUri());

            for (auto i = 0; i < meshResource.get().subMeshes.size() + 1; i++) {
                auto model = object.transform.model();

                const Mesh &mesh = i == 0 ? meshResource.get() : meshResource.get().subMeshes.at(i - 1);

                auto material = mesh.material.get();

                auto mIt = materials.find(i);
                if (mIt != materials.end()) {
                    material = mIt->second.get();
                }

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
                    data.metallic.level_index_filtering_assigned[2] = material.metallicTexture.get().description.
                            filterMag;
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
                    data.roughness.level_index_filtering_assigned[2] = material.roughnessTexture.get().description.
                            filterMag;
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
                    data.ambientOcclusion.level_index_filtering_assigned[2] = material.ambientOcclusionTexture.get().
                            description.filterMag;
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
                    auto tex = textures.at(material.normal.getUri());

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

                auto &draw = drawData.data.at(i);
                drawCalls.emplace_back(draw.drawCall);
                baseVertices.emplace_back(draw.baseVertex);
            }
        }

        auto &atlasTextures = atlas.getAtlasTextures(ctx);

        ctx.beginRenderPass({
                                RenderGraphAttachment(gBufferPosition, Vec4f(0)),
                                RenderGraphAttachment(gBufferNormal, Vec4f(0)),
                                RenderGraphAttachment(gBufferTangent, Vec4f(0)),
                                RenderGraphAttachment(gBufferRoughnessMetallicAmbientOcclusion, Vec4f(0)),
                                RenderGraphAttachment(gBufferAlbedo, ColorRGBA::black(1, 0)),
                                RenderGraphAttachment(gBufferObjectShadows, Vec4i(0)),
                            },
                            RenderGraphAttachment(gBufferDepth, 1, 0));

        ctx.setViewport({}, currentResolution);

        ctx.uploadBuffer(boneBuffer,
                         reinterpret_cast<const uint8_t *>(boneMatrices.data()),
                         boneMatrices.size() * sizeof(Mat4f),
                         0);

        ctx.bindPipeline(renderPipelineSkinned);
        ctx.bindVertexBuffer(meshAllocator.getVertexBuffer());
        ctx.bindIndexBuffer(meshAllocator.getIndexBuffer());

        std::vector<RenderGraphResource> bindTextures;
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; ++i) {
            bindTextures.emplace_back(atlasTextures.at(static_cast<TextureAtlasResolution>(i)));
        }
        ctx.bindTextures({{"atlasTextures", bindTextures}});

        for (auto i = 0; i < shaderData.size(); ++i) {
            auto &data = shaderData.at(i);
            ctx.uploadBuffer(shaderBuffer,
                             reinterpret_cast<const uint8_t *>(&data),
                             sizeof(ShaderDrawData),
                             0);
            ctx.bindShaderBuffers({
                {"data", shaderBuffer},
                {"bones", boneBuffer}
            });
            ctx.drawIndexed(drawCalls.at(i), baseVertices.at(i));
        }

        ctx.endRenderPass();
    }
}
