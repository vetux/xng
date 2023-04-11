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

#include "xng/render/graph/passes/phongdeferredpass.hpp"

#include "xng/render/graph/framegraphbuilder.hpp"

#include "xng/render/graph/passes/gbufferpass.hpp"
#include "xng/render/graph/framegraphproperties.hpp"

#include "xng/render/shaderinclude.hpp"

#include "xng/geometry/vertexstream.hpp"

static const char *SHADER_VERT_GEOMETRY = R"###(#version 460

#include "phong.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

layout(binding = 0, std140) uniform ShaderUniformBuffer {
    vec4 viewPosition;
} globs;

layout(binding = 1, std140) buffer PointLightsData
{
    PointLight lights[];
} pLights;

layout(binding = 2, std140) buffer SpotLightsData
{
    SpotLight lights[];
} sLights;

layout(binding = 3, std140) buffer DirectionalLightsData
{
    DirectionalLight lights[];
} dLights;

layout(binding = 4) uniform sampler2D gBufferPos;
layout(binding = 5) uniform sampler2D gBufferNormal;
layout(binding = 6) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 7) uniform sampler2D gBufferAlbedo;
layout(binding = 8) uniform sampler2D gBufferAmbient;
layout(binding = 9) uniform sampler2D gBufferSpecular;
layout(binding = 10) uniform isampler2D gBufferModelObject;
layout(binding = 11) uniform sampler2D gBufferDepth;

void main()
{
    fPos = vec4(vPosition, 1);
    fUv = vUv;
    gl_Position = fPos;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 460

#include "phong.glsl"

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;
layout(depth_any) out float gl_FragDepth;

layout(binding = 0, std140) uniform ShaderUniformBuffer {
    vec4 viewPosition;
} globs;

layout(binding = 1, std140) buffer PointLightsData
{
    PointLight lights[];
} pLights;

layout(binding = 2, std140) buffer SpotLightsData
{
    SpotLight lights[];
} sLights;

layout(binding = 3, std140) buffer DirectionalLightsData
{
    DirectionalLight lights[];
} dLights;

layout(binding = 4) uniform sampler2D gBufferPos;
layout(binding = 5) uniform sampler2D gBufferNormal;
layout(binding = 6) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 7) uniform sampler2D gBufferAlbedo;
layout(binding = 8) uniform sampler2D gBufferAmbient;
layout(binding = 9) uniform sampler2D gBufferSpecular;
layout(binding = 10) uniform isampler2D gBufferModelObject;
layout(binding = 11) uniform sampler2D gBufferDepth;

void main() {
    int model = texture(gBufferModelObject, fUv).x;
    if (model == 1)
    {
        vec3 fPos = texture(gBufferPos, fUv).xyz;
        vec3 fNorm = texture(gBufferNormal, fUv).xyz;
        vec4 diffuseColor = texture(gBufferAlbedo, fUv);
        vec4 specularColor = texture(gBufferSpecular, fUv);
        float shininess = texture(gBufferRoughnessMetallicAO, fUv).x;

        LightComponents comp;

        for (int i = 0; i < pLights.lights.length(); i++)
        {
            PointLight light = pLights.lights[i];
            LightComponents c = phong_point(fPos,
                                            fNorm,
                                            diffuseColor,
                                            specularColor,
                                            shininess,
                                            globs.viewPosition.xyz,
                                            mat3(1),
                                            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < sLights.lights.length(); i++)
        {
            SpotLight light = sLights.lights[i];
            LightComponents c = phong_spot(fPos,
                                            fNorm,
                                            diffuseColor,
                                            specularColor,
                                            shininess,
                                            globs.viewPosition.xyz,
                                            mat3(1),
                                            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < dLights.lights.length(); i++)
        {
            DirectionalLight light = dLights.lights[i];
            LightComponents c = phong_directional(fPos,
                                                    fNorm,
                                                    diffuseColor,
                                                    specularColor,
                                                    shininess,
                                                    globs.viewPosition.xyz,
                                                    mat3(1),
                                                    light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        vec3 color = comp.ambient + comp.diffuse + comp.specular;
        oColor = vec4(color, 1);
        gl_FragDepth = texture(gBufferDepth, fUv).r;
    } else {
        oColor = vec4(0, 0, 0, 1);
        gl_FragDepth = 1;
    }
}
)###";

namespace xng {
#pragma pack(push, 1)
    struct DirectionalLight {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> direction;
    };

    struct PointLight {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> position;
        std::array<float, 4> constant_linear_quadratic;
    };

    struct SpotLight {
        std::array<float, 4> ambient;
        std::array<float, 4> diffuse;
        std::array<float, 4> specular;
        std::array<float, 4> position;
        std::array<float, 4> direction_quadratic;
        std::array<float, 4> cutOff_outerCutOff_constant_linear;
    };

    struct UniformBuffer {
        std::array<float, 4> viewPosition{};
    };
#pragma pack(pop)

    std::vector<DirectionalLight> getDirLights(const std::vector<Light> &lights) {
        std::vector<DirectionalLight> ret;
        for (auto &l: lights) {
            auto tmp = DirectionalLight{
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

    std::vector<PointLight> getPointLights(const std::vector<Light> &lights) {
        std::vector<PointLight> ret;
        for (auto &l: lights) {
            auto tmp = PointLight{
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

    std::vector<SpotLight> getSpotLights(const std::vector<Light> &lights) {
        std::vector<SpotLight> ret;
        for (auto &l: lights) {
            auto tmp = SpotLight{
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

    PhongDeferredPass::PhongDeferredPass() = default;

    void PhongDeferredPass::setup(FrameGraphBuilder &builder) {
        if (!vertexBufferRes.assigned) {
            VertexBufferDesc desc;
            desc.size = mesh.vertices.size() * mesh.vertexLayout.getSize();
            vertexBufferRes = builder.createVertexBuffer(desc);

            VertexArrayObjectDesc oDesc;
            oDesc.vertexLayout = mesh.vertexLayout;
            vertexArrayObjectRes = builder.createVertexArrayObject(oDesc);

            builder.write(vertexBufferRes);
        }

        builder.persist(vertexBufferRes);
        builder.persist(vertexArrayObjectRes);
        builder.read(vertexBufferRes);
        builder.read(vertexArrayObjectRes);

        if (!pipelineRes.assigned) {
            auto vs = ShaderSource(SHADER_VERT_GEOMETRY, "main", xng::VERTEX, xng::GLSL_460, false);
            auto fs = ShaderSource(SHADER_FRAG_GEOMETRY, "main", xng::FRAGMENT, xng::GLSL_460, false);

            vs = vs.preprocess(builder.getShaderCompiler(),
                               ShaderInclude::getShaderIncludeCallback(),
                               ShaderInclude::getShaderMacros(GLSL_460));
            fs = fs.preprocess(builder.getShaderCompiler(),
                               ShaderInclude::getShaderIncludeCallback(),
                               ShaderInclude::getShaderMacros(GLSL_460));

            vsb = vs.compile(builder.getShaderCompiler());
            fsb = fs.compile(builder.getShaderCompiler());

            pipelineRes = builder.createPipeline(RenderPipelineDesc{
                    .shaders = {
                            {VERTEX,   vsb.getShader()},
                            {FRAGMENT, fsb.getShader()}
                    },
                    .bindings = {BIND_SHADER_UNIFORM_BUFFER,
                                 BIND_SHADER_STORAGE_BUFFER,
                                 BIND_SHADER_STORAGE_BUFFER,
                                 BIND_SHADER_STORAGE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER,
                                 BIND_TEXTURE_BUFFER},
                    .primitive = TRIANGLES,
                    .vertexLayout = mesh.vertexLayout,
                    .clearColorValue = ColorRGBA(0, 0, 0, 0),
                    .clearColor = true,
                    .clearDepth = true,
                    .enableDepthTest = true,
                    .depthTestWrite = true,
            });
        }

        builder.persist(pipelineRes);
        builder.read(pipelineRes);

        renderSize = builder.getBackBufferDescription().size
                     * builder.getProperties().get<float>(FrameGraphProperties::RENDER_SCALE, 1);

        targetRes = builder.createRenderTarget(RenderTargetDesc{
                .size = renderSize,
                .numberOfColorAttachments = 1,
                .hasDepthStencilAttachment = true
        });
        builder.read(targetRes);

        TextureBufferDesc desc;
        desc.size = renderSize;
        colorTextureRes = builder.createTextureBuffer(desc);
        builder.write(colorTextureRes);

        desc.format = DEPTH_STENCIL;
        depthTextureRes = builder.createTextureBuffer(desc);
        builder.write(depthTextureRes);

        passRes = builder.createRenderPass(RenderPassDesc{
                .numberOfColorAttachments = 1,
                .hasDepthStencilAttachment = true});

        builder.read(passRes);

        uniformBufferRes = builder.createShaderUniformBuffer(ShaderUniformBufferDesc{.size =  sizeof(UniformBuffer)});
        builder.read(uniformBufferRes);
        builder.write(uniformBufferRes);

        pointLights.clear();
        spotLights.clear();
        directionalLights.clear();
        for (auto &l: builder.getScene().lights) {
            switch (l.type) {
                case LIGHT_POINT:
                    pointLights.emplace_back(l);
                    break;
                case LIGHT_SPOT:
                    spotLights.emplace_back(l);
                    break;
                case LIGHT_DIRECTIONAL:
                    directionalLights.emplace_back(l);
                    break;
            }
        }

        pointLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(PointLight) * pointLights.size()
        });
        builder.read(pointLightsBufferRes);
        builder.write(pointLightsBufferRes);

        spotLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(SpotLight) * spotLights.size()
        });
        builder.read(spotLightsBufferRes);
        builder.write(spotLightsBufferRes);

        directionalLightsBufferRes = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(DirectionalLight) * directionalLights.size()
        });
        builder.read(directionalLightsBufferRes);
        builder.write(directionalLightsBufferRes);

        gBufferPosition = builder.getSharedData().get<FrameGraphResource>(
                GBufferPass::GEOMETRY_BUFFER_POSITION);
        builder.read(gBufferPosition);

        gBufferNormal = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_NORMAL);
        builder.read(gBufferNormal);

        gBufferTangent = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_TANGENT);
        builder.read(gBufferTangent);

        gBufferRoughnessMetallicAO = builder.getSharedData().get<FrameGraphResource>(
                GBufferPass::GEOMETRY_BUFFER_ROUGHNESS_METALLIC_AO);
        builder.read(gBufferRoughnessMetallicAO);

        gBufferAlbedo = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_ALBEDO);
        builder.read(gBufferAlbedo);

        gBufferAmbient = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_AMBIENT);
        builder.read(gBufferAmbient);

        gBufferSpecular = builder.getSharedData().get<FrameGraphResource>(
                GBufferPass::GEOMETRY_BUFFER_SPECULAR);
        builder.read(gBufferSpecular);

        gBufferModelObject = builder.getSharedData().get<FrameGraphResource>(
                GBufferPass::GEOMETRY_BUFFER_MODEL_OBJECT);
        builder.read(gBufferModelObject);

        gBufferDepth = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_DEPTH);
        builder.read(gBufferDepth);

        cameraTransform = builder.getScene().cameraTransform;

        builder.getSharedData().set(COLOR, colorTextureRes);
        builder.getSharedData().set(DEPTH, depthTextureRes);
    }

    void PhongDeferredPass::execute(FrameGraphPassResources &resources) {
        auto &target = resources.get<RenderTarget>(targetRes);

        auto &pipeline = resources.get<RenderPipeline>(pipelineRes);
        auto &pass = resources.get<RenderPass>(passRes);

        auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
        auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);

        auto &uniformBuffer = resources.get<ShaderUniformBuffer>(uniformBufferRes);

        auto &pointLightBuffer = resources.get<ShaderStorageBuffer>(pointLightsBufferRes);
        auto &spotLightBuffer = resources.get<ShaderStorageBuffer>(spotLightsBufferRes);
        auto &dirLightBuffer = resources.get<ShaderStorageBuffer>(directionalLightsBufferRes);

        auto &colorTex = resources.get<TextureBuffer>(colorTextureRes);
        auto &depthTex = resources.get<TextureBuffer>(depthTextureRes);

        auto plights = getPointLights(pointLights);
        auto slights = getSpotLights(spotLights);
        auto dlights = getDirLights(directionalLights);

        pointLightBuffer.upload(reinterpret_cast<const uint8_t *>(plights.data()), plights.size() * sizeof(PointLight));
        spotLightBuffer.upload(reinterpret_cast<const uint8_t *>(slights.data()), slights.size() * sizeof(SpotLight));
        dirLightBuffer.upload(reinterpret_cast<const uint8_t *>(dlights.data()),
                              dlights.size() * sizeof(DirectionalLight));

        if (!quadAllocated) {
            quadAllocated = true;
            auto verts = VertexStream().addVertices(mesh.vertices).getVertexBuffer();
            vertexBuffer.upload(0,
                                verts.data(),
                                verts.size());
            vertexArrayObject.bindBuffers(vertexBuffer);
        }

        UniformBuffer buf;
        buf.viewPosition = Vec4f(cameraTransform.getPosition().x,
                                 cameraTransform.getPosition().y,
                                 cameraTransform.getPosition().z,
                                 0).getMemory();
        uniformBuffer.upload(buf);

        auto &gBufPos = resources.get<TextureBuffer>(gBufferPosition);
        auto &gBufNorm = resources.get<TextureBuffer>(gBufferNormal);
        auto &gBufTan = resources.get<TextureBuffer>(gBufferTangent);
        auto &gBufRoughnessMetallicAO = resources.get<TextureBuffer>(gBufferRoughnessMetallicAO);
        auto &gBufAlbedo = resources.get<TextureBuffer>(gBufferAlbedo);
        auto &gBufAmbient = resources.get<TextureBuffer>(gBufferAmbient);
        auto &gBufSpecular = resources.get<TextureBuffer>(gBufferSpecular);
        auto &gBufModelObject = resources.get<TextureBuffer>(gBufferModelObject);
        auto &gBufDepth = resources.get<TextureBuffer>(gBufferDepth);

        target.setColorAttachments({colorTex});
        target.setDepthStencilAttachment(&depthTex);

        pass.beginRenderPass(target, {}, target.getDescription().size);

        pass.bindPipeline(pipeline);
        pass.bindVertexArrayObject(vertexArrayObject);
        pass.bindShaderData({
                                    uniformBuffer,
                                    pointLightBuffer,
                                    spotLightBuffer,
                                    dirLightBuffer,
                                    gBufPos,
                                    gBufNorm,
                                    gBufRoughnessMetallicAO,
                                    gBufAlbedo,
                                    gBufAmbient,
                                    gBufSpecular,
                                    gBufModelObject,
                                    gBufDepth
                            });
        pass.drawArray(RenderPass::DrawCall{.offset = 0, .count = mesh.vertices.size()});
        pass.endRenderPass();

        target.setColorAttachments({});
        target.setDepthStencilAttachment(nullptr);
    }

    std::type_index PhongDeferredPass::getTypeIndex() const {
        return typeid(PhongDeferredPass);
    }
}