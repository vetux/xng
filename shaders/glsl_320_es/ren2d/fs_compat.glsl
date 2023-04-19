#version 320 es

//TODO: Add linear filtering support for GLSL ES render path

layout (location = 0) in highp vec4 fPosition;
layout (location = 1) in highp vec2 fUv;

layout (location = 0) out highp vec4 color;

layout(binding = 0, std140) uniform ShaderData
{
    highp vec4 color;
    highp vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex_texFilter;
    highp mat4 mvp;
    highp vec4 uvOffset_uvScale;
    highp vec4 atlasScale_texSize;
} vars;

layout(binding = 1) uniform highp sampler2DArray atlasTextures[12];

void main() {
    if (vars.texAtlasLevel_texAtlasIndex_texFilter.y >= 0) {
        highp vec2 uv = fUv;
        uv = uv * vars.uvOffset_uvScale.zw;
        uv = uv + vars.uvOffset_uvScale.xy;
        uv = uv * vars.atlasScale_texSize.xy;
        highp vec4 texColor;
        texColor = texture(atlasTextures[vars.texAtlasLevel_texAtlasIndex_texFilter.x],
                                        vec3(uv.x, uv.y, vars.texAtlasLevel_texAtlasIndex_texFilter.y));
        if (vars.colorMixFactor_alphaMixFactor_colorFactor.z != 0.f) {
            color = vars.color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.color.rgb, vars.colorMixFactor_alphaMixFactor_colorFactor.x);
            color.a = mix(texColor.a, vars.color.a, vars.colorMixFactor_alphaMixFactor_colorFactor.y);
        }
    } else {
        color = vars.color;
    }
}
