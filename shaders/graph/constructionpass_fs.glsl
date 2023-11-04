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
layout(location = 5) out vec4 oAmbient;
layout(location = 6) out vec4 oSpecular;
layout(location = 7) out ivec4 oModelObjectShadows;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 shadeModel_objectID_boneOffset_shadows;
    vec4 metallic_roughness_ambientOcclusion_shininess;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;

    vec4 normalIntensity;

    ShaderAtlasTexture normal;

    ShaderAtlasTexture metallic;
    ShaderAtlasTexture roughness;
    ShaderAtlasTexture ambientOcclusion;

    ShaderAtlasTexture diffuse;
    ShaderAtlasTexture ambient;
    ShaderAtlasTexture specular;
    ShaderAtlasTexture shininess;
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

    if (data.diffuse.level_index_filtering_assigned.w == 0) {
        oAlbedo = data.diffuseColor;
    } else {
        oAlbedo = textureAtlas(data.diffuse, fUv);
    }

    if (data.shadeModel_objectID_boneOffset_shadows.x == 0)
    {
        oRoughnessMetallicAO.r = textureAtlas(data.roughness, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.y;
        oRoughnessMetallicAO.g = textureAtlas(data.metallic, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.x;
        oRoughnessMetallicAO.b = textureAtlas(data.ambientOcclusion, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.z;
        oRoughnessMetallicAO.a = 1;
    }
    else
    {
        if (data.ambient.level_index_filtering_assigned.w == 0){
            oAmbient =  data.ambientColor;
        } else {
            oAmbient = textureAtlas(data.ambient, fUv) ;
        }
        if (data.specular.level_index_filtering_assigned.w == 0){
            oSpecular = data.specularColor;
        } else {
            oSpecular = textureAtlas(data.specular, fUv);
        }
        if (data.shininess.level_index_filtering_assigned.w == 0){
            oRoughnessMetallicAO.r = data.metallic_roughness_ambientOcclusion_shininess.w;
        } else {
            oRoughnessMetallicAO.r = textureAtlas(data.shininess, fUv).r;
        }
        oRoughnessMetallicAO.g = 0;
        oRoughnessMetallicAO.b = 0;
        oRoughnessMetallicAO.a = 1;
    }

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

    oModelObjectShadows.r = data.shadeModel_objectID_boneOffset_shadows.x;
    oModelObjectShadows.g = data.shadeModel_objectID_boneOffset_shadows.y;
    oModelObjectShadows.b = data.shadeModel_objectID_boneOffset_shadows.w;
    oModelObjectShadows.a = 1;
}
