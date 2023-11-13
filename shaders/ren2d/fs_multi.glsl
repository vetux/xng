#version 460

#include "texfilter.glsl"

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;
layout (location = 2) flat in uint drawID;

layout (location = 0) out vec4 color;

struct PassData {
    vec4 color;
    float colorMixFactor;
    float alphaMixFactor;
    float colorFactor;
    int texAtlasLevel;
    int texAtlasIndex;
    int texFilter;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale_texSize;
    float _padding;
};

layout(binding = 0, std140) buffer ShaderData
{
    PassData passes[];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.passes[drawID].texAtlasIndex >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.passes[drawID].uvOffset_uvScale.zw;
        uv = uv + vars.passes[drawID].uvOffset_uvScale.xy;
        uv = uv * vars.passes[drawID].atlasScale_texSize.xy;
        vec4 texColor;
        if (vars.passes[drawID].texFilter == 1)
        {
            texColor = textureBicubic(atlasTextures[vars.passes[drawID].texAtlasLevel],
            vec3(uv.x, uv.y, vars.passes[drawID].texAtlasIndex),
            vars.passes[drawID].atlasScale_texSize.zw);
        }
        else
        {
            texColor = texture(atlasTextures[vars.passes[drawID].texAtlasLevel],
            vec3(uv.x, uv.y, vars.passes[drawID].texAtlasIndex));
        }
        if (vars.passes[drawID].colorFactor != 0) {
            color = vars.passes[drawID].color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.passes[drawID].color.rgb, vars.passes[drawID].colorMixFactor);
            color.a = mix(texColor.a, vars.passes[drawID].color.a, vars.passes[drawID].alphaMixFactor);
        }
    } else {
        color = vars.passes[drawID].color;
    }
}
