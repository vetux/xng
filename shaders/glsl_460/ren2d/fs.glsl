#version 460

#include "texfilter.glsl"

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;
layout (location = 2) flat in uint drawID;

layout (location = 0) out vec4 color;

struct PassData {
    vec4 color;
    vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex_texFilter;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale_texSize;
};

layout(binding = 0, std140) buffer ShaderData
{
    PassData passes[];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

// Windows findings:
// drawID is always zero??
// Accesses into the vars.passes array return (correct) different values even though drawID is always zero.
// Accesses into the atlasTextures arrays returns black color.
// In Conclusion it appears that the windows opengl driver (AMD) appears to have a very butched multidraw implementation
// and no real support for 2d array textures (SPIRV support also does not exist but theres no glErrors).

void main() {
    if (vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.y >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.passes[drawID].uvOffset_uvScale.zw;
        uv = uv + vars.passes[drawID].uvOffset_uvScale.xy;
        uv = uv * vars.passes[drawID].atlasScale_texSize.xy;
        vec4 texColor;
        if (vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.z == 1)
        {
            texColor = textureBicubic(atlasTextures[vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.x],
                            vec3(uv.x, uv.y, vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.y),
                            vars.passes[drawID].atlasScale_texSize.zw);
        }
        else
        {
            texColor = texture(atlasTextures[vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.x],
                            vec3(uv.x, uv.y, vars.passes[drawID].texAtlasLevel_texAtlasIndex_texFilter.y));
        }
        if (vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.z != 0) {
            color = vars.passes[drawID].color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.passes[drawID].color.rgb, vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.x);
            color.a = mix(texColor.a, vars.passes[drawID].color.a, vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.y);
        }
    } else {
        color = vars.passes[drawID].color;
    }
}
