/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "render/graph/passes/gbufferpass.hpp"

#include "render/shader/shaderinclude.hpp"

static const char *SHADER_VERT_GEOMETRY = R"###(#version 460

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

layout(location = 0) uniform mat4 MANA_M;
layout(location = 1) uniform mat4 MANA_MVP;

layout(location = 2) uniform int hasTextureNormal;

layout(location = 3) uniform vec4 diffuseColor;
layout(location = 4) uniform vec4 ambientColor;
layout(location = 5) uniform vec4 specularColor;
layout(location = 6) uniform float shininessColor;

layout(location = 7) uniform vec4 emissiveColor;

layout(location = 8) uniform sampler2D diffuse;
layout(location = 9) uniform sampler2D ambient;
layout(location = 10) uniform sampler2D specular;
layout(location = 11) uniform sampler2D shininess;
layout(location = 12) uniform sampler2D emissive;

layout(location = 13) uniform sampler2D normal;

layout(location = 14) uniform mat4 TRANSFORM_ROTATION;

void main()
{
    mat4 instanceMatrix = mat4(vInstanceRow0, vInstanceRow1, vInstanceRow2, vInstanceRow3);

    vPos = (instanceMatrix * MANA_MVP) * vec4(vPosition, 1);
    fPos = ((instanceMatrix * MANA_M) * vec4(vPosition, 1)).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    gl_Position = vPos;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 460

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

layout(location = 0) uniform mat4 MANA_M;
layout(location = 1) uniform mat4 MANA_MVP;

layout(location = 2) uniform int hasTextureNormal;

layout(location = 3) uniform vec4 diffuseColor;
layout(location = 4) uniform vec4 ambientColor;
layout(location = 5) uniform vec4 specularColor;
layout(location = 6) uniform float shininessColor;

layout(location = 7) uniform vec4 emissiveColor;

layout(location = 8) uniform sampler2D diffuse;
layout(location = 9) uniform sampler2D ambient;
layout(location = 10) uniform sampler2D specular;
layout(location = 11) uniform sampler2D shininess;
layout(location = 12) uniform sampler2D emissive;

layout(location = 13) uniform sampler2D normal;

void main() {
    oPosition = vec4(fPos, 1);
    oDiffuse = texture(diffuse, fUv) + diffuseColor;
    oAmbient = texture(ambient, fUv) + ambientColor;
    oSpecular = texture(specular, fUv) + specularColor;
    oShininess.r = texture(shininess, fUv).r + shininessColor;

    mat3 normalMatrix = transpose(inverse(mat3(MANA_M)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (hasTextureNormal != 0)
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

namespace xengine {
    static inline Vec4f scaleColor(const ColorRGBA &color) {
        return {static_cast<float>(color.r()) / 255,
                static_cast<float>(color.g()) / 255,
                static_cast<float>(color.b()) / 255,
                static_cast<float>(color.a()) / 255};
    }

    static void bindTextures(RenderTarget &target, GBuffer &gBuffer) {
        target.setNumberOfColorAttachments(GBuffer::GEOMETRY_TEXTURE_END + 1);
        for (int i = GBuffer::GEOMETRY_TEXTURE_BEGIN; i < GBuffer::GEOMETRY_TEXTURE_END; i++) {
            target.attachColor(i, gBuffer.getTexture((GBuffer::GTexture) i));
        }
    }

    GBufferPass::GBufferPass(Scene &scene, RenderDevice &device)
            : scene(scene), device(device), gBuffer(device) {
        shaderSrc.vertexShader = ShaderSource(SHADER_VERT_GEOMETRY, "main", VERTEX, GLSL_410);
        shaderSrc.fragmentShader = ShaderSource(SHADER_FRAG_GEOMETRY, "main", FRAGMENT, GLSL_410);

        shaderSrc.vertexShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                          ShaderInclude::getShaderMacros(GLSL_410));
        shaderSrc.fragmentShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                            ShaderInclude::getShaderMacros(GLSL_410));

        defaultImage = std::make_shared<ImageRGBA>(1,
                                                   1,
                                                   std::vector<ColorRGBA>({ColorRGBA::black()}));
    }

    void GBufferPass::setup(FrameGraphBuilder &builder) {
        Texture tex;
        tex.attributes.size = {1, 1};
        tex.images.emplace_back(ResourceHandle<ImageRGBA>(Uri(),
                                                          nullptr,
                                                          defaultImage));

        defaultTexture = builder.createTextureBuffer(tex);
        for (auto &object: scene.objects) {
            sceneResources[object.mesh->getId()] = builder.createMeshBuffer(*object.mesh);
            sceneResources[object.material->diffuseTexture.get().getId()]
                    = builder.createTextureBuffer(object.material->diffuseTexture.get());
            sceneResources[object.material->ambientTexture.get().getId()] = builder.createTextureBuffer(
                    object.material->ambientTexture.get());
            sceneResources[object.material->specularTexture.get().getId()] = builder.createTextureBuffer(
                    object.material->specularTexture.get());
            sceneResources[object.material->emissiveTexture.get().getId()] = builder.createTextureBuffer(
                    object.material->emissiveTexture.get());
            sceneResources[object.material->shininessTexture.get().getId()] = builder.createTextureBuffer(
                    object.material->shininessTexture.get());
            sceneResources[object.material->normalTexture.get().getId()] = builder.createTextureBuffer(
                    object.material->normalTexture.get());
        }

        shader = builder.createShader(shaderSrc);

        auto format = builder.getRenderFormat();
        if (format.first != gBuffer.getSize() || format.second != gBuffer.getSamples()) {
            gBuffer = GBuffer(device, format.first, format.second);
        }
        renderTarget = builder.createRenderTarget(format.first, format.second);
    }

    void GBufferPass::execute(RenderPassResources &resources, Renderer &ren, FrameGraphBlackboard &board) {
        auto &shaderProgram = resources.getShader(shader);
        auto &target = resources.getRenderTarget(renderTarget);
        auto &defTex = resources.getTextureBuffer(defaultTexture);

        bindTextures(target, gBuffer);

        Mat4f model, view, projection;
        view = scene.camera.view();
        projection = scene.camera.projection();

        shaderProgram.activate();
        shaderProgram.setTexture(8, 0);
        shaderProgram.setTexture(9, 1);
        shaderProgram.setTexture(10, 2);
        shaderProgram.setTexture(11, 3);
        shaderProgram.setTexture(12, 4);
        shaderProgram.setTexture(13, 5);

        std::vector<std::reference_wrapper<TextureBuffer>> textures;
        textures.reserve(6);

        bool firstCommand = true;
        Material shaderMaterial;

        ren.renderBegin(target, RenderOptions({}, target.getSize(), true));

        for (auto &object: scene.objects) {
            auto &mesh = resources.getMeshBuffer(sceneResources.at(object.mesh->getId()));
            auto &diffuseTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->diffuseTexture.get().getId()));
            auto &ambientTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->ambientTexture.get().getId()));
            auto &specularTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->specularTexture.get().getId()));
            auto &emissiveTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->emissiveTexture.get().getId()));
            auto &shininessTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->shininessTexture.get().getId()));
            auto &normalTexture = resources.getTextureBuffer(
                    sceneResources.at(object.material->normalTexture.get().getId()));

            textures.clear();

            if (object.material->diffuseTexture) {
                if (firstCommand || shaderMaterial.diffuse != ColorRGBA()) {
                    shaderMaterial.diffuse = ColorRGBA();
                    shaderProgram.setVec4(3, Vec4f());
                }
                textures.emplace_back(diffuseTexture);
            } else {
                if (firstCommand || shaderMaterial.diffuse != object.material->diffuse) {
                    shaderMaterial.diffuse = object.material->diffuse;
                    shaderProgram.setVec4(3, scaleColor(object.material->diffuse));
                }
                textures.emplace_back(defTex);
            }

            if (object.material->ambientTexture) {
                if (firstCommand || shaderMaterial.ambient != ColorRGBA()) {
                    shaderMaterial.ambient = ColorRGBA();
                    shaderProgram.setVec4(4, Vec4f());
                }
                textures.emplace_back(ambientTexture);
            } else {
                if (firstCommand || shaderMaterial.ambient != object.material->ambient) {
                    shaderMaterial.ambient = object.material->ambient;
                    shaderProgram.setVec4(4, scaleColor(object.material->ambient));
                }
                textures.emplace_back(defTex);
            }

            if (object.material->specularTexture) {
                if (firstCommand || shaderMaterial.specular != ColorRGBA()) {
                    shaderMaterial.specular = ColorRGBA();
                    shaderProgram.setVec4(5, Vec4f());
                }
                textures.emplace_back(specularTexture);
            } else {
                if (firstCommand || shaderMaterial.specular != object.material->specular) {
                    shaderMaterial.specular = object.material->specular;
                    shaderProgram.setVec4(5, scaleColor(object.material->specular));
                }
                textures.emplace_back(defTex);
            }

            if (object.material->shininessTexture) {
                if (firstCommand || shaderMaterial.shininess != 0) {
                    shaderMaterial.shininess = 0;
                    shaderProgram.setFloat(6, 0);
                }
                textures.emplace_back(shininessTexture);
            } else {
                if (firstCommand || shaderMaterial.shininess != object.material->shininess) {
                    shaderMaterial.shininess = object.material->shininess;
                    shaderProgram.setFloat(6, object.material->shininess);
                }
                textures.emplace_back(defTex);
            }

            if (object.material->emissiveTexture) {
                if (firstCommand || shaderMaterial.emissive != ColorRGBA()) {
                    shaderMaterial.emissive = ColorRGBA();
                    shaderProgram.setVec4(7, Vec4f());
                }
                textures.emplace_back(emissiveTexture);
            } else {
                if (firstCommand || shaderMaterial.emissive != object.material->emissive) {
                    shaderMaterial.emissive = object.material->emissive;
                    shaderProgram.setVec4(7, scaleColor(object.material->emissive));
                }
                textures.emplace_back(defTex);
            }

            if (firstCommand
                || shaderMaterial.normalTexture != object.material->normalTexture) {
                shaderMaterial.normalTexture = object.material->normalTexture;
                shaderProgram.setInt(2, (object.material->normalTexture));
            }

            if (object.material->normalTexture) {
                textures.emplace_back(normalTexture);
            }

            model = object.transform.model();

            shaderProgram.setMat4(0, model);
            shaderProgram.setMat4(1, projection * view * model);

            RenderCommand c(shaderProgram, mesh);
            c.textures = textures;
            c.properties.enableFaceCulling = true;
            ren.addCommand(c);

            firstCommand = false;
        }

        ren.renderFinish();

        target.setNumberOfColorAttachments(0);

        board.add<GBuffer>(gBuffer);
    }
}
