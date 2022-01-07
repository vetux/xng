/**
 *  XEngine - C++ game engine library
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

static const char *SHADER_VERT = R"###(#version 460 core

#define MAX_COLOR 15

struct Layer {
    int MSAA_SAMPLES;
    sampler2DMS color;
    sampler2DMS depth;
    int has_depth;
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

#define MAX_COLOR 15

struct Layer {
    int MSAA_SAMPLES;
    sampler2DMS color;
    sampler2DMS depth;
    int has_depth;
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

vec4 msaaAverage(sampler2DMS color, vec2 uv)
{
    vec4 ret;
    ivec2 size = textureSize(color);
    ivec2 pos = ivec2(size.x * uv.x, size.y * uv.y);
    for(int i = 0; i < globals.layer.MSAA_SAMPLES; i++)
    {
        ret += texelFetch(color, pos, i);
    }
    return ret / globals.layer.MSAA_SAMPLES;
}

void main()
{
    vec4 color = msaaAverage(globals.layer.color, fUv);
    float depth = 0;

    if (globals.layer.has_depth != 0)
    {
        depth = msaaAverage(globals.layer.depth, fUv).r;
    }

    fragColor = color;
    gl_FragDepth = depth;
}
)###";

namespace xengine {
    Compositor::Compositor(RenderDevice &device, std::vector<Layer> layers)
            : device(device), layers(std::move(layers)) {
        ShaderSource shaderVert(SHADER_VERT, "main", VERTEX, GLSL_460);
        ShaderSource shaderFrag(SHADER_FRAG, "main", FRAGMENT, GLSL_460);
        shader = std::unique_ptr<ShaderProgram>(device.getAllocator().createShaderProgram(shaderVert, shaderFrag));
    }

    std::vector<Compositor::Layer> &Compositor::getLayers() {
        return layers;
    }

    void Compositor::setClearColor(ColorRGB color) {
        clearColor = color;
    }

    void Compositor::presentLayers(RenderTarget &screen, GeometryBuffer &buffer) {
        presentLayers(screen, buffer, layers);
    }

    void Compositor::presentLayers(RenderTarget &screen,
                                   GeometryBuffer &buffer,
                                   const std::vector<Layer> &pLayers) {
        auto &ren = device.getRenderer();

        ren.renderClear(screen, {0, 0, 0, 255});

        if (layers.empty())
            return;

        shader->activate();

        for (auto &layer: layers) {
            drawLayer(screen, buffer, layer);
        }
    }

    void Compositor::drawLayer(RenderTarget &screen,
                               GeometryBuffer &buffer,
                               const Compositor::Layer &layer) {
        auto &ren = device.getRenderer();

        std::string prefix = "globals.layer";

        std::vector<std::reference_wrapper<TextureBuffer>> textures;

        textures.emplace_back(buffer.getBuffer(layer.color));
        assert(shader->setTexture(prefix + ".color", 0));

        assert(shader->setInt(prefix + ".has_depth", !layer.depth.empty()));
        if (!layer.depth.empty()) {
            textures.emplace_back(buffer.getBuffer(layer.depth));
            assert(shader->setTexture(prefix + ".depth", 1));
        }

        assert(shader->setInt("globals.layer.MSAA_SAMPLES", textures.at(0).get().getAttributes().samples));

        RenderCommand command(*shader, buffer.getScreenQuad());
        command.textures = textures;

        command.properties.enableBlending = true;
        command.properties.depthTestMode = layer.depthTestMode;
        command.properties.blendSourceMode = layer.colorBlendModeSource;
        command.properties.blendDestinationMode = layer.colorBlendModeDest;

        ren.renderBegin(screen, RenderOptions({}, screen.getSize(), false, {}, 0, false, false));
        ren.addCommand(command);
        ren.renderFinish();
    }

    void Compositor::setLayers(const std::vector<Layer> &l) {
        layers = l;
    }
}