/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#pragma message "Not Implemented"

#include "xng/render/graph/passes/gbufferpass.hpp"

#include "xng/resource/resourceregistry.hpp"
#include "xng/io/archive/memoryarchive.hpp"

#include "xng/render/graph/framegraphbuilder.hpp"
#include "xng/render/graph/framegraphproperties.hpp"

#include "xng/io/protocol/jsonprotocol.hpp"

#include "xng/resource/parsers/jsonparser.hpp"

static const char *SHADER_VERT_GEOMETRY = R"###(#version 420 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in vec4 vInstanceRow0;
layout (location = 6) in vec4 vInstanceRow1;
layout (location = 7) in vec4 vInstanceRow2;
layout (location = 8) in vec4 vInstanceRow3;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec4 vPos;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 model;
    mat4 mvp;

    int hasTextureNormal;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;
    float shininessColor;
    vec4 emissiveColor;
} globs;

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D ambient;
layout(binding = 3) uniform sampler2D specular;
layout(binding = 4) uniform sampler2D shininess;
layout(binding = 5) uniform sampler2D emissive;
layout(binding = 6) uniform sampler2D normal;

void main()
{
    mat4 instanceMatrix = mat4(vInstanceRow0, vInstanceRow1, vInstanceRow2, vInstanceRow3);

    vPos = (instanceMatrix * globs.mvp) * vec4(vPosition, 1);
    fPos = ((instanceMatrix * globs.model) * vec4(vPosition, 1)).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    gl_Position = vPos;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 420 core

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oTangent;
layout(location = 3) out vec4 oRoughnessMetallicAO;
layout(location = 4) out vec4 oAlbedo;
layout(location = 5) out vec4 oAmbient;
layout(location = 6) out vec4 oSpecular;
layout(location = 7) out vec4 oModelObject;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 model;
    mat4 mvp;

    int shadeModel;
    int objectID;

    int hasTextureNormal;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;
    float shininessColor;
} globs;

layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D diffuse;
layout(binding = 3) uniform sampler2D ambient;
layout(binding = 4) uniform sampler2D specular;
layout(binding = 5) uniform sampler2D shininess;

void main() {
    oPosition = vec4(fPos, 1);
    oDiffuse = texture(diffuse, fUv) + globs.diffuseColor;
    oAmbient = texture(ambient, fUv) + globs.ambientColor;
    oSpecular = texture(specular, fUv) + globs.specularColor;
    oShininess.r = texture(shininess, fUv).r + globs.shininessColor;

    mat3 normalMatrix = transpose(inverse(mat3(globs.model)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (globs.hasTextureNormal != 0)
    {
        vec3 texNormal = texture(normal, fUv).xyz;
        texNormal = normalize(texNormal * 2.0 - 1.0);
        oTexNormal = vec4(texNormal, 1);
    }
    else
    {
        oTexNormal = vec4(0, 0, 0, 0);
    }

    oModelObject.r = globs.shadeModel;
    oModelObject.g = globs.objectID;
}
)###";

static const char *SHADER_FRAG_GEOMETRY_PBR = R"###(#version 420 core

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oTangent;
layout(location = 3) out vec4 oRoughnessMetallicAO;
layout(location = 4) out vec4 oAlbedo;
layout(location = 5) out vec4 oAmbient;
layout(location = 6) out vec4 oSpecular;
layout(location = 7) out vec4 oModelObject;

struct PBRObjectData {
    mat4 model;
    mat4 mvp;

    int shadeModel;
    int objectID;

    int hasTextureNormal;

    vec4 albedo;
    float metallic;
    float roughness;
    float ambientOcclusion;
}

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PBRObjectData data[
} globs;

layout(binding = 1) uniform sampler2D normal;

layout(binding = 2) uniform sampler2D albedo;
layout(binding = 3) uniform sampler2D metallic;
layout(binding = 4) uniform sampler2D roughness;
layout(binding = 5) uniform sampler2D ambientOcclusion;

void main() {
    oPosition = vec4(fPos, 1);
    oAlbedo = texture(albedo, fUv) + globs.albedo;
    oRoughnessMetallicAO.r = texture(roughness, fUv) + globs.roughness;
    oRoughnessMetallicAO.g = texture(metallic, fUv).r + globs.metallic;
    oRoughnessMetallicAO.b = texture(ambientOcclusion, fUv).r + globs.ambientOcclusion;

    mat3 normalMatrix = transpose(inverse(mat3(globs.model)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (globs.hasTextureNormal != 0)
    {
        vec3 texNormal = texture(normal, fUv).xyz;
        texNormal = normalize(texNormal * 2.0 - 1.0);
        oTexNormal = vec4(texNormal, 1);
    }
    else
    {
        oTexNormal = vec4(0, 0, 0, 0);
    }

    oModelObject.r = globs.shadeModel;
    oModelObject.g = globs.objectID;
}
)###";

static xng::Shader phongShader = xng::Shader(xng::ShaderSource(SHADER_VERT_GEOMETRY,
                                                               "main",
                                                               xng::VERTEX,
                                                               xng::GLSL_420,
                                                               false),
                                             xng::ShaderSource(SHADER_FRAG_GEOMETRY,
                                                               "main",
                                                               xng::FRAGMENT,
                                                               xng::GLSL_420,
                                                               false),
                                             xng::ShaderSource());

static xng::Shader pbrShader = xng::Shader(xng::ShaderSource(SHADER_VERT_GEOMETRY,
                                                             "main",
                                                             xng::VERTEX,
                                                             xng::GLSL_420,
                                                             false),
                                           xng::ShaderSource(SHADER_FRAG_GEOMETRY,
                                                             "main",
                                                             xng::FRAGMENT,
                                                             xng::GLSL_420,
                                                             false),
                                           xng::ShaderSource());

static bool shaderLoaded = false;

static const xng::Uri SHADER_GEOMETRY_PHONG_URI = xng::Uri("gbufferpass/phong");
static const xng::Uri SHADER_GEOMETRY_PBR_URI = xng::Uri("gbufferpass/pbr");

static void loadShaderResources() {
    if (!shaderLoaded) {
        shaderLoaded = true;

        // Phong Shader
        xng::ResourceBundle bundle;
        bundle.add("shader", std::make_unique<xng::Shader>(phongShader));

        auto msg = xng::JsonParser::createBundle(bundle);

        std::stringstream stream;
        xng::JsonProtocol().serialize(stream, msg);

        std::vector<uint8_t> vec;
        for (auto c: stream.str()) {
            vec.emplace_back(c);
        }

        auto &a = dynamic_cast<xng::MemoryArchive &>(xng::ResourceRegistry::getDefaultRegistry().getArchive("memory"));
        a.addData(SHADER_GEOMETRY_PHONG_URI.toString(), vec);

        // PBR Shader
        bundle = {};
        bundle.add("shader", std::make_unique<xng::Shader>(pbrShader));

        msg = xng::JsonParser::createBundle(bundle);

        stream = {};
        xng::JsonProtocol().serialize(stream, msg);

        vec.clear();
        for (auto c: stream.str()) {
            vec.emplace_back(c);
        }

        a.addData(SHADER_GEOMETRY_PBR_URI.toString(), vec);
    }
}

namespace xng {
    struct ShaderBufferDataPBR {
        Mat4f model;
        Mat4f mvp;

        int shadeModel;
        uint objectID;

        int hasTextureNormal;

        std::array<float, 4> albedo;
        float metallic;
        float roughness;
        float ambientOcclusion;
    };

    struct ShaderBufferDataPhong {
        Mat4f model;
        Mat4f mvp;

        int shadeModel;
        uint objectID;

        int hasTextureNormal;

        std::array<float, 4> diffuseColor;
        std::array<float, 4> ambientColor;
        std::array<float, 4> specularColor;
        float shininessColor;
    };

    GBufferPass::GBufferPass() {
        loadShaderResources();
    }

    void GBufferPass::setup(FrameGraphBuilder &builder) {
        auto renderSize = builder.getBackBufferFormat().first *
                          builder.getProperties().get<float>(FrameGraphProperties::RENDER_SCALE, 1);
        renderTarget = builder.createRenderTarget(RenderTargetDesc{
                .size = renderSize,
                .multisample = false,
                .samples = 0,
                .numberOfColorAttachments = 8,
                .hasDepthStencilAttachment = true,
        });
        phongPipeline = builder.createPipeline(ResourceHandle<Shader>(Uri(SHADER_GEOMETRY_PHONG_URI)),
                                               RenderPipelineDesc{
                                                       .viewportOffset = {},
                                                       .viewportSize = renderSize,
                                                       .clearColorValue = ColorRGBA(0, 0, 0, 0),
                                                       .clearColor = true,
                                                       .clearDepth = true,
                                                       .enableDepthTest = true,
                                                       .depthTestWrite = true,
                                                       .depthTestMode = DEPTH_TEST_LESS,
                                                       .enableFaceCulling = true,
                                                       .enableBlending = true
                                               });
        pbrPipeline = builder.createPipeline(ResourceHandle<Shader>(Uri(SHADER_GEOMETRY_PBR_URI)),
                                             RenderPipelineDesc{
                                                     .viewportOffset = {},
                                                     .viewportSize = renderSize,
                                                     .clearColor = false,
                                                     .clearDepth = false,
                                                     .enableDepthTest = true,
                                                     .depthTestWrite = true,
                                                     .depthTestMode = DEPTH_TEST_LESS,
                                                     .enableFaceCulling = true,
                                                     .enableBlending = true
                                             });
        pbrShaderBuffer = builder.createShaderBuffer(ShaderBufferDesc{
                .size = sizeof(ShaderBufferDataPBR),
                .bufferType = RenderBufferType::HOST_VISIBLE
        });

        phongShaderBuffer = builder.createShaderBuffer(ShaderBufferDesc{
                .size = sizeof(ShaderBufferDataPhong),
                .bufferType = RenderBufferType::HOST_VISIBLE
        });

        auto desc = TextureBufferDesc();
        desc.size = {1, 1};

        defaultTexture = builder.createTextureBuffer(desc);

        desc.size = renderSize;

        desc.format = RGBA32F;
        gBufferPosition = builder.createTextureBuffer(desc);
        gBufferNormal = builder.createTextureBuffer(desc);
        gBufferTangent = builder.createTextureBuffer(desc);
        gBufferRoughnessMetallicAmbientOcclusion = builder.createTextureBuffer(desc);

        desc.format = RGBA;
        gBufferAlbedo = builder.createTextureBuffer(desc);
        gBufferAmbient = builder.createTextureBuffer(desc);
        gBufferSpecular = builder.createTextureBuffer(desc);

        desc.format = RGBA32I;
        gBufferModelObject = builder.createTextureBuffer(desc);

        desc.format = DEPTH_STENCIL;
        gBufferDepth = builder.createTextureBuffer(desc);

        phongObjects.clear();
        pbrObjects.clear();

        auto &objects = builder.getScene().objects;
        for (auto id = 0; id < objects.size(); id++) {
            auto &object = objects.at(id);
            if (object.mesh.assigned()) {
                if (meshBuffers.find(object.mesh.getUri()) == meshBuffers.end()) {
                    meshBuffers[object.mesh.getUri()] = builder.createMeshBuffer(object.mesh);
                }
                if (object.material.assigned()) {
                    auto &mat = object.material.get();
                    if (mat.transparent
                        || mat.shader.assigned())
                        continue;
                    if (mat.normal.assigned()
                        && textureBuffers.find(mat.normal.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.normal.getUri()] = builder.createTextureBuffer(mat.normal);
                    }
                    if (mat.albedoTexture.assigned()
                        && textureBuffers.find(mat.albedoTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.albedoTexture.getUri()] = builder.createTextureBuffer(mat.albedoTexture);
                    }
                    if (mat.metallicTexture.assigned()
                        && textureBuffers.find(mat.metallicTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.metallicTexture.getUri()] = builder.createTextureBuffer(mat.metallicTexture);
                    }
                    if (mat.roughnessTexture.assigned()
                        && textureBuffers.find(mat.roughnessTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.roughnessTexture.getUri()] = builder.createTextureBuffer(
                                mat.roughnessTexture);
                    }
                    if (mat.ambientOcclusionTexture.assigned()
                        && textureBuffers.find(mat.ambientOcclusionTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.ambientOcclusionTexture.getUri()] = builder.createTextureBuffer(
                                mat.ambientOcclusionTexture);
                    }
                    if (mat.diffuseTexture.assigned()
                        && textureBuffers.find(mat.diffuseTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.diffuseTexture.getUri()] = builder.createTextureBuffer(mat.diffuseTexture);
                    }
                    if (mat.ambientTexture.assigned()
                        && textureBuffers.find(mat.ambientTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.ambientTexture.getUri()] = builder.createTextureBuffer(mat.ambientTexture);
                    }
                    if (mat.specularTexture.assigned()
                        && textureBuffers.find(mat.specularTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.specularTexture.getUri()] = builder.createTextureBuffer(mat.specularTexture);
                    }
                    if (mat.shininessTexture.assigned()
                        && textureBuffers.find(mat.shininessTexture.getUri()) == textureBuffers.end()) {
                        textureBuffers[mat.shininessTexture.getUri()] = builder.createTextureBuffer(
                                mat.shininessTexture);
                    }

                    if (mat.shadingModel == SHADE_PBR) {
                        pbrObjects.emplace_back(std::pair<uint, Scene::Object>{id, object});
                    } else {
                        phongObjects.emplace_back(std::pair<uint, Scene::Object>{id, object});
                    }
                }
            }
        }

        camera = builder.getScene().camera;
        cameraTransform = builder.getScene().cameraTransform;
    }

    void GBufferPass::execute(FrameGraphPassResources &resources) {
        auto &target = resources.getRenderTarget(renderTarget);
        auto &phongPip = resources.getPipeline(phongPipeline);
        auto &pbrPip = resources.getPipeline(pbrPipeline);

        auto &phongBuffer = resources.getShaderBuffer(phongShaderBuffer);
        auto &pbrBuffer = resources.getShaderBuffer(pbrShaderBuffer);

        auto &defaultTex = resources.getTextureBuffer(defaultTexture);

        auto &posTex = resources.getTextureBuffer(gBufferPosition);
        auto &normalTex = resources.getTextureBuffer(gBufferNormal);
        auto &tanTex = resources.getTextureBuffer(gBufferTangent);
        auto &roughMetallicAOTex = resources.getTextureBuffer(gBufferRoughnessMetallicAmbientOcclusion);
        auto &albedoTex = resources.getTextureBuffer(gBufferAlbedo);
        auto &ambientTex = resources.getTextureBuffer(gBufferAmbient);
        auto &specularTex = resources.getTextureBuffer(gBufferSpecular);
        auto &modelObjectTex = resources.getTextureBuffer(gBufferModelObject);
        auto &depthTex = resources.getTextureBuffer(gBufferDepth);

        target.setColorAttachments({
                                           posTex,
                                           normalTex,
                                           tanTex,
                                           roughMetallicAOTex,
                                           albedoTex,
                                           ambientTex,
                                           specularTex,
                                           modelObjectTex
                                   });

        target.setDepthStencilAttachment(&depthTex);

        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        std::vector<RenderCommand> commands;
        for (auto &pair: phongObjects) {
            auto &object = pair.second;
            auto model = object.transform.model();
            auto &mat = object.material.get();
            auto data = ShaderBufferDataPhong{
                    .model = model,
                    .mvp = model * view * projection,
                    .shadeModel = mat.shadingModel,
                    .objectID = pair.first,
                    .hasTextureNormal = mat.normal.assigned(),
                    .diffuseColor = mat.diffuse.divide().getMemory(),
                    .ambientColor = mat.ambient.divide().getMemory(),
                    .specularColor = mat.specular.divide().getMemory(),
                    .shininessColor = mat.shininess
            };
            phongBuffer.upload(data);
            commands.emplace_back(RenderCommand(resources.getMeshBuffer(meshBuffers.at(object.mesh.getUri())),
                                                {
                                                        &phongBuffer,
                                                        mat.normal.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.normal.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.diffuseTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.diffuseTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.ambientTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.ambientTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.specularTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.specularTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.shininessTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.shininessTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                }));
        }
        phongPip.render(target, commands);

        commands.clear();
        for (auto &pair: pbrObjects) {
            auto &object = pair.second;
            auto model = object.transform.model();
            auto &mat = object.material.get();
            auto data = ShaderBufferDataPBR{
                    .model = model,
                    .mvp = model * view * projection,
                    .shadeModel = mat.shadingModel,
                    .objectID = pair.first,
                    .hasTextureNormal = mat.normal.assigned(),
                    .albedo = mat.albedo.divide().getMemory(),
                    .metallic = mat.metallic,
                    .roughness = mat.roughness,
                    .ambientOcclusion = mat.ambientOcclusion
            };
            pbrBuffer.upload(data);
            commands.emplace_back(RenderCommand(resources.getMeshBuffer(meshBuffers.at(object.mesh.getUri())),
                                                {
                                                        &pbrBuffer,
                                                        mat.normal.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.normal.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.diffuseTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.diffuseTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.ambientTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.ambientTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.specularTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.specularTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                        mat.shininessTexture.assigned()
                                                        ? &resources.getTextureBuffer(
                                                                textureBuffers.at(mat.shininessTexture.getUri()))
                                                        : &resources.getTextureBuffer(defaultTexture),
                                                }));
        }
        pbrPip.render(target, commands);

        target.setColorAttachments({});
        target.setDepthStencilAttachment(nullptr);
    }

    std::type_index GBufferPass::getTypeIndex() {
        return typeid(GBufferPass);
    }
}
/*

#include "xng/render/graph/passes/gbufferpass.hpp"

#include "xng/render/shaderinclude.hpp"

#include "xng/io/archive/memoryarchive.hpp"

static const char *SHADER_VERT_GEOMETRY = R"###(#version 420 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in vec4 vInstanceRow0;
layout (location = 6) in vec4 vInstanceRow1;
layout (location = 7) in vec4 vInstanceRow2;
layout (location = 8) in vec4 vInstanceRow3;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec4 vPos;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 MANA_M;
    mat4 MANA_MVP;

    int hasTextureNormal;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;
    float shininessColor;
    vec4 emissiveColor;

    mat4 TRANSFORM_ROTATION;
} globs;

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D ambient;
layout(binding = 3) uniform sampler2D specular;
layout(binding = 4) uniform sampler2D shininess;
layout(binding = 5) uniform sampler2D emissive;
layout(binding = 6) uniform sampler2D normal;

void main()
{
    mat4 instanceMatrix = mat4(vInstanceRow0, vInstanceRow1, vInstanceRow2, vInstanceRow3);

    vPos = (instanceMatrix * globs.MANA_MVP) * vec4(vPosition, 1);
    fPos = ((instanceMatrix * globs.MANA_M) * vec4(vPosition, 1)).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    gl_Position = vPos;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 420 core

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oTangent;
layout(location = 3) out vec4 oTexNormal;
layout(location = 4) out vec4 oDiffuse;
layout(location = 5) out vec4 oAmbient;
layout(location = 6) out vec4 oSpecular;
layout(location = 7) out vec4 oShininess;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 MANA_M;
    mat4 MANA_MVP;

    int hasTextureNormal;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;
    float shininessColor;
    vec4 emissiveColor;
} globs;

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D ambient;
layout(binding = 3) uniform sampler2D specular;
layout(binding = 4) uniform sampler2D shininess;
layout(binding = 5) uniform sampler2D emissive;
layout(binding = 6) uniform sampler2D normal;

void main() {
    oPosition = vec4(fPos, 1);
    oDiffuse = texture(diffuse, fUv) + globs.diffuseColor;
    oAmbient = texture(ambient, fUv) + globs.ambientColor;
    oSpecular = texture(specular, fUv) + globs.specularColor;
    oShininess.r = texture(shininess, fUv).r + globs.shininessColor;

    mat3 normalMatrix = transpose(inverse(mat3(globs.MANA_M)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (globs.hasTextureNormal != 0)
    {
        vec3 texNormal = texture(normal, fUv).xyz;
        texNormal = normalize(texNormal * 2.0 - 1.0);
        oTexNormal = vec4(texNormal, 1);
    }
    else
    {
        oTexNormal = vec4(0, 0, 0, 0);
    }
}
)###";

namespace xng {
    static bool shaderLoaded = false;

    static const Uri shaderUri = Uri("memory", "/shaders/gbufferpass", "");

    static void initShaderResource() {
        if (!shaderLoaded) {
            shaderLoaded = true;
            Shader shaderSrc;
            shaderSrc.vertexShader = ShaderSource(SHADER_VERT_GEOMETRY, "main", VERTEX, GLSL_420, false);
            shaderSrc.fragmentShader = ShaderSource(SHADER_FRAG_GEOMETRY, "main", FRAGMENT, GLSL_420, false);

            shaderSrc.vertexShader.preprocess();
            shaderSrc.fragmentShader.preprocess();
        }
    }

    static inline Vec4f scaleColor(const ColorRGBA &color) {
        return {static_cast<float>(color.r()) / 255,
                static_cast<float>(color.g()) / 255,
                static_cast<float>(color.b()) / 255,
                static_cast<float>(color.a()) / 255};
    }

    GBufferPass::GBufferPass(RenderDevice &device)
            : device(device) {
        ShaderProgramDesc desc;

        auto defaultImage = ImageRGBA(1,
                                      1,
                                      std::vector<ColorRGBA>({ColorRGBA::black()}));

        defaultTexture = device.getAllocator().createTextureBuffer({.size = {1, 1}, .samples = 1});
        defaultTexture->upload(defaultImage);
    }

    void
    GBufferPass::setup(FrameGraphBuilder &builder, const GenericMapString &properties, GenericMapString &sharedData) {
        scene = builder.getScene();

        for (auto &object: scene.objects) {
            sceneResources[object.mesh.getUri()] = builder.createMeshBuffer(object.mesh);
            if (object.material.get().diffuseTexture)
                sceneResources[object.material.get().diffuseTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().diffuseTexture);
            if (object.material.get().ambientTexture)
                sceneResources[object.material.get().ambientTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().ambientTexture);
            if (object.material.get().specularTexture)
                sceneResources[object.material.get().specularTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().specularTexture);
            if (object.material.get().emissiveTexture)
                sceneResources[object.material.get().emissiveTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().emissiveTexture);
            if (object.material.get().shininessTexture)
                sceneResources[object.material.get().shininessTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().shininessTexture);
            if (object.material.get().normalTexture)
                sceneResources[object.material.get().normalTexture.getUri()] = builder.createTextureBuffer(
                        object.material.get().normalTexture);
        }

        auto format = builder.getRenderFormat();
        if (format.first != gBuffer.getSize() || format.second != gBuffer.getSamples()) {
            gBuffer = GBuffer(device, format.first, format.second);
        }
        renderTarget = builder.createRenderTarget(format.first, format.second);
    }

    void GBufferPass::execute(FrameGraphPassResources &resources) {
        auto &target = resources.getRenderTarget(renderTarget);

        bindTextures(target, gBuffer);

        Mat4f model, view, projection;
        view = scene.camera.view();
        projection = scene.camera.projection();

        shader->activate();
        shader->setTexture(8, 0);
        shader->setTexture(9, 1);
        shader->setTexture(10, 2);
        shader->setTexture(11, 3);
        shader->setTexture(12, 4);
        shader->setTexture(13, 5);

        std::vector<std::reference_wrapper<TextureBuffer>> textures;
        textures.reserve(6);

        bool firstCommand = true;
        Material shaderMaterial;

        ren.renderBegin(target, RenderOptions({}, target.getSize(), true));

        for (auto &object: scene.objects) {
            auto &mesh = resources.getMeshBuffer(sceneResources.at(object.mesh.getUri()));

            textures.clear();

            if (object.material.get().diffuseTexture) {
                auto &diffuseTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().diffuseTexture.getUri()));
                if (firstCommand || shaderMaterial.diffuse != ColorRGBA()) {
                    shaderMaterial.diffuse = ColorRGBA();
                    shader->setVec4(3, Vec4f());
                }
                textures.emplace_back(diffuseTexture);
            } else {
                if (firstCommand || shaderMaterial.diffuse != object.material.get().diffuse) {
                    shaderMaterial.diffuse = object.material.get().diffuse;
                    shader->setVec4(3, scaleColor(object.material.get().diffuse));
                }
                textures.emplace_back(*defaultTexture);
            }

            if (object.material.get().ambientTexture) {
                auto &ambientTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().ambientTexture.getUri()));
                if (firstCommand || shaderMaterial.ambient != ColorRGBA()) {
                    shaderMaterial.ambient = ColorRGBA();
                    shader->setVec4(4, Vec4f());
                }
                textures.emplace_back(ambientTexture);
            } else {
                if (firstCommand || shaderMaterial.ambient != object.material.get().ambient) {
                    shaderMaterial.ambient = object.material.get().ambient;
                    shader->setVec4(4, scaleColor(object.material.get().ambient));
                }
                textures.emplace_back(*defaultTexture);
            }

            if (object.material.get().specularTexture) {
                auto &specularTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().specularTexture.getUri()));
                if (firstCommand || shaderMaterial.specular != ColorRGBA()) {
                    shaderMaterial.specular = ColorRGBA();
                    shader->setVec4(5, Vec4f());
                }
                textures.emplace_back(specularTexture);
            } else {
                if (firstCommand || shaderMaterial.specular != object.material.get().specular) {
                    shaderMaterial.specular = object.material.get().specular;
                    shader->setVec4(5, scaleColor(object.material.get().specular));
                }
                textures.emplace_back(*defaultTexture);
            }

            if (object.material.get().shininessTexture) {
                auto &shininessTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().shininessTexture.getUri()));
                if (firstCommand || shaderMaterial.shininess != 0) {
                    shaderMaterial.shininess = 0;
                    shader->setFloat(6, 0);
                }
                textures.emplace_back(shininessTexture);
            } else {
                if (firstCommand || shaderMaterial.shininess != object.material.get().shininess) {
                    shaderMaterial.shininess = object.material.get().shininess;
                    shader->setFloat(6, object.material.get().shininess);
                }
                textures.emplace_back(*defaultTexture);
            }

            if (object.material.get().emissiveTexture) {
                auto &emissiveTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().emissiveTexture.getUri()));
                if (firstCommand || shaderMaterial.emissive != ColorRGBA()) {
                    shaderMaterial.emissive = ColorRGBA();
                    shader->setVec4(7, Vec4f());
                }
                textures.emplace_back(emissiveTexture);
            } else {
                if (firstCommand || shaderMaterial.emissive != object.material.get().emissive) {
                    shaderMaterial.emissive = object.material.get().emissive;
                    shader->setVec4(7, scaleColor(object.material.get().emissive));
                }
                textures.emplace_back(*defaultTexture);
            }

            if (firstCommand
                || shaderMaterial.normalTexture != object.material.get().normalTexture) {
                shaderMaterial.normalTexture = object.material.get().normalTexture;
                shader->setInt(2, (object.material.get().normalTexture));
            }

            if (object.material.get().normalTexture) {
                auto &normalTexture = resources.getTextureBuffer(
                        sceneResources.at(object.material.get().normalTexture.getUri()));
                textures.emplace_back(normalTexture);
            }

            model = object.transform.model();

            shader->setMat4(0, model);
            shader->setMat4(1, projection * view * model);

            RenderCommand c(*shader, mesh);
            c.textures = textures;
            c.properties.enableFaceCulling = true;
            ren.addCommand(c);

            firstCommand = false;
        }

        ren.renderFinish();

        ren.renderClear(target, ColorRGBA::blue(), 0);

        unbindTextures(target);

        board.set<GBuffer>(gBuffer);
    }
}
*/