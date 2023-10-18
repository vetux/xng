#version 460

#include "texfilter.glsl"
#include "phong.glsl"

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;
layout(location = 5) in vec3 fT;
layout(location = 6) in vec3 fB;
layout(location = 7) in vec3 fN;
layout(location = 8) flat in uint drawID;

//layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

layout(location = 0) out vec4 oColor;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 shadeModel_objectID;
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
    vec4 viewPosition;
    vec4 viewportSize;
    ShaderDrawData data[];
} globs;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];
layout(binding = 13) uniform sampler2D deferredDepth;

layout(binding = 14, std140) buffer PointLightsData
{
    PointLight lights[];
} pLights;

layout(binding = 15, std140) buffer SpotLightsData
{
    SpotLight lights[];
} sLights;

layout(binding = 16, std140) buffer DirectionalLightsData
{
    DirectionalLight lights[];
} dLights;

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
    float defDepth = texture(deferredDepth, gl_FragCoord.xy / globs.viewportSize.xy).r;
    if (gl_FragCoord.z > defDepth)
    {
        oColor = vec4(0, 0, 0, 0);
        return;
    }

    ShaderDrawData data = globs.data[drawID];

    mat3 normalMatrix = transpose(inverse(mat3(data.model)));
    vec3 normal = normalize(normalMatrix * fNorm);
    vec3 tangent = normalize(normalMatrix * fTan);

    if (data.normal.level_index_filtering_assigned.w != 0)
    {
        mat3x3 tbn = mat3(fT, fB, fN);
        vec3 texNormal = textureAtlas(data.normal, fUv).xyz * vec3(data.normalIntensity.x, data.normalIntensity.x, 1);
        texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
        normal = texNormal;
    }

    if (data.shadeModel_objectID.x == 0)
    {
        // PBR
        vec4 oAlbedo = textureAtlas(data.diffuse, fUv) + data.diffuseColor;
        vec4 oRoughnessMetallicAO;
        oRoughnessMetallicAO.r = textureAtlas(data.roughness, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.y;
        oRoughnessMetallicAO.g = textureAtlas(data.metallic, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.x;
        oRoughnessMetallicAO.b = textureAtlas(data.ambientOcclusion, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.z;
        oRoughnessMetallicAO.a = 1;

        oColor = vec4(1, 0, 1, 1);
    }
    else
    {
        // Phong
        vec4 diffuse = textureAtlas(data.diffuse, fUv) + data.diffuseColor;
        vec4 ambient = textureAtlas(data.ambient, fUv) + data.ambientColor;
        vec4 specular = textureAtlas(data.specular, fUv) + data.specularColor;
        float shininess = textureAtlas(data.shininess, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.w;

        LightComponents comp;
        comp.ambient = vec3(0, 0, 0);
        comp.diffuse = vec3(0, 0, 0);
        comp.specular = vec3(0, 0, 0);

        for (int i = 0; i < pLights.lights.length(); i++)
        {
            PointLight light = pLights.lights[i];
            LightComponents c = phong_point(fPos,
            normal,
            diffuse,
            specular,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < sLights.lights.length(); i++)
        {
            SpotLight light = sLights.lights[i];
            LightComponents c = phong_spot(fPos,
            normal,
            diffuse,
            specular,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < dLights.lights.length(); i++)
        {
            DirectionalLight light = dLights.lights[i];
            LightComponents c = phong_directional(fPos,
            normal,
            diffuse,
            specular,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        vec3 color = comp.ambient + comp.diffuse + comp.specular;
        oColor = vec4(color, diffuse.a);
    }
}
