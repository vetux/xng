#version 320 es

layout (location = 0) in highp vec2 position;
layout (location = 1) in highp vec2 uv;

layout (location = 0) out highp vec4 fPosition;
layout (location = 1) out highp vec2 fUv;

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
    fPosition = (vars.mvp) * vec4(position, 0, 1);
    fUv = uv;
    gl_Position = fPosition;
}
