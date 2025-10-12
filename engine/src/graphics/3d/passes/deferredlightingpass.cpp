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

#include "xng/graphics/3d/passes/deferredlightingpass.hpp"

#include "xng/graphics/vertexstream.hpp"

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

    struct ShaderStorageData {
        std::array<float, 4> viewPosition_gamma{};
    };
#pragma pack(pop)

    DeferredLightingPass::DeferredLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                                               std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(configuration)),
          registry(std::move(registry)) {
    }

    void DeferredLightingPass::create(RenderGraphBuilder &builder) {
        layerSize = builder.getBackBufferSize() * config->getRenderScale();

        RenderGraphPipeline pip;
        pip.enableDepthTest = false;
        pip.enableBlending = false;
        pip.enableFaceCulling = false;
        pip.shaders = {createVertexShader(), createFragmentShader()};
        pipeline = builder.createPipeline(pip);

        vertexBuffer = builder.createVertexBuffer(
            normalizedQuad.vertexLayout.getLayoutSize() * normalizedQuad.vertices.size());

        shaderDataBuffer = builder.createShaderBuffer(sizeof(ShaderStorageData));

        pointLightBuffer = builder.createShaderBuffer(0);
        directionalLightBuffer = builder.createShaderBuffer(0);
        spotLightBuffer = builder.createShaderBuffer(0);

        shadowPointLightBuffer = builder.createShaderBuffer(0);
        shadowDirectionalLightBuffer = builder.createShaderBuffer(0);
        shadowSpotLightBuffer = builder.createShaderBuffer(0);

        shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(0);
        shadowSpotLightTransformBuffer = builder.createShaderBuffer(0);

        shadowMaps = registry->get<ShadowMaps>();
        gBuffer = registry->get<GBuffer>();

        layerSize = builder.getBackBufferSize() * config->getRenderScale();

        RenderGraphTexture tex;
        tex.format = RGBA;
        tex.size = layerSize;
        layer.color = builder.createTexture(tex);
        layer.depth = gBuffer.gBufferDepth;

        auto layers = registry->getOrCreate<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        builder.addPass("DeferredLightingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });
    }

    void DeferredLightingPass::recreate(RenderGraphBuilder &builder) {
        pipeline = builder.inheritResource(pipeline);
        vertexBuffer = builder.inheritResource(vertexBuffer);
        shaderDataBuffer = builder.inheritResource(shaderDataBuffer);

        if (recreateLightBuffers) {
            pointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData)
                                                          * pointLights.size());
            directionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData)
                                                                * directionalLights.size());
            spotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData)
                                                         * spotLights.size());
            shadowPointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData)
                                                                * shadowPointLights.size());
            shadowDirectionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData)
                                                                      * shadowDirectionalLights.size());
            shadowSpotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData)
                                                               * shadowSpotLights.size());
            shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f)
                                                                               * shadowDirectionalLights.size());
            shadowSpotLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f) * shadowSpotLights.size());
        } else {
            pointLightBuffer = builder.inheritResource(pointLightBuffer);
            directionalLightBuffer = builder.inheritResource(directionalLightBuffer);
            spotLightBuffer = builder.inheritResource(spotLightBuffer);
            shadowPointLightBuffer = builder.inheritResource(shadowPointLightBuffer);
            shadowDirectionalLightBuffer = builder.inheritResource(shadowDirectionalLightBuffer);
            shadowSpotLightBuffer = builder.inheritResource(shadowSpotLightBuffer);
            shadowDirectionalLightTransformBuffer = builder.inheritResource(shadowDirectionalLightTransformBuffer);
            shadowSpotLightTransformBuffer = builder.inheritResource(shadowSpotLightTransformBuffer);
        }

        shadowMaps = registry->get<ShadowMaps>();
        gBuffer = registry->get<GBuffer>();

        auto nLayerSize = builder.getBackBufferSize() * config->getRenderScale();
        if (layerSize != nLayerSize) {
            layerSize = nLayerSize;
            RenderGraphTexture tex;
            tex.format = RGBA;
            tex.size = layerSize;
            layer.color = builder.createTexture(tex);
        } else {
            layer.color = builder.inheritResource(layer.color);
        }

        layer.depth = gBuffer.gBufferDepth;

        auto layers = registry->get<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        builder.addPass("DeferredLightingPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });
    }

    bool DeferredLightingPass::shouldRebuild(const Vec2i &backBufferSize) {
        std::vector<PointLightObject> pLights;
        std::vector<PointLightObject> psLights;
        for (auto &object: config->getScene().pointLights) {
            if (object.light.castShadows) {
                psLights.emplace_back(object);
            } else {
                pLights.emplace_back(object);
            }
        }

        std::vector<DirectionalLightObject> dLights;
        std::vector<DirectionalLightObject> dsLights;
        for (auto &object: config->getScene().directionalLights) {
            if (object.light.castShadows) {
                dsLights.emplace_back(object);
            } else {
                dLights.emplace_back(object);
            }
        }

        std::vector<SpotLightObject> sLights;
        std::vector<SpotLightObject> ssLights;
        for (auto &object: config->getScene().spotLights) {
            if (object.light.castShadows) {
                ssLights.emplace_back(object);
            } else {
                sLights.emplace_back(object);
            }
        }

        recreateLightBuffers = pLights != pointLights
                               || psLights != shadowPointLights
                               || dLights != directionalLights
                               || dsLights != shadowDirectionalLights
                               || sLights != spotLights
                               || ssLights != shadowSpotLights;

        pointLights = pLights;
        shadowPointLights = psLights;
        directionalLights = dLights;
        shadowDirectionalLights = dsLights;
        spotLights = sLights;
        shadowSpotLights = ssLights;

        viewPosition = config->getScene().cameraTransform.getPosition();

        if (recreateLightBuffers) {
            return true;
        }

        return layerSize != backBufferSize * config->getRenderScale();
    }

    void DeferredLightingPass::runPass(RenderGraphContext &ctx) {
        if (recreateLightBuffers) {
            std::vector<PointLightData> pointLightData;
            for (auto &light: pointLights) {
                PointLightData data{};
                data.position = Vec4f(light.transform.getPosition().x,
                                      light.transform.getPosition().y,
                                      light.transform.getPosition().z,
                                      0).getMemory();
                data.color = (light.light.color.divide() * light.light.power).getMemory();
                data.farPlane = Vec4f(light.light.shadowFarPlane, 0, 0, 0).getMemory();
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(pointLightBuffer,
                             reinterpret_cast<const uint8_t *>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData),
                             0);

            pointLightData.clear();
            for (auto &light: shadowPointLights) {
                PointLightData data{};
                data.position = Vec4f(light.transform.getPosition().x,
                                      light.transform.getPosition().y,
                                      light.transform.getPosition().z,
                                      0).getMemory();
                data.color = (light.light.color.divide() * light.light.power).getMemory();
                data.farPlane = Vec4f(light.light.shadowFarPlane, 0, 0, 0).getMemory();
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(shadowPointLightBuffer,
                             reinterpret_cast<const uint8_t *>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData),
                             0);

            std::vector<DirectionalLightData> directionalLightData;
            for (auto &light: directionalLights) {
                DirectionalLightData data{};
                data.direction = Vec4f(light.transform.forward().x,
                                       light.transform.forward().y,
                                       light.transform.forward().z,
                                       0).getMemory();
                data.color = (light.light.color.divide() * light.light.power).getMemory();
                data.farPlane = Vec4f(light.light.shadowFarPlane, 0, 0, 0).getMemory();
                directionalLightData.emplace_back(data);
            }
            ctx.uploadBuffer(directionalLightBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData),
                             0);

            directionalLightData.clear();

            std::vector<Mat4f> directionalLightTransforms;
            for (auto &lightObject: shadowDirectionalLights) {
                DirectionalLightData data{};
                data.direction = Vec4f(lightObject.transform.forward().x,
                                       lightObject.transform.forward().y,
                                       lightObject.transform.forward().z,
                                       0).getMemory();
                data.color = (lightObject.light.color.divide() * lightObject.light.power).getMemory();
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0).getMemory();
                directionalLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(MatrixMath::ortho(-lightObject.light.shadowProjectionExtent,
                                                                          lightObject.light.shadowProjectionExtent,
                                                                          -lightObject.light.shadowProjectionExtent,
                                                                          lightObject.light.shadowProjectionExtent,
                                                                          lightObject.light.shadowNearPlane,
                                                                          lightObject.light.shadowFarPlane)
                                                        * MatrixMath::lookAt(lightObject.transform.getPosition(),
                                                                             lightObject.transform.getPosition() +
                                                                             lightObject.transform.forward(),
                                                                             Vec3f(0, 1, 0)));
            }
            ctx.uploadBuffer(shadowDirectionalLightBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightData.data()),
                             directionalLightData.size() * sizeof(DirectionalLightData),
                             0);
            ctx.uploadBuffer(shadowDirectionalLightTransformBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);


            std::vector<SpotLightData> spotLightData;
            for (auto &light: spotLights) {
                SpotLightData data{};
                data.position = Vec4f(light.transform.getPosition().x,
                                      light.transform.getPosition().y,
                                      light.transform.getPosition().z,
                                      0).getMemory();
                data.direction_quadratic = Vec4f(light.transform.forward().x,
                                                 light.transform.forward().y,
                                                 light.transform.forward().z,
                                                 light.light.quadratic).getMemory();
                data.color = (light.light.color.divide() * light.light.power).getMemory();
                data.farPlane = Vec4f(light.light.shadowFarPlane, 0, 0, 0).getMemory();
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(light.light.cutOff),
                                                                SpotLight::getCutOff(light.light.outerCutOff),
                                                                light.light.constant,
                                                                light.light.linear).getMemory();
                spotLightData.emplace_back(data);
            }
            ctx.uploadBuffer(spotLightBuffer,
                             reinterpret_cast<const uint8_t *>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData),
                             0);

            spotLightData.clear();
            directionalLightTransforms.clear();

            for (auto &light: shadowSpotLights) {
                SpotLightData data{};
                data.position = Vec4f(light.transform.getPosition().x,
                                      light.transform.getPosition().y,
                                      light.transform.getPosition().z,
                                      0).getMemory();
                data.direction_quadratic = Vec4f(light.transform.forward().x,
                                                 light.transform.forward().y,
                                                 light.transform.forward().z,
                                                 light.light.quadratic).getMemory();
                data.color = (light.light.color.divide() * light.light.power).getMemory();
                data.farPlane = Vec4f(light.light.shadowFarPlane, 0, 0, 0).getMemory();
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(light.light.cutOff),
                                                                SpotLight::getCutOff(light.light.outerCutOff),
                                                                light.light.constant,
                                                                light.light.linear).getMemory();
                spotLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(MatrixMath::perspective(45,
                                                            1,
                                                            light.light.shadowNearPlane,
                                                            light.light.shadowFarPlane)
                                                        * MatrixMath::lookAt(light.transform.getPosition(),
                                                                             light.transform.getPosition() + light.
                                                                             transform.forward(),
                                                                             Vec3f(0, 1, 0)));
            }
            ctx.uploadBuffer(shadowSpotLightBuffer,
                             reinterpret_cast<const uint8_t *>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData),
                             0);
            ctx.uploadBuffer(shadowSpotLightTransformBuffer,
                             reinterpret_cast<const uint8_t *>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);
            recreateLightBuffers = false;
        }

        if (!normalizedQuadUploaded) {
            VertexStream stream;
            for (auto &vert: normalizedQuad.vertices) {
                stream.addVertex(vert);
            }
            auto data = stream.getVertexBuffer();
            ctx.uploadBuffer(vertexBuffer, data.data(), data.size(), 0);
            normalizedQuadUploaded = true;
        }

        ShaderStorageData shaderData;
        shaderData.viewPosition_gamma = Vec4f(viewPosition.x,
                                              viewPosition.y,
                                              viewPosition.z,
                                              config->getGamma()).getMemory();
        ctx.uploadBuffer(shaderDataBuffer,
                         reinterpret_cast<const uint8_t *>(&shaderData),
                         sizeof(ShaderStorageData),
                         0);

        ctx.beginRenderPass({RenderGraphAttachment(layer.color)}, {});
        ctx.bindPipeline(pipeline);

        ctx.setViewport({}, layerSize);

        ctx.bindVertexBuffer(vertexBuffer);

        ctx.bindShaderBuffer("shaderData", shaderDataBuffer);
        ctx.bindShaderBuffer("pointLights", pointLightBuffer);
        ctx.bindShaderBuffer("directionalLights", directionalLightBuffer);
        ctx.bindShaderBuffer("spotLights", spotLightBuffer);
        ctx.bindShaderBuffer("shadowPointLights", shadowPointLightBuffer);
        ctx.bindShaderBuffer("shadowDirectionalLights", shadowDirectionalLightBuffer);
        ctx.bindShaderBuffer("shadowSpotLights", shadowSpotLightBuffer);
        ctx.bindShaderBuffer("directionalLightShadowTransforms", shadowDirectionalLightTransformBuffer);
        ctx.bindShaderBuffer("spotLightShadowTransforms", shadowSpotLightTransformBuffer);

        ctx.bindTexture("gBufferPos", gBuffer.gBufferPosition);
        ctx.bindTexture("gBufferNormal", gBuffer.gBufferNormal);
        ctx.bindTexture("gBufferRoughnessMetallicAO", gBuffer.gBufferRoughnessMetallicAmbientOcclusion);
        ctx.bindTexture("gBufferAlbedo", gBuffer.gBufferAlbedo);
        ctx.bindTexture("gBufferObjectShadows", gBuffer.gBufferObjectShadows);
        ctx.bindTexture("gBufferDepth", gBuffer.gBufferDepth);

        ctx.bindTexture("pointLightShadowMaps", shadowMaps.pointShadowMaps);
        ctx.bindTexture("directionalLightShadowMaps", shadowMaps.dirShadowMaps);
        ctx.bindTexture("spotLightShadowMaps", shadowMaps.spotShadowMaps);

        ctx.drawArray(DrawCall(0, 6));

        ctx.endRenderPass();
    }
}
