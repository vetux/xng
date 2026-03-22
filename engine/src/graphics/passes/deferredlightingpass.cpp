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

#include "xng/graphics/passes/deferredlightingpass.hpp"
#include "xng/graphics/sharedresources/ibl.hpp"

namespace xng
{
    DeferredLightingPass::DeferredLightingPass(std::shared_ptr<RenderConfiguration> configuration,
                                               std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(configuration)),
          registry(std::move(registry))
    {
    }

    void DeferredLightingPass::create(RenderGraphBuilder& builder)
    {
        layerSize = builder.getBackBufferSize() * config->getRenderScale();

        RenderGraphPipeline pip;
        pip.enableDepthTest = false;
        pip.enableBlending = false;
        pip.enableFaceCulling = false;
        pip.shaders = {createVertexShader(), createFragmentShader()};
        pipeline = builder.createPipeline(pip);

        vertexBuffer = builder.createVertexBuffer(normalizedQuad.vertices.size());

        shaderDataBuffer = builder.createShaderBuffer(sizeof(ShaderData::CPU));

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

        RenderGraphTextureBuffer tex;
        tex.format = RGBA;
        tex.size = layerSize;
        layer.color = builder.createTexture(tex);
        layer.depth = gBuffer.gBufferDepth;

        auto layers = registry->getOrCreate<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        auto pass = builder.addPass("DeferredLightingPass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });

        // If IBL maps are available in the shared registry, declare them as read resources
        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) builder.read(pass, ibl.irradiance);
            if (ibl.prefilter) builder.read(pass, ibl.prefilter);
            if (ibl.brdfLUT) builder.read(pass, ibl.brdfLUT);
        }
    }

    void DeferredLightingPass::recreate(RenderGraphBuilder& builder)
    {
        pipeline = builder.inheritResource(pipeline);
        vertexBuffer = builder.inheritResource(vertexBuffer);
        shaderDataBuffer = builder.inheritResource(shaderDataBuffer);

        if (recreateLightBuffers)
        {
            pointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData::CPU)
                * pointLights.size());
            directionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData::CPU)
                * directionalLights.size());
            spotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData::CPU)
                * spotLights.size());
            shadowPointLightBuffer = builder.createShaderBuffer(sizeof(PointLightData::CPU)
                * shadowPointLights.size());
            shadowDirectionalLightBuffer = builder.createShaderBuffer(sizeof(DirectionalLightData::CPU)
                * shadowDirectionalLights.size());
            shadowSpotLightBuffer = builder.createShaderBuffer(sizeof(SpotLightData::CPU)
                * shadowSpotLights.size());
            shadowDirectionalLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f)
                * shadowDirectionalLights.size());
            shadowSpotLightTransformBuffer = builder.createShaderBuffer(sizeof(Mat4f) * shadowSpotLights.size());
        }
        else
        {
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
        if (layerSize != nLayerSize)
        {
            layerSize = nLayerSize;
            RenderGraphTextureBuffer tex;
            tex.format = RGBA;
            tex.size = layerSize;
            layer.color = builder.createTexture(tex);
        }
        else
        {
            layer.color = builder.inheritResource(layer.color);
        }

        layer.depth = gBuffer.gBufferDepth;

        auto layers = registry->get<CompositingLayers>();
        layers.layers.emplace_back(layer);
        registry->set<CompositingLayers>(layers);

        auto pass = builder.addPass("DeferredLightingPass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });

        // If IBL maps are available in the shared registry, declare them as read resources
        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) builder.read(pass, ibl.irradiance);
            if (ibl.prefilter) builder.read(pass, ibl.prefilter);
            if (ibl.brdfLUT) builder.read(pass, ibl.brdfLUT);
        }
    }

    bool DeferredLightingPass::shouldRebuild(const Vec2i& backBufferSize)
    {
        std::vector<PointLightObject> pLights;
        std::vector<PointLightObject> psLights;
        for (auto& object : config->getScene().pointLights)
        {
            if (object.light.castShadows)
            {
                psLights.emplace_back(object);
            }
            else
            {
                pLights.emplace_back(object);
            }
        }

        std::vector<DirectionalLightObject> dLights;
        std::vector<DirectionalLightObject> dsLights;
        for (auto& object : config->getScene().directionalLights)
        {
            if (object.light.castShadows)
            {
                dsLights.emplace_back(object);
            }
            else
            {
                dLights.emplace_back(object);
            }
        }

        std::vector<SpotLightObject> sLights;
        std::vector<SpotLightObject> ssLights;
        for (auto& object : config->getScene().spotLights)
        {
            if (object.light.castShadows)
            {
                ssLights.emplace_back(object);
            }
            else
            {
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

        if (recreateLightBuffers)
        {
            return true;
        }

        return layerSize != backBufferSize * config->getRenderScale();
    }

    void DeferredLightingPass::runPass(RenderGraphContext& ctx)
    {
        if (recreateLightBuffers)
        {
            std::vector<PointLightData::CPU> pointLightData;
            for (auto& lightObject : pointLights)
            {
                PointLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(pointLightBuffer,
                             reinterpret_cast<const uint8_t*>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData::CPU),
                             0);

            pointLightData.clear();
            for (auto& lightObject : shadowPointLights)
            {
                PointLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                pointLightData.emplace_back(data);
            }
            ctx.uploadBuffer(shadowPointLightBuffer,
                             reinterpret_cast<const uint8_t*>(pointLightData.data()),
                             pointLightData.size() * sizeof(PointLightData::CPU),
                             0);

            std::vector<DirectionalLightData::CPU> dirLightData;
            for (auto& lightObject : directionalLights)
            {
                DirectionalLightData::CPU data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                dirLightData.emplace_back(data);
            }
            ctx.uploadBuffer(directionalLightBuffer,
                             reinterpret_cast<const uint8_t*>(dirLightData.data()),
                             dirLightData.size() * sizeof(DirectionalLightData::CPU),
                             0);

            dirLightData.clear();

            std::vector<Mat4f> directionalLightTransforms;
            for (auto& lightObject : shadowDirectionalLights)
            {
                DirectionalLightData::CPU data{};
                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction = Vec4f(direction.x,
                                       direction.y,
                                       direction.z,
                                       0);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                dirLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowDirectionalLightBuffer,
                             reinterpret_cast<const uint8_t*>(dirLightData.data()),
                             dirLightData.size() * sizeof(DirectionalLightData::CPU),
                             0);
            ctx.uploadBuffer(shadowDirectionalLightTransformBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);


            std::vector<SpotLightData::CPU> spotLightData;
            for (auto& lightObject : spotLights)
            {
                SpotLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear);
                spotLightData.emplace_back(data);
            }
            ctx.uploadBuffer(spotLightBuffer,
                             reinterpret_cast<const uint8_t*>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData::CPU),
                             0);

            spotLightData.clear();
            directionalLightTransforms.clear();

            for (auto& lightObject : shadowSpotLights)
            {
                SpotLightData::CPU data{};
                data.position = Vec4f(lightObject.transform.getPosition().x,
                                      lightObject.transform.getPosition().y,
                                      lightObject.transform.getPosition().z,
                                      0);

                auto direction = lightObject.light.getDirection(lightObject.transform);
                data.direction_quadratic = Vec4f(direction.x,
                                                 direction.y,
                                                 direction.z,
                                                 lightObject.light.quadratic);
                data.color = (lightObject.light.color.divide() * lightObject.light.power);
                data.farPlane = Vec4f(lightObject.light.shadowFarPlane, 0, 0, 0);
                data.cutOff_outerCutOff_constant_linear = Vec4f(SpotLight::getCutOff(lightObject.light.cutOff),
                                                                SpotLight::getCutOff(lightObject.light.outerCutOff),
                                                                lightObject.light.constant,
                                                                lightObject.light.linear);
                spotLightData.emplace_back(data);
                directionalLightTransforms.emplace_back(lightObject.light.getShadowProjection(lightObject.transform));
            }
            ctx.uploadBuffer(shadowSpotLightBuffer,
                             reinterpret_cast<const uint8_t*>(spotLightData.data()),
                             spotLightData.size() * sizeof(SpotLightData::CPU),
                             0);
            ctx.uploadBuffer(shadowSpotLightTransformBuffer,
                             reinterpret_cast<const uint8_t*>(directionalLightTransforms.data()),
                             directionalLightTransforms.size() * sizeof(Mat4f),
                             0);
            recreateLightBuffers = false;
        }

        if (!normalizedQuadUploaded)
        {
            ctx.uploadBuffer(vertexBuffer, normalizedQuad.vertices.data(), normalizedQuad.vertices.size(), 0);
            normalizedQuadUploaded = true;
        }

        ShaderData::CPU shaderData;
        shaderData.viewPosition_gamma = Vec4f(viewPosition.x,
                                              viewPosition.y,
                                              viewPosition.z,
                                              config->getGamma());
        if (registry->check<IBLMaps>())
        {
            shaderData.iblPresent_prefilterMipCount = Vec4i(true,
                RenderGraphTextureBuffer::calculateMipLevels(config->iblPrefilterSize),
                0,
                0);
        }
        else
        {
            shaderData.iblPresent_prefilterMipCount = Vec4i(false, 0, 0, 0);
        }
        ctx.uploadBuffer(shaderDataBuffer,
                         reinterpret_cast<const uint8_t*>(&shaderData),
                         sizeof(ShaderData::CPU),
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

        if (registry->check<IBLMaps>())
        {
            auto ibl = registry->get<IBLMaps>();
            if (ibl.irradiance) ctx.bindTexture("iblIrradiance", ibl.irradiance);
            if (ibl.prefilter) ctx.bindTexture("iblPrefilter", ibl.prefilter);
            if (ibl.brdfLUT) ctx.bindTexture("iblBRDF", ibl.brdfLUT);
        }

        ctx.drawArray(DrawCall(0, 6));

        ctx.endRenderPass();
    }
}
