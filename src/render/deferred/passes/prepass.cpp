/**
 *  Mana - 3D Game Engine
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

#include <sstream>

#include "render/deferred/passes/prepass.hpp"
#include "render/deferred/deferredrenderer.hpp"
#include "platform/graphics/shadercompiler.hpp"
#include "render/shader/shaderinclude.hpp"
#include "asset/assetimporter.hpp"

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
    using namespace ShaderCompiler;

    static inline Vec4f scaleColor(const ColorRGBA &color) {
        return {static_cast<float>(color.r()) / 255,
                static_cast<float>(color.g()) / 255,
                static_cast<float>(color.b()) / 255,
                static_cast<float>(color.a()) / 255};
    }

    const char *PrePass::DEPTH = "depth";
    const char *PrePass::POSITION = "position";
    const char *PrePass::NORMAL = "normal";
    const char *PrePass::TANGENT = "tangent";
    const char *PrePass::TEXTURE_NORMAL = "texture_normal";
    const char *PrePass::DIFFUSE = "diffuse";
    const char *PrePass::AMBIENT = "ambient";
    const char *PrePass::SPECULAR = "specular";
    const char *PrePass::SHININESS_ID = "shininess_id";

    PrePass::PrePass(RenderDevice &device)
            : renderDevice(device) {
         vs = ShaderSource(SHADER_VERT_GEOMETRY, "main", VERTEX, GLSL_460);
         fs = ShaderSource(SHADER_FRAG_GEOMETRY, "main", FRAGMENT, GLSL_460);

        vs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(GLSL_460));
        fs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(GLSL_460));

        auto &allocator = device.getAllocator();
        shader = allocator.createShaderProgram(vs, fs);

        shader->activate();
        shader->setTexture(8, 0);
        shader->setTexture(9, 1);
        shader->setTexture(10, 2);
        shader->setTexture(11, 3);
        shader->setTexture(12, 4);
        shader->setTexture(13, 5);

        TextureBuffer::Attributes attributes;
        attributes.size = {1, 1};
        defaultTexture = allocator.createTextureBuffer(attributes);
        defaultTexture->upload(Image<ColorRGBA>(1, 1, {{0, 0, 0, 0}}));
    }

    PrePass::~PrePass() = default;

    void PrePass::prepareBuffer(GeometryBuffer &gBuffer) {
        gBuffer.addBuffer(DEPTH, TextureBuffer::ColorFormat::DEPTH_STENCIL);
        gBuffer.addBuffer(POSITION, TextureBuffer::ColorFormat::RGBA32F);
        gBuffer.addBuffer(NORMAL, TextureBuffer::ColorFormat::RGBA32F);
        gBuffer.addBuffer(TANGENT, TextureBuffer::ColorFormat::RGBA32F);
        gBuffer.addBuffer(TEXTURE_NORMAL, TextureBuffer::ColorFormat::RGBA32F);
        gBuffer.addBuffer(DIFFUSE, TextureBuffer::ColorFormat::RGBA);
        gBuffer.addBuffer(AMBIENT, TextureBuffer::ColorFormat::RGBA);
        gBuffer.addBuffer(SPECULAR, TextureBuffer::ColorFormat::RGBA);
        gBuffer.addBuffer(SHININESS_ID, TextureBuffer::ColorFormat::RGBA32F);
    }

    void PrePass::render(GeometryBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) {
        auto &ren = renderDevice.getRenderer();

        Mat4f model, view, projection;
        view = scene.camera.view();
        projection = scene.camera.projection();

        shader->activate();

        // Draw deferred geometry
        gBuffer.attachColor({
                                    POSITION,
                                    NORMAL,
                                    TANGENT,
                                    TEXTURE_NORMAL,
                                    DIFFUSE,
                                    AMBIENT,
                                    SPECULAR,
                                    SHININESS_ID
                            });
        gBuffer.attachDepthStencil(DEPTH);

        //Clear geometry buffer
        ren.renderBegin(gBuffer.getRenderTarget(), RenderOptions({}, gBuffer.getRenderTarget().getSize()));

        std::vector<std::reference_wrapper<TextureBuffer>> textures;
        textures.reserve(6);

        bool firstCommand = true;
        Material shaderMaterial;

        // Rasterize the geometry and store the geometry + shading data in the geometry buffer.
        for (auto &command: scene.deferred) {
            textures.clear();

            if (command.material.get().diffuseTexture.empty()) {
                if (firstCommand || shaderMaterial.diffuse != command.material.get().diffuse) {
                    shaderMaterial.diffuse = command.material.get().diffuse;
                    shader->setVec4(3, scaleColor(command.material.get().diffuse));
                }
                textures.emplace_back(*defaultTexture);
            } else {
                if (firstCommand || shaderMaterial.diffuse != ColorRGBA()) {
                    shaderMaterial.diffuse = ColorRGBA();
                    shader->setVec4(3, Vec4f());
                }
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().diffuseTexture));
            }

            if (command.material.get().ambientTexture.empty()) {
                if (firstCommand || shaderMaterial.ambient != command.material.get().ambient) {
                    shaderMaterial.ambient = command.material.get().ambient;
                    shader->setVec4(4, scaleColor(command.material.get().ambient));
                }
                textures.emplace_back(*defaultTexture);
            } else {
                if (firstCommand || shaderMaterial.ambient != ColorRGBA()) {
                    shaderMaterial.ambient = ColorRGBA();
                    shader->setVec4(4, Vec4f());
                }
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().ambientTexture));
            }

            if (command.material.get().specularTexture.empty()) {
                if (firstCommand || shaderMaterial.specular != command.material.get().specular) {
                    shaderMaterial.specular = command.material.get().specular;
                    shader->setVec4(5, scaleColor(command.material.get().specular));
                }
                textures.emplace_back(*defaultTexture);
            } else {
                if (firstCommand || shaderMaterial.specular != ColorRGBA()) {
                    shaderMaterial.specular = ColorRGBA();
                    shader->setVec4(5, Vec4f());
                }
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().specularTexture));
            }

            if (command.material.get().shininessTexture.empty()) {
                if (firstCommand || shaderMaterial.shininess != command.material.get().shininess) {
                    shaderMaterial.shininess = command.material.get().shininess;
                    shader->setFloat(6, command.material.get().shininess);
                }
                textures.emplace_back(*defaultTexture);
            } else {
                if (firstCommand || shaderMaterial.shininess != 0) {
                    shaderMaterial.shininess = 0;
                    shader->setFloat(6, 0);
                }
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().shininessTexture));
            }

            if (command.material.get().emissiveTexture.empty()) {
                if (firstCommand || shaderMaterial.emissive != command.material.get().emissive) {
                    shaderMaterial.emissive = command.material.get().emissive;
                    shader->setVec4(7, scaleColor(command.material.get().emissive));
                }
                textures.emplace_back(*defaultTexture);
            } else {
                if (firstCommand || shaderMaterial.emissive != ColorRGBA()) {
                    shaderMaterial.emissive = ColorRGBA();
                    shader->setVec4(7, Vec4f());
                }
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().emissiveTexture));
            }

            if (firstCommand
                || !(shaderMaterial.normalTexture == command.material.get().normalTexture)) {
                shaderMaterial.normalTexture = command.material.get().normalTexture;
                shader->setInt(2, !command.material.get().normalTexture.empty());
            }

            if (!command.material.get().normalTexture.empty()) {
                textures.emplace_back(assetRenderManager.get<TextureBuffer>(command.material.get().normalTexture));
            }

            model = command.transform.model();

            shader->setMat4(0, model);
            shader->setMat4(1, projection * view * model);

            RenderCommand c(*shader, command.mesh.getRenderObject<MeshBuffer>());
            c.textures = textures;
            c.properties.enableFaceCulling = true;
            ren.addCommand(c);

            firstCommand = false;
        }

        ren.renderFinish();
    }
}