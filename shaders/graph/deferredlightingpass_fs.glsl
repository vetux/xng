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

layout(binding = 1) uniform sampler2D gBufferPos;
layout(binding = 2) uniform sampler2D gBufferNormal;
layout(binding = 3) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 4) uniform sampler2D gBufferAlbedo;
layout(binding = 5) uniform isampler2D gBufferObjectShadows;
layout(binding = 6) uniform sampler2D gBufferDepth;

layout(binding = 7) uniform samplerCubeArray pointLightShadowMaps;

layout(binding = 8, std140) buffer PointLightsData
{
    PBRPointLight lights[];
} pointLights;

layout(binding = 9, std140) buffer ShadowPointLightsData
{
    PBRPointLight lights[];
} pointLightsShadow;

void main() {
    float gDepth = texture(gBufferDepth, fUv).r;
    if (gDepth == 1) {
        oColor = vec4(0, 0, 0, 0);
        gl_FragDepth = 1;
        return;
    }

    int receiveShadows = texture(gBufferObjectShadows, fUv).y;

    vec3 fPos = texture(gBufferPos, fUv).xyz;
    vec3 fNorm = texture(gBufferNormal, fUv).xyz;
    vec3 roughnessMetallicAO = texture(gBufferRoughnessMetallicAO, fUv).xyz;
    vec4 albedo = texture(gBufferAlbedo, fUv);

    PbrPass pass = pbr_begin(fPos,
                                fNorm,
                                albedo.rgb,
                                roughnessMetallicAO.y,
                                roughnessMetallicAO.x,
                                roughnessMetallicAO.z,
                                globs.viewPosition.xyz);

    vec3 reflectance = vec3(0);

    for (int i = 0; i < pointLights.lights.length(); i++) {
        PBRPointLight light = pointLights.lights[i];
        reflectance = pbr_point(pass, reflectance, light);
    }

    if (receiveShadows == 0 || globs.enableShadows.x == 0){
        for (int i = 0; i < pointLightsShadow.lights.length(); i++) {
            PBRPointLight light = pointLightsShadow.lights[i];
            reflectance = pbr_point(pass, reflectance, light);
        }
    } else {
        for (int i = 0; i < pointLightsShadow.lights.length(); i++) {
            PBRPointLight light = pointLightsShadow.lights[i];
            float shadow = sampleShadow(fPos, light.position.xyz, globs.viewPosition.xyz, pointLightShadowMaps, i, globs.farPlane.x);
            reflectance = pbr_point(pass, reflectance, light) * shadow;
        }
    }

    oColor = vec4(pbr_finish(pass, reflectance), albedo.a);
    gl_FragDepth = gDepth;
}
