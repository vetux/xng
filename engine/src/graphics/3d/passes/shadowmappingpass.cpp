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

#include "xng/graphics/3d/passes/shadowmappingpass.hpp"

//TODO: Optimize shadow mapping pass

namespace xng {
    struct ShadowShaderDrawData {
        std::array<int, 4> boneOffset{};
        Mat4f model;
    };

    struct ShadowPointLightData {
        std::array<float, 4> lightPosFarPlane{};
        std::array<int, 4> layer{};
        std::array<Mat4f, 6> shadowMatrices;
    };

    struct ShadowDirLightData {
        std::array<int, 4> layer{};
        Mat4f shadowMatrix;
    };

    ShadowMappingPass::ShadowMappingPass(std::shared_ptr<RenderConfiguration> config,
                                         std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(config)),
          registry(std::move(registry)) {
    }

    void ShadowMappingPass::create(RenderGraphBuilder &builder) {
        RenderGraphPipeline pipeline;
        pipeline.enableDepthTest = true;
        pipeline.depthTestWrite = true;
        pipeline.depthTestMode = RenderGraphPipeline::DEPTH_TEST_LESS;
        pipeline.enableFaceCulling = true;
        pipeline.enableBlending = false;

        pipeline.shaders = {
            createVertexShader(),
            createGeometryShader(),
            createFragmentShader()
        };

        pointPipeline = builder.createPipeline(pipeline);

        pipeline.shaders = {
            createDirVertexShader(),
            createDirGeometryShader(),
            createDirFragmentShader()
        };

        dirPipeline = builder.createPipeline(pipeline);

        const auto pointShadowResolution = Vec2i(config->getPointShadowResolution());
        const auto dirShadowResolution = Vec2i(config->getDirectionalShadowResolution());
        const auto spotShadowResolution = Vec2i(config->getSpotShadowResolution());

        pointShadowMapResolution = pointShadowResolution;
        dirShadowMapResolution = dirShadowResolution;
        spotShadowMapResolution = spotShadowResolution;

        RenderGraphTexture tex;
        tex.format = DEPTH;
        tex.textureType = TEXTURE_CUBE_MAP_ARRAY;
        tex.size = pointShadowResolution;
        shadowMaps.pointShadowMaps = builder.createTexture(tex);

        tex.textureType = TEXTURE_2D_ARRAY;
        tex.size = dirShadowResolution;
        shadowMaps.dirShadowMaps = builder.createTexture(tex);

        tex.size = spotShadowResolution;
        shadowMaps.spotShadowMaps = builder.createTexture(tex);

        registry->set(shadowMaps);

        shaderBuffer = builder.createShaderBuffer(sizeof(ShadowShaderDrawData));

        currentBoneBufferSize = 0;
        boneBuffer = builder.createShaderBuffer(currentBoneBufferSize);

        dirLightBuffer = builder.createShaderBuffer(sizeof(ShadowDirLightData));
        pointLightBuffer = builder.createShaderBuffer(sizeof(ShadowPointLightData));

        auto pass = builder.addPass("ShadowMapping", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshBuffer.update(builder, pass);
    }

    void ShadowMappingPass::recreate(RenderGraphBuilder &builder) {
        pointPipeline = builder.inheritResource(pointPipeline);
        dirPipeline = builder.inheritResource(dirPipeline);
        shaderBuffer = builder.inheritResource(shaderBuffer);
        dirLightBuffer = builder.inheritResource(dirLightBuffer);
        pointLightBuffer = builder.inheritResource(pointLightBuffer);
        const auto pointShadowResolution = Vec2i(config->getPointShadowResolution());
        const auto dirShadowResolution = Vec2i(config->getDirectionalShadowResolution());
        const auto spotShadowResolution = Vec2i(config->getSpotShadowResolution());

        if (pointShadowResolution != pointShadowMapResolution || pointShadowMapCount != pointLights.size()) {
            RenderGraphTexture tex;
            tex.format = DEPTH;
            tex.textureType = TEXTURE_CUBE_MAP_ARRAY;
            tex.size = pointShadowResolution;
            tex.arrayLayers = pointLights.size();
            shadowMaps.pointShadowMaps = builder.createTexture(tex);
            pointShadowMapResolution = pointShadowResolution;
            pointShadowMapCount = pointLights.size();
        } else {
            shadowMaps.pointShadowMaps = builder.inheritResource(shadowMaps.pointShadowMaps);
        }

        if (dirShadowResolution != dirShadowMapResolution || dirShadowMapCount != dirLights.size()) {
            RenderGraphTexture tex;
            tex.format = DEPTH;
            tex.textureType = TEXTURE_2D_ARRAY;
            tex.size = dirShadowResolution;
            tex.arrayLayers = dirLights.size();
            shadowMaps.dirShadowMaps = builder.createTexture(tex);
            dirShadowMapResolution = dirShadowResolution;
            dirShadowMapCount = dirLights.size();
        } else {
            shadowMaps.dirShadowMaps = builder.inheritResource(shadowMaps.dirShadowMaps);
        }

        if (spotShadowResolution != spotShadowMapResolution || spotShadowMapCount != spotLights.size()) {
            RenderGraphTexture tex;
            tex.format = DEPTH;
            tex.textureType = TEXTURE_2D_ARRAY;
            tex.size = spotShadowResolution;
            tex.arrayLayers = spotLights.size();
            shadowMaps.spotShadowMaps = builder.createTexture(tex);
            spotShadowMapResolution = spotShadowResolution;
            spotShadowMapCount = spotLights.size();
        } else {
            shadowMaps.spotShadowMaps = builder.inheritResource(shadowMaps.spotShadowMaps);
        }

        registry->set(shadowMaps);

        if (currentBoneBufferSize != requiredBoneBufferSize) {
            boneBuffer = builder.createShaderBuffer(requiredBoneBufferSize);
            currentBoneBufferSize = requiredBoneBufferSize;
        } else {
            boneBuffer = builder.inheritResource(boneBuffer);
        }

        auto pass = builder.addPass("ShadowMapping", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        meshBuffer.update(builder, pass);
    }

    bool ShadowMappingPass::shouldRebuild(const Vec2i &backBufferSize) {
        auto pointShadowResolution = Vec2i(config->getPointShadowResolution());
        auto dirShadowResolution = Vec2i(config->getDirectionalShadowResolution());
        auto spotShadowResolution = Vec2i(config->getSpotShadowResolution());

        meshObjects.clear();

        std::vector<ResourceHandle<SkinnedMesh> > usedMeshes;

        size_t boneCount = 0;
        for (auto &meshObject: config->getScene().skinnedMeshes) {
            if (meshObject.mesh.assigned() && meshObject.castShadows) {
                meshBuffer.allocateMesh(meshObject.mesh);
                usedMeshes.emplace_back(meshObject.mesh);

                for (auto i = 0; i < meshObject.mesh.get().subMeshes.size() + 1; i++) {
                    auto &mesh = i == 0 ? meshObject.mesh.get() : meshObject.mesh.get().subMeshes.at(i - 1);
                    boneCount += mesh.bones.size();
                }

                meshObjects.emplace_back(meshObject);
            }
        }

        requiredBoneBufferSize = boneCount * sizeof(Mat4f);

        for (auto &mesh: allocatedMeshes) {
            if (std::find(usedMeshes.begin(), usedMeshes.end(), mesh) == usedMeshes.end()) {
                meshBuffer.deallocateMesh(ResourceHandle<SkinnedMesh>());
            }
        }

        allocatedMeshes = usedMeshes;

        pointLights.clear();
        dirLights.clear();
        spotLights.clear();

        for (auto &lightNode: config->getScene().pointLights) {
            if (lightNode.light.castShadows) {
                pointLights.emplace_back(lightNode);
            }
        }

        for (auto &lightNode: config->getScene().directionalLights) {
            if (lightNode.light.castShadows) {
                dirLights.emplace_back(lightNode);
            }
        }

        for (auto &lightNode: config->getScene().spotLights) {
            if (lightNode.light.castShadows) {
                spotLights.emplace_back(lightNode);
            }
        }

        if (meshBuffer.shouldRebuild()) {
            return true;
        }

        if (currentBoneBufferSize < requiredBoneBufferSize) {
            return true;
        }

        if (pointShadowResolution != pointShadowMapResolution
            || dirShadowResolution != dirShadowMapResolution
            || spotShadowResolution != spotShadowMapResolution) {
            return true;
        }

        return false;
    }

    void ShadowMappingPass::runPass(RenderGraphContext &ctx) {
        const auto &meshAllocations = meshBuffer.getMeshAllocations(ctx);

        std::vector<Mat4f> boneMatrices;
        std::vector<ShadowShaderDrawData> drawData;
        std::vector<MeshBuffer3D::MeshAllocation::Data> meshData;
        for (auto &meshObject: meshObjects) {
            auto model = meshObject.transform.model();

            for (auto i = 0; i < meshObject.mesh.get().subMeshes.size() + 1; i++) {
                auto& mesh = i == 0 ? meshObject.mesh.get() : meshObject.mesh.get().subMeshes.at(i - 1);
                int boneOffset = static_cast<int>(boneMatrices.size());
                if (meshObject.mesh.get().bones.empty()) {
                    boneOffset = -1;
                } else {
                    for (auto &bone: mesh.bones) {
                        auto bt = meshObject.boneTransforms.find(bone);
                        if (bt != meshObject.boneTransforms.end()) {
                            boneMatrices.emplace_back(bt->second);
                        } else {
                            boneMatrices.emplace_back(MatrixMath::identity());
                        }
                    }
                }

                auto data = ShadowShaderDrawData();
                data.model = model;
                data.boneOffset[0] = boneOffset;
                drawData.emplace_back(data);

                meshData.emplace_back(meshAllocations.at(meshObject.mesh.getUri()).data.at(i));
            }
        }

        ctx.uploadBuffer(boneBuffer,
                         reinterpret_cast<const uint8_t *>(boneMatrices.data()),
                         boneMatrices.size() * sizeof(Mat4f), 0);

        // Render Point Light Shadow Maps
        if (pointLights.size() > 0) {
            ctx.beginRenderPass({}, RenderGraphAttachment(shadowMaps.pointShadowMaps));
            ctx.clearDepthAttachment(1);
            ctx.bindPipeline(pointPipeline);
            ctx.setViewport({}, pointShadowMapResolution);
            ctx.bindVertexBuffer(meshBuffer.getVertexBuffer());
            ctx.bindIndexBuffer(meshBuffer.getIndexBuffer());
            ctx.bindShaderBuffer("drawData", shaderBuffer);
            ctx.bindShaderBuffer("bones", boneBuffer);
            ctx.bindShaderBuffer("lightData", pointLightBuffer);
            for (auto i = 0; i < pointLights.size(); i++) {
                auto &lightObject = pointLights.at(i);
                auto &transform = lightObject.transform;
                float aspect = 1;
                float near = lightObject.light.shadowNearPlane;
                float far = lightObject.light.shadowFarPlane;

                Mat4f shadowProj = MatrixMath::perspective(90.0f, aspect, near, far);

                auto &lightPos = transform.getPosition();

                ShadowPointLightData lightData;
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

                lightData.layer[0] = i;

                ctx.uploadBuffer(pointLightBuffer,
                                 reinterpret_cast<const uint8_t *>(&lightData),
                                 sizeof(ShadowPointLightData),
                                 0);

                for (auto drawIndex = 0; drawIndex < meshData.size(); drawIndex++) {
                    auto &mesh = meshData.at(drawIndex);
                    auto &draw = drawData.at(drawIndex);
                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&draw),
                                     sizeof(ShadowShaderDrawData),
                                     0);
                    ctx.drawIndexed(mesh.drawCall, mesh.baseVertex);
                }
            }
            ctx.endRenderPass();
        }

        // Render Directional Light Shadow Maps
        if (dirLights.size() > 0) {
            ctx.beginRenderPass({}, RenderGraphAttachment(shadowMaps.dirShadowMaps));
            ctx.clearDepthAttachment(1);
            ctx.bindPipeline(dirPipeline);
            ctx.setViewport({}, dirShadowMapResolution);
            ctx.bindVertexBuffer(meshBuffer.getVertexBuffer());
            ctx.bindIndexBuffer(meshBuffer.getIndexBuffer());
            ctx.bindShaderBuffer("drawData", shaderBuffer);
            ctx.bindShaderBuffer("bones", boneBuffer);
            ctx.bindShaderBuffer("lightData", dirLightBuffer);
            for (auto i = 0; i < dirLights.size(); i++) {
                auto &lightObject = dirLights.at(i);
                auto &light = lightObject.light;

                Mat4f shadowProj = MatrixMath::ortho(-light.shadowProjectionExtent,
                                                     light.shadowProjectionExtent,
                                                     -light.shadowProjectionExtent,
                                                     light.shadowProjectionExtent,
                                                     light.shadowNearPlane,
                                                     light.shadowFarPlane)
                                   * MatrixMath::lookAt(lightObject.transform.getPosition(),
                                                        lightObject.transform.getPosition() + lightObject.transform.forward(),
                                                        Vec3f(0, 1, 0));

                ShadowDirLightData lightData;
                lightData.shadowMatrix = shadowProj;
                lightData.layer[0] = i;

                ctx.uploadBuffer(dirLightBuffer,
                                 reinterpret_cast<const uint8_t *>(&lightData),
                                 sizeof(ShadowDirLightData),
                                 0);

                for (auto drawIndex = 0; drawIndex < meshData.size(); drawIndex++) {
                    auto &mesh = meshData.at(drawIndex);
                    auto &draw = drawData.at(drawIndex);
                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&draw),
                                     sizeof(ShadowShaderDrawData),
                                     0);
                    ctx.drawIndexed(mesh.drawCall, mesh.baseVertex);
                }
            }
            ctx.endRenderPass();
        }

        // Render Spot Light Shadow Maps
        if (spotLights.size() > 0) {
            ctx.beginRenderPass({}, RenderGraphAttachment(shadowMaps.spotShadowMaps));
            ctx.clearDepthAttachment(1);
            ctx.bindPipeline(dirPipeline);
            ctx.setViewport({}, spotShadowMapResolution);
            ctx.bindVertexBuffer(meshBuffer.getVertexBuffer());
            ctx.bindIndexBuffer(meshBuffer.getIndexBuffer());
            ctx.bindShaderBuffer("drawData", shaderBuffer);
            ctx.bindShaderBuffer("bones", boneBuffer);
            ctx.bindShaderBuffer("lightData", dirLightBuffer);
            for (auto i = 0; i < spotLights.size(); i++) {
                auto &lightObject = spotLights.at(i);
                auto &light = lightObject.light;
                auto &transform = lightObject.transform;
                float aspect = 1;

                Mat4f shadowProj = MatrixMath::perspective(45,
                                                           aspect,
                                                           light.shadowNearPlane,
                                                           light.shadowFarPlane)
                                   * MatrixMath::lookAt(transform.getPosition(),
                                                        transform.getPosition() + transform.forward(),
                                                        Vec3f(0, 1, 0));

                ShadowDirLightData lightData;
                lightData.shadowMatrix = shadowProj;
                lightData.layer[0] = i;

                ctx.uploadBuffer(dirLightBuffer,
                                 reinterpret_cast<const uint8_t *>(&lightData),
                                 sizeof(ShadowDirLightData),
                                 0);

                for (auto drawIndex = 0; drawIndex < meshData.size(); drawIndex++) {
                    auto &mesh = meshData.at(drawIndex);
                    auto &draw = drawData.at(drawIndex);
                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&draw),
                                     sizeof(ShadowShaderDrawData),
                                     0);
                    ctx.drawIndexed(mesh.drawCall, mesh.baseVertex);
                }
            }
            ctx.endRenderPass();
        }
    }
}
