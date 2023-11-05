#version 460

#include "texfilter.glsl"

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;
layout(location = 5) in vec3 fT;
layout(location = 6) in vec3 fB;
layout(location = 7) in vec3 fN;
layout(location = 8) flat in uint drawID;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oTangent;
layout(location = 3) out vec4 oRoughnessMetallicAO;
layout(location = 4) out vec4 oAlbedo;
layout(location = 5) out ivec4 oObjectShadows;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 objectID_boneOffset_shadows;

    vec4 metallic_roughness_ambientOcclusion;
    vec4 albedoColor;

    vec4 normalIntensity;

    ShaderAtlasTexture normal;

    ShaderAtlasTexture metallic;
    ShaderAtlasTexture roughness;
    ShaderAtlasTexture ambientOcclusion;
    ShaderAtlasTexture albedo;
};

layout(binding = 0, std140) buffer ShaderUniformBuffer
{
    ShaderDrawData data[];
} globs;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

vec4 textureAtlas(ShaderAtlasTexture tex, vec2 inUv)
{
    if (tex.level_index_filtering_assigned.w == 0)
    {
        return vec4(0, 0, 0, 0);
    } else {
        vec2 uv = inUv * tex.atlasScale_texSize.xy;
        if (tex.level_index_filtering_assigned.z == 1)
        {
            return textureBicubic(atlasTextures[tex.level_index_filtering_assigned.x],
            vec3(uv.x, uv.y, tex.level_index_filtering_assigned.y),
            tex.atlasScale_texSize.zw);
        }
        else
        {
            return texture(atlasTextures[tex.level_index_filtering_assigned.x], vec3(uv.x, uv.y, tex.level_index_filtering_assigned.y));
        }
    }
}

void main() {
    ShaderDrawData data = globs.data[drawID];

    oPosition = vec4(fPos, 1);

    if (data.albedo.level_index_filtering_assigned.w == 0) {
        oAlbedo = data.albedoColor;
    } else {
        oAlbedo = textureAtlas(data.albedo, fUv);
    }

    oRoughnessMetallicAO.r = textureAtlas(data.roughness, fUv).r + data.metallic_roughness_ambientOcclusion.y;
    oRoughnessMetallicAO.g = textureAtlas(data.metallic, fUv).r + data.metallic_roughness_ambientOcclusion.x;
    oRoughnessMetallicAO.b = textureAtlas(data.ambientOcclusion, fUv).r + data.metallic_roughness_ambientOcclusion.z;
    oRoughnessMetallicAO.a = 1;

    mat3 normalMatrix = mat3(transpose(inverse(data.model)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (data.normal.level_index_filtering_assigned.w != 0)
    {
        mat3x3 tbn = mat3(fT, fB, fN);
        vec3 texNormal = textureAtlas(data.normal, fUv).xyz * vec3(data.normalIntensity.x, data.normalIntensity.x, 1);
        texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
        oNormal = vec4(normalize(texNormal), 1);
    }

    oObjectShadows.r = data.objectID_boneOffset_shadows.x;
    oObjectShadows.g = data.objectID_boneOffset_shadows.z;
    oObjectShadows.b = 0;
    oObjectShadows.a = 1;
}
