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

#include "render/graph/passes/skyboxpass.hpp"

SkyboxPass::SkyboxPass() {

}

void xng::SkyboxPass::setup(FrameGraphBuilder &builder, const GenericMapString &properties, GenericMapString &sharedData) {

}

void xng::SkyboxPass::execute(FrameGraphPassResources &resources) {

}

std::type_index xng::SkyboxPass::getTypeName() {
    return typeid(xng::SkyboxPass);
}

/*

#include "render/graph/passes/skyboxpass.hpp"

#include "render/shader/shaderinclude.hpp"

#include "render/graph/passes/compositepass.hpp"

static const char *SHADER_VERT = R"###(#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 instanceRow0;
layout (location = 6) in vec4 instanceRow1;
layout (location = 7) in vec4 instanceRow2;
layout (location = 8) in vec4 instanceRow3;

layout (location = 0) out vec3 worldPos;

uniform mat4 MANA_VIEW_TRANSLATION;
uniform mat4 MANA_M;
uniform mat4 MANA_V;
uniform mat4 MANA_P;

void main()
{
    mat4 t = MANA_P * MANA_V * MANA_VIEW_TRANSLATION;
    worldPos = position;
    gl_Position = t * vec4(position, 1);
}
)###";

static const char *SHADER_FRAG = R"###(#version 410 core

layout (location = 0) in vec3 worldPos;

layout (location = 0) out vec4 color;

uniform mat4 MANA_VIEW_TRANSLATION;
uniform mat4 MANA_M;
uniform mat4 MANA_V;
uniform mat4 MANA_P;

uniform samplerCube diffuse;

void main() {
    color = texture(diffuse, worldPos);
}
)###";

namespace xng {
    SkyboxPass::SkyboxPass(RenderDevice &device) {
        Shader shaderSrc;
        shaderSrc.vertexShader = ShaderSource(SHADER_VERT, "main", VERTEX, GLSL_410);
        shaderSrc.fragmentShader = ShaderSource(SHADER_FRAG, "main", FRAGMENT, GLSL_410);

        shaderSrc.vertexShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                          ShaderInclude::getShaderMacros(GLSL_410));
        shaderSrc.fragmentShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                            ShaderInclude::getShaderMacros(GLSL_410));

        shader = device.getAllocator().createShaderProgram(shaderSrc.vertexShader, shaderSrc.fragmentShader);

        TextureBuffer::Attributes attributes;
        attributes.size = Vec2i(1, 1);
        attributes.format = TextureBuffer::RGBA;
        attributes.textureType = TextureBuffer::TEXTURE_CUBE_MAP;
        attributes.generateMipmap = false;
        attributes.wrapping = TextureBuffer::REPEAT;

        defaultTexture = device.getAllocator().createTextureBuffer(attributes);

        skyboxCube = device.getAllocator().createMeshBuffer(Mesh::normalizedCube());
    }

    void SkyboxPass::setup(FrameGraphBuilder &builder) {
        scene = builder.getScene();

        auto format = builder.getRenderFormat();
        renderTarget = builder.createRenderTarget(format.first, format.second);

        outColor = builder.createTextureBuffer(TextureBuffer::Attributes{.size = format.first, });

        if (scene.skybox.texture) {
            skyboxTexture = builder.createTextureBuffer(scene.skybox.texture);
        } else {
            skyboxTexture = {};
        }

        outDepth = builder.createTextureBuffer(
                TextureBuffer::Attributes{.size = format.first, .format = TextureBuffer::DEPTH_STENCIL});
    }

    void SkyboxPass::execute(RenderPassResources &resources, Renderer &ren, FrameGraphBlackboard &board) {
        auto &target = resources.getRenderTarget(renderTarget);
        auto &color = resources.getTextureBuffer(outColor);
        auto &depth = resources.getTextureBuffer(outDepth);
        auto &gBuffer = board.get<GBuffer>();

        shader->activate();
        shader->setTexture("diffuse", 0);

        Mat4f model, view, projection, cameraTranslation;
        view = scene.camera.view();
        projection = scene.camera.projection();
        cameraTranslation = MatrixMath::translate(scene.camera.transform.getPosition());

        //Draw skybox
        target.setNumberOfColorAttachments(1);
        target.attachColor(0, color);
        target.attachDepthStencil(depth);

        ren.renderBegin(target, RenderOptions({}, target.getSize(), false));

        shader->setMat4("MANA_M", model);
        shader->setMat4("MANA_V", view);
        shader->setMat4("MANA_P", projection);
        shader->setMat4("MANA_MVP", projection * view * model);
        shader->setMat4("MANA_M_INVERT", MatrixMath::inverse(model));
        shader->setMat4("MANA_VIEW_TRANSLATION", cameraTranslation);

        RenderCommand skyboxCommand(*shader, *skyboxCube);

        if (skyboxTexture.assigned) {
            skyboxCommand.textures.emplace_back(resources.getTextureBuffer(skyboxTexture));
        } else {
            for (int i = TextureBuffer::CubeMapFace::POSITIVE_X; i <= TextureBuffer::CubeMapFace::NEGATIVE_Z; i++) {
                defaultTexture->upload(static_cast<TextureBuffer::CubeMapFace>(i),
                                       ImageRGBA(1, 1, {scene.skybox.color}));
            }
            skyboxCommand.textures.emplace_back(*defaultTexture);
        }

        skyboxCommand.properties.enableDepthTest = false;
        skyboxCommand.properties.enableFaceCulling = false;

        ren.addCommand(skyboxCommand);

        ren.renderFinish();

        target.detachColor(0);

        auto layers = board.get<std::vector<CompositePass::Layer>>();

        layers.emplace_back(CompositePass::Layer(&color));

        board.set(layers);
    }
}
 */