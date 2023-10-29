#version 460

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

layout(binding = 0, std140) uniform ShaderData
{
    vec4 color;
    vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex_texFilter;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale_texSize;
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.texAtlasLevel_texAtlasIndex_texFilter.y >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.uvOffset_uvScale.zw;
        uv = uv + vars.uvOffset_uvScale.xy;
        uv = uv * vars.atlasScale_texSize.xy;
        vec4 texColor;
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
