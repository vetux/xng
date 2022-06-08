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

#warning NOT IMPLEMENTED
/*
#include "render/graph/passes/compositepass.hpp"

#include "render/shaderinclude.hpp"

static const char *SHADER_VERT = R"###(#version 410 core

struct Layer {
    sampler2D color;
    sampler2D depth;
    int has_depth;
    bool filterBicubic;
};

struct Globals {
    Layer layer;
};

uniform Globals globals;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 instanceRow0;
layout (location = 6) in vec4 instanceRow1;
layout (location = 7) in vec4 instanceRow2;
layout (location = 8) in vec4 instanceRow3;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

void main()
{
    fPos = vec4(position, 1);
    fUv = uv;
    gl_Position = fPos;
}

)###";

static const char *SHADER_FRAG = R"###(#version 410 core

#include "texfilter.glsl"

struct Layer {
    sampler2D color;
    sampler2D depth;
    int has_depth;
    bool filterBicubic;
};

struct Globals {
    Layer layer;
};

uniform Globals globals;

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 fragColor;

vec4 blend(vec4 colorA, vec4 colorB)
{
    return vec4((colorB.rgb * colorB.a + colorA.rgb * (1.0 - colorB.a)).rgb, 1);
}

void main()
{
    // Apply bicubic filtering which smooths out the image if the texture size is smaller than the viewport size.
    if (globals.layer.filterBicubic)
    {
        fragColor = textureBicubic(globals.layer.color, fUv);
    }
    else
    {
        fragColor = texture(globals.layer.color, fUv);
    }

    gl_FragDepth = 1;
    if (globals.layer.has_depth != 0)
    {
        gl_FragDepth = texture(globals.layer.depth, fUv).r;
    }
}
)###";

namespace xengine {
    CompositePass::CompositePass(RenderDevice &device, ColorRGBA clearColor)
            : clearColor(clearColor) {
        Shader shaderSrc;
        shaderSrc.vertexShader = ShaderSource(SHADER_VERT, "main", VERTEX, GLSL_410);
        shaderSrc.fragmentShader = ShaderSource(SHADER_FRAG, "main", FRAGMENT, GLSL_410);
        shaderSrc.fragmentShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                            ShaderInclude::getShaderMacros(GLSL_410));
        shaderSrc.fragmentShader.crossCompile(GLSL_410);

        shader = device.createShaderProgram(shaderSrc.vertexShader, shaderSrc.fragmentShader);
        quadMesh = device.createMeshBuffer(Mesh::normalizedQuad());
    }

    void CompositePass::setup(FrameGraphBuilder &builder) {
        backBuffer = builder.getBackBuffer();
    }

    void CompositePass::execute(RenderPassResources &resources, Renderer &ren, FrameGraphBlackboard &board) {
        auto &target = resources.getRenderTarget(backBuffer);
        shader->activate();
        ren.renderClear(target, clearColor, 1);
        auto &layers = board.get<std::vector<Layer>>();
        for (auto &l: layers) {
            drawLayer(l, ren, target, *shader, *quadMesh);
        }
    }

    //Does not seem to draw the contents of the texture.
    void CompositePass::drawLayer(Layer layer,
                                  Renderer &ren,
                                  RenderTarget &target,
                                  ShaderProgram &shaderProgram,
                                  MeshBuffer &screenQuad) {
        std::string prefix = "globals.layer";

        std::vector<std::reference_wrapper<TextureBuffer>> textures;

        if (layer.color != nullptr) {
            textures.emplace_back(*layer.color);
            assert(shaderProgram.setTexture(prefix + ".color", 0));
        }

        assert(shaderProgram.setInt(prefix + ".has_depth", layer.depth != nullptr));
        if (layer.depth != nullptr) {
            textures.emplace_back(*layer.depth);
            assert(shaderProgram.setTexture(prefix + ".depth", textures.size() - 1));
        }

        assert(shaderProgram.setBool("globals.layer.filterBicubic",
                                     textures.at(0).get().getAttributes().size != target.getSize()));

        RenderCommand command(shaderProgram, screenQuad);
        command.textures = textures;
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = layer.enableBlending;
        command.properties.depthTestMode = layer.depthTestMode;
        command.properties.blendSourceMode = layer.colorBlendModeSource;
        command.properties.blendDestinationMode = layer.colorBlendModeDest;

        ren.renderBegin(target, RenderOptions({}, target.getSize(), true, false, 1, {}, 0, false, false));
        ren.addCommand(command);
        ren.renderFinish();
    }
}*/