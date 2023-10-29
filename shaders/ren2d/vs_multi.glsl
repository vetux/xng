#version 460

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;
layout (location = 2) flat out uint drawID;

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

void main() {
    fPosition = (vars.passes[gl_DrawID].mvp) * vec4(position, 0, 1);
    fUv = uv;
    drawID = gl_DrawID;
    gl_Position = fPosition;
}
