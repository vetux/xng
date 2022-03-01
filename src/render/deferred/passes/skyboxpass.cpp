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

#include "render/deferred/passes/skyboxpass.hpp"
#include "render/deferred/deferredpipeline.hpp"
#include "render/shader/shaderinclude.hpp"

#include "asset/assetimporter.hpp"

#include <sstream>

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

namespace xengine {
    SkyboxPass::SkyboxPass(RenderDevice &device)
            : device(device) {
        vert = ShaderSource(SHADER_VERT, "main", VERTEX, GLSL_410);
        frag = ShaderSource(SHADER_FRAG, "main", FRAGMENT, GLSL_410);

        vert.preprocess(ShaderInclude::getShaderIncludeCallback(),
                        ShaderInclude::getShaderMacros(GLSL_410));
        frag.preprocess(ShaderInclude::getShaderIncludeCallback(),
                        ShaderInclude::getShaderMacros(GLSL_410));

        auto &allocator = device.getAllocator();

        shader = allocator.createShaderProgram(vert, frag);

        TextureBuffer::Attributes attributes;
        attributes.size = Vec2i(1, 1);
        attributes.format = TextureBuffer::RGBA;
        attributes.textureType = TextureBuffer::TEXTURE_CUBE_MAP;
        attributes.generateMipmap = false;
        attributes.wrapping = TextureBuffer::REPEAT;

        defaultTexture = allocator.createTextureBuffer(attributes);

        meshBuffer = allocator.createMeshBuffer(Mesh::normalizedCube());

        resizeTextureBuffers({1, 1}, device.getAllocator(), false);
    }

    void SkyboxPass::render(GBuffer &gBuffer, Scene &scene) {
        auto &ren = device.getRenderer();

        shader->activate();
        shader->setTexture("diffuse", 0);

        Mat4f model, view, projection, cameraTranslation;
        view = scene.camera.view();
        projection = scene.camera.projection();
        cameraTranslation = MatrixMath::translate(scene.camera.transform.getPosition());

        //Draw skybox
        auto &target = gBuffer.getPassTarget();

        if (colorBuffer->getAttributes().size != gBuffer.getSize()) {
            resizeTextureBuffers(gBuffer.getSize(), device.getAllocator(), false);
        }

        target.setNumberOfColorAttachments(1);
        target.attachColor(0, *colorBuffer);

        ren.renderBegin(target, RenderOptions({}, target.getSize(), false));

        shader->setMat4("MANA_M", model);
        shader->setMat4("MANA_V", view);
        shader->setMat4("MANA_P", projection);
        shader->setMat4("MANA_MVP", projection * view * model);
        shader->setMat4("MANA_M_INVERT", MatrixMath::inverse(model));
        shader->setMat4("MANA_VIEW_TRANSLATION", cameraTranslation);

        RenderCommand skyboxCommand(*shader, *meshBuffer);

        if (scene.skybox.texture) {
            skyboxCommand.textures.emplace_back(*scene.skybox.texture);
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
    }
}