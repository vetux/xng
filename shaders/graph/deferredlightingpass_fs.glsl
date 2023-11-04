#version 460

#include "phong.glsl"
#include "pbr.glsl"
#include "shadow.glsl"

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;
layout(depth_any) out float gl_FragDepth;

layout(binding = 0, std140) buffer ShaderData {
    vec4 viewPosition;
    vec4 farPlane;
    ivec4 enableShadows;
} globs;

layout(binding = 1, std140) buffer PointLightsData
{
    PointLight lights[];
} pLights;

layout(binding = 2, std140) buffer SpotLightsData
{
    SpotLight lights[];
} sLights;

layout(binding = 3, std140) buffer DirectionalLightsData
{
    DirectionalLight lights[];
} dLights;

layout(binding = 4, std140) buffer PBRPointLightsData
{
    PBRPointLight lights[];
} pbrPointLights;

layout(binding = 5) uniform sampler2D gBufferPos;
layout(binding = 6) uniform sampler2D gBufferNormal;
layout(binding = 7) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 8) uniform sampler2D gBufferAlbedo;
layout(binding = 9) uniform sampler2D gBufferAmbient;
layout(binding = 10) uniform sampler2D gBufferSpecular;
layout(binding = 11) uniform isampler2D gBufferModelObjectShadows;
layout(binding = 12) uniform sampler2D gBufferDepth;

layout(binding = 13) uniform samplerCubeArray pbrPointLightShadowMaps;

layout(binding = 14, std140) buffer PBRPointLightsDataShadow
{
    PBRPointLight lights[];
} pbrPointLightsShadow;

void main() {
    float gDepth = texture(gBufferDepth, fUv).r;
    if (gDepth == 1) {
        oColor = vec4(0, 0, 0, 0);
        gl_FragDepth = 1;
        return;
    }

    int model = texture(gBufferModelObjectShadows, fUv).x;
    int receiveShadows = texture(gBufferModelObjectShadows, fUv).z;
    if (model == 0) {
        // PBR
        vec3 fPos = texture(gBufferPos, fUv).xyz;
        vec3 fNorm = texture(gBufferNormal, fUv).xyz;
        vec3 roughnessMetallicAO = texture(gBufferRoughnessMetallicAO, fUv).xyz;
        vec3 albedo = texture(gBufferAlbedo, fUv).xyz;

        PbrPass pass = pbr_begin(fPos,
        fNorm,
        albedo,
        roughnessMetallicAO.y,
        roughnessMetallicAO.x,
        roughnessMetallicAO.z,
        globs.viewPosition.xyz);

        vec3 reflectance = vec3(0);

        for (int i = 0; i < pbrPointLights.lights.length(); i++) {
            PBRPointLight light = pbrPointLights.lights[i];
            reflectance = pbr_point(pass, reflectance, light);
        }

        if (receiveShadows == 0 || globs.enableShadows.x == 0){
            for (int i = 0; i < pbrPointLightsShadow.lights.length(); i++) {
                PBRPointLight light = pbrPointLightsShadow.lights[i];
                reflectance = pbr_point(pass, reflectance, light);
            }
        } else {
            for (int i = 0; i < pbrPointLightsShadow.lights.length(); i++) {
                PBRPointLight light = pbrPointLightsShadow.lights[i];
                float shadow = sampleShadow(fPos, light.position.xyz, globs.viewPosition.xyz, pbrPointLightShadowMaps, i, globs.farPlane.x);
                reflectance = pbr_point(pass, reflectance, light) * shadow;
            }
        }

        oColor = vec4(pbr_finish(pass, reflectance), 1);
        gl_FragDepth = gDepth;
    } else if (model == 1) {
        // Phong
        vec3 fPos = texture(gBufferPos, fUv).xyz;
        vec3 fNorm = texture(gBufferNormal, fUv).xyz;
        vec4 diffuseColor = texture(gBufferAlbedo, fUv);
        vec4 specularColor = texture(gBufferSpecular, fUv);
        float shininess = texture(gBufferRoughnessMetallicAO, fUv).x;

        LightComponents comp;
        comp.ambient = vec3(0, 0, 0);
        comp.diffuse = vec3(0, 0, 0);
        comp.specular = vec3(0, 0, 0);

        for (int i = 0; i < pLights.lights.length(); i++) {
            PointLight light = pLights.lights[i];
            LightComponents c = phong_point(fPos,
            fNorm,
            diffuseColor,
            specularColor,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < sLights.lights.length(); i++) {
            SpotLight light = sLights.lights[i];
            LightComponents c = phong_spot(fPos,
            fNorm,
            diffuseColor,
            specularColor,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < dLights.lights.length(); i++) {
            DirectionalLight light = dLights.lights[i];
            LightComponents c = phong_directional(fPos,
            fNorm,
            diffuseColor,
            specularColor,
            shininess,
            globs.viewPosition.xyz,
            mat3(1),
            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        vec3 color = comp.ambient + comp.diffuse + comp.specular;
        oColor = vec4(color, 1);
        gl_FragDepth = gDepth;
    } else {
        oColor = vec4(1, 0, 1, 1);
        gl_FragDepth = 0;
    }
}
