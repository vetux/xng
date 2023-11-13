#version 460

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

layout(binding = 0, std140) uniform ShaderData
{
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
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.texAtlasIndex >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.uvOffset_uvScale.zw;
        uv = uv + vars.uvOffset_uvScale.xy;
        uv = uv * vars.atlasScale_texSize.xy;
        vec4 texColor;
        texColor = texture(atlasTextures[vars.texAtlasLevel],
                                        vec3(uv.x, uv.y, vars.texAtlasIndex));
        if (vars.colorFactor != 0.f) {
            color = vars.color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.color.rgb, vars.colorMixFactor);
            color.a = mix(texColor.a, vars.color.a, vars.alphaMixFactor);
        }
    } else {
        color = vars.color;
    }
}
