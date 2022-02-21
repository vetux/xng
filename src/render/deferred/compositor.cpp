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

#include "render/deferred/compositor.hpp"

#include "render/shader/shaderinclude.hpp"

static const char *SHADER_VERT = R"###(#version 460 core

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

static const char *SHADER_FRAG = R"###(#version 460 core

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
    Compositor::Compositor(RenderDevice &device)
            : device(device), screenQuad(device.getAllocator().createMeshBuffer(Mesh::normalizedQuad())) {
        ShaderSource shaderVert(SHADER_VERT, "main", VERTEX, GLSL_460);
        ShaderSource shaderFrag(SHADER_FRAG, "main", FRAGMENT, GLSL_460);
        shaderFrag.preprocess(ShaderInclude::getShaderIncludeCallback(), ShaderInclude::getShaderMacros(GLSL_460));
        shaderFrag.crossCompile(GLSL_460);
        shader = std::unique_ptr<ShaderProgram>(device.getAllocator().createShaderProgram(shaderVert.compile(),
                                                                                          shaderFrag.compile()));
    }

    void Compositor::setClearColor(ColorRGBA color) {
        clearColor = color;
    }

    void Compositor::present(RenderTarget &screen,
                             std::vector<std::unique_ptr<RenderPass>> &passes) {
        auto &ren = device.getRenderer();

        ren.renderClear(screen, clearColor, 1);

        auto layers = getLayers(passes);

        if (layers.empty())
            return;

        shader->activate();

        for (auto &node: layers) {
            drawLayer(screen, node);
        }
    }

    std::vector<Compositor::Layer> Compositor::getLayers(std::vector<std::unique_ptr<RenderPass>> &passes) {
        std::vector<Layer> ret;
        for (auto &pass: passes) {
            Layer l;
            l.color = pass->getColorBuffer();
            l.depth = pass->getDepthBuffer();
            ret.emplace_back(l);
        }
        return ret;
    }

    void Compositor::drawLayer(RenderTarget &screen, const Compositor::Layer &layer) {
        auto &ren = device.getRenderer();

        std::string prefix = "globals.layer";

        std::vector<std::reference_wrapper<TextureBuffer>> textures;

        if (layer.color != nullptr) {
            textures.emplace_back(*layer.color);
            assert(shader->setTexture(prefix + ".color", 0));
        }

        assert(shader->setInt(prefix + ".has_depth", layer.depth != nullptr));
        if (layer.depth != nullptr) {
            textures.emplace_back(*layer.depth);
            assert(shader->setTexture(prefix + ".depth", textures.size() - 1));
        }

        assert(shader->setBool("globals.layer.filterBicubic",
                               textures.at(0).get().getAttributes().size != screen.getSize()));

        RenderCommand command(*shader, *screenQuad);
        command.textures = textures;
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = layer.enableBlending;
        command.properties.depthTestMode = layer.depthTestMode;
        command.properties.blendSourceMode = layer.colorBlendModeSource;
        command.properties.blendDestinationMode = layer.colorBlendModeDest;

        ren.renderBegin(screen, RenderOptions({}, screen.getSize(), false, false, 1, {}, 0, false, false));
        ren.addCommand(command);
        ren.renderFinish();
    }
}