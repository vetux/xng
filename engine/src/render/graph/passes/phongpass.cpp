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

#include "render/graph/passes/phongpass.hpp"

namespace xng {
    PhongPass::PhongPass() {

    }

    void PhongPass::setup(FrameGraphBuilder &builder,
                          const GenericMapString &properties,
                          GenericMapString &sharedData) {

    }

    void PhongPass::execute(FrameGraphPassResources &resources) {

    }

    std::type_index PhongPass::getTypeName() {
        return typeid(PhongPass);
    }
}

/*
#include "render/graph/passes/phongpass.hpp"

#include "render/shader/shaderinclude.hpp"

#include "math/rotation.hpp"

#include "render/graph/passes/compositepass.hpp"

// TODO: Fix phong pass outputting artifacts when resizing the window with the mouse or changing gbuffer resolution.

const char *SHADER_VERT_LIGHTING = R"###(#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 instanceRow0;
layout (location = 6) in vec4 instanceRow1;
layout (location = 7) in vec4 instanceRow2;
layout (location = 8) in vec4 instanceRow3;

layout (location = 0) out vec3 fPos;
layout (location = 1) out vec3 fNorm;
layout (location = 2) out vec2 fUv;
layout (location = 3) out vec4 vPos;

void main()
{
    fPos = position;
    fNorm = normal;
    fUv = uv;
    vPos = vec4(position, 1);
    gl_Position = vPos;
}
)###";

const char *SHADER_FRAG_LIGHTING = R"###(#version 410 core

#include "phong.glsl"

layout (location = 0) in vec3 fPos;
layout (location = 1) in vec3 fNorm;
layout (location = 2) in vec2 fUv;

layout (location = 0) out vec4 color;

uniform sampler2DMS position;
uniform sampler2DMS normal;
uniform sampler2DMS tangent;
uniform sampler2DMS texNormal;
uniform sampler2DMS diffuse;
uniform sampler2DMS ambient;
uniform sampler2DMS specular;
uniform sampler2DMS shininess;
uniform sampler2DMS depth;

uniform vec3 VIEW_POS;
uniform int NUM_SAMPLES;

LightComponents getSampleLightComponents(ivec2 coord, int sampleIndex)
{
    vec3 fragPosition = texelFetch(position, coord, sampleIndex).xyz;
    vec3 fragNormal = texelFetch(normal, coord, sampleIndex).xyz;
    vec3 fragTangent = texelFetch(tangent, coord, sampleIndex).xyz;
    vec4 fragDiffuse = texelFetch(diffuse, coord, sampleIndex);
    vec4 fragSpecular = texelFetch(specular, coord, sampleIndex);
    float fragShininess = texelFetch(shininess, coord, sampleIndex).x;
    vec3 fragTexNormal = texelFetch(texNormal, coord, sampleIndex).xyz;

    if (length(fragTexNormal) > 0)
    {
        // Calculate lighting in tangent space, does not output correct value
        mat3 TBN = transpose(mat3(cross(fragNormal, normalize(fragTangent - dot(fragTangent, fragNormal) * fragNormal)), fragTangent, fragNormal));

        return mana_calculate_light(TBN * fragPosition,
                                    fragTexNormal,
                                    fragDiffuse,
                                    fragSpecular,
                                    fragShininess,
                                    TBN * VIEW_POS,
                                    TBN);
    }
    else
    {
        // Calculate lighting in world space
        return mana_calculate_light(fragPosition,
                                fragNormal,
                                fragDiffuse,
                                fragSpecular,
                                fragShininess,
                                VIEW_POS,
                                mat3(1));
    }
}

void main() {
    ivec2 size = textureSize(position);
    int samples = NUM_SAMPLES;
    vec2 coordFloat = fUv * vec2(size.x, size.y);
    ivec2 coord = ivec2(coordFloat.x, coordFloat.y);

    float fDepth = texelFetch(depth, coord, gl_SampleID).x;

    gl_FragDepth = fDepth;

    if (fDepth < 1) {
        LightComponents comp = getSampleLightComponents(coord, gl_SampleID);
        vec3 comb = comp.ambient + comp.diffuse + comp.specular;
        color = vec4(comb.r, comb.g, comb.b, 1);
    } else {
        color = vec4(0, 0, 0, 0); //Even though the texture should be cleared with an alpha value of 0 it apparently isnt.
    }
}
)###";

//TODO: Fix phong pass rendering black texture depending on camera orientation
namespace xng {
    using namespace ShaderCompiler;

    const int MAX_LIGHTS = 1000;

    PhongPass::PhongPass(RenderDevice &device) {
        Shader shaderSrc;
        shaderSrc.vertexShader = ShaderSource(SHADER_VERT_LIGHTING,
                                              "main",
                                              VERTEX,
                                              GLSL_410);
        shaderSrc.fragmentShader = ShaderSource(SHADER_FRAG_LIGHTING,
                                                "main",
                                                FRAGMENT,
                                                GLSL_410);

        shaderSrc.vertexShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                          ShaderInclude::getShaderMacros(GLSL_410));
        shaderSrc.fragmentShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                            ShaderInclude::getShaderMacros(GLSL_410));

        shader = device.getAllocator().createShaderProgram(shaderSrc.vertexShader, shaderSrc.fragmentShader);
        quadMesh = device.getAllocator().createMeshBuffer(Mesh::normalizedQuad());
    }

    void PhongPass::setup(FrameGraphBuilder &builder) {
        scene = builder.getScene();

        auto format = builder.getRenderFormat();

        renderTarget = builder.createRenderTarget(format.first, 1);
        multiSampleRenderTarget = builder.createRenderTarget(format.first, format.second);

        colorMultisample = builder.createTextureBuffer(TextureBuffer::Attributes{.size = format.first,
                .samples = format.second,
                .textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE});
        depthMultisample = builder.createTextureBuffer(TextureBuffer::Attributes{.size = format.first,
                .samples = format.second,
                .textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE,
                .format = TextureBuffer::DEPTH_STENCIL});

        outColor = builder.createTextureBuffer(TextureBuffer::Attributes{.size = format.first});
        outDepth = builder.createTextureBuffer(TextureBuffer::Attributes{
                .size = format.first,
                .format = TextureBuffer::DEPTH_STENCIL});
    }

    void PhongPass::execute(RenderPassResources &resources, Renderer &ren, FrameGraphBlackboard &board) {
        auto &gBuffer = board.get<GBuffer>();
        auto &target = resources.getRenderTarget(renderTarget);
        auto &multiSampleTarget = resources.getRenderTarget(multiSampleRenderTarget);
        auto &color = resources.getTextureBuffer(outColor);
        auto &depth = resources.getTextureBuffer(outDepth);
        auto &colorMs = resources.getTextureBuffer(colorMultisample);
        auto &depthMs = resources.getTextureBuffer(depthMultisample);

        int dirCount = 0;
        int pointCount = 0;
        int spotCount = 0;

        shader->activate();

        for (auto &light: scene.lights) {
            if (dirCount + pointCount + spotCount > MAX_LIGHTS)
                break;

            std::string name;
            switch (light.type) {
                case LIGHT_DIRECTIONAL:
                    name = "DIRECTIONAL_LIGHTS[" + std::to_string(dirCount++) + "].";
                    shader->setVec3(name + "direction", light.direction);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
                case LIGHT_POINT:
                    name = "POINT_LIGHTS[" + std::to_string(pointCount++) + "].";
                    shader->setVec3(name + "position", light.transform.getPosition());
                    shader->setFloat(name + "constantValue", light.constant);
                    shader->setFloat(name + "linearValue", light.linear);
                    shader->setFloat(name + "quadraticValue", light.quadratic);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
                case LIGHT_SPOT:
                    name = "SPOT_LIGHTS[" + std::to_string(spotCount++) + "].";
                    shader->setVec3(name + "position", light.transform.getPosition());
                    shader->setVec3(name + "direction", light.direction);
                    shader->setFloat(name + "cutOff", cosf(degreesToRadians(light.cutOff)));
                    shader->setFloat(name + "outerCutOff", cosf(degreesToRadians(light.outerCutOff)));
                    shader->setFloat(name + "constantValue", light.constant);
                    shader->setFloat(name + "linearValue", light.linear);
                    shader->setFloat(name + "quadraticValue", light.quadratic);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
            }
        }

        shader->setInt("DIRECTIONAL_LIGHTS_COUNT", dirCount);
        shader->setInt("POINT_LIGHTS_COUNT", pointCount);
        shader->setInt("SPOT_LIGHTS_COUNT", spotCount);

        shader->setTexture("position", 0);
        shader->setTexture("normal", 1);
        shader->setTexture("tangent", 2);
        shader->setTexture("texNormal", 3);
        shader->setTexture("diffuse", 4);
        shader->setTexture("ambient", 5);
        shader->setTexture("specular", 6);
        shader->setTexture("shininess", 7);
        shader->setTexture("depth", 8);

        shader->setVec3("VIEW_POS", scene.camera.transform.getPosition());
        shader->setInt("NUM_SAMPLES", gBuffer.getSamples());

        RenderCommand command(*shader, *quadMesh);

        command.textures.emplace_back(gBuffer.getTexture(GBuffer::POSITION));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::NORMAL));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::TANGENT));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::TEXTURE_NORMAL));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::DIFFUSE));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::AMBIENT));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::SPECULAR));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::ID_SHININESS));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::DEPTH));

        command.properties.enableDepthTest = false;
        command.properties.enableStencilTest = false;
        command.properties.enableFaceCulling = false;
        command.properties.enableBlending = false;

        multiSampleTarget.setNumberOfColorAttachments(1);
        multiSampleTarget.attachColor(0, colorMs);
        multiSampleTarget.attachDepthStencil(depthMs);

        ren.renderBegin(multiSampleTarget, RenderOptions({}, multiSampleTarget.getSize(), true));
        ren.addCommand(command);
        ren.renderFinish();

        target.setNumberOfColorAttachments(1);
        target.attachColor(0, color);
        target.attachDepthStencil(depth);

        ren.renderClear(target, ColorRGBA(0), 1);

        target.blitColor(multiSampleTarget,
                         {},
                         {},
                         multiSampleTarget.getSize(),
                         target.getSize(),
                         TextureBuffer::LINEAR,
                         0,
                         0);
        target.blitDepth(multiSampleTarget,
                         {},
                         {},
                         multiSampleTarget.getSize(),
                         target.getSize());

        target.detachColor(0);
        target.detachDepthStencil();

        multiSampleTarget.detachColor(0);
        multiSampleTarget.detachDepthStencil();

        auto layers = board.get<std::vector<CompositePass::Layer>>();

        layers.emplace_back(CompositePass::Layer(&color, &depth));

        board.set(layers);
    }
}*/