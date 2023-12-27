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
    ivec4 enableShadows;
} globs;

layout(binding = 1) uniform sampler2D gBufferPos;
layout(binding = 2) uniform sampler2D gBufferNormal;
layout(binding = 3) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 4) uniform sampler2D gBufferAlbedo;
layout(binding = 5) uniform isampler2D gBufferObjectShadows;
layout(binding = 6) uniform sampler2D gBufferDepth;

layout(binding = 7) uniform samplerCubeArray pointLightShadowMaps;
layout(binding = 8) uniform sampler2DArray dirLightShadowMaps;
layout(binding = 9) uniform sampler2DArray spotLightShadowMaps;

layout(binding = 10, std140) buffer PointLightsData
{
    PBRPointLight lights[];
} pointLights;

layout(binding = 11, std140) buffer ShadowPointLightsData
{
    PBRPointLight lights[];
} pointLightsShadow;

layout(binding = 12, std140) buffer DirectionalLightsData
{
    PBRDirectionalLight lights[];
} directionalLights;

layout(binding = 13, std140) buffer ShadowDirectionalLightsData
{
    PBRDirectionalLight lights[];
} directionalLightsShadow;

layout(binding = 14, std140) buffer SpotLightsData
{
    PBRSpotLight lights[];
} spotLights;

layout(binding = 15, std140) buffer ShadowSpotLightsData
{
    PBRSpotLight lights[];
} spotLightsShadow;

layout(binding = 16, std140) buffer DirectionalLightTransforms
{
    mat4 transforms[];
} dirLightTransforms;

layout(binding = 17, std140) buffer SpotLightTransforms
{
    mat4 transforms[];
} spotLightTransforms;

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
    vec3 albedo = texture(gBufferAlbedo, fUv).xyz;

    PbrPass pass = pbr_begin(fPos,
    fNorm,
    albedo,
    roughnessMetallicAO.y,
    roughnessMetallicAO.x,
    roughnessMetallicAO.z,
    globs.viewPosition.xyz);

    vec3 reflectance = vec3(0);

    for (int i = 0; i < pointLights.lights.length(); i++) {
        PBRPointLight light = pointLights.lights[i];
        reflectance = pbr_point(pass, reflectance, light, 1);
    }

    for (int i = 0; i < directionalLights.lights.length(); i++) {
        PBRDirectionalLight light = directionalLights.lights[i];
        reflectance = pbr_directional(pass, reflectance, light, 1);
    }

    for (int i = 0; i < spotLights.lights.length(); i++) {
        PBRSpotLight light = spotLights.lights[i];
        reflectance = pbr_spot(pass, reflectance, light, 1);
    }

    if (receiveShadows == 0 || globs.enableShadows.x == 0){
        for (int i = 0; i < pointLightsShadow.lights.length(); i++) {
            PBRPointLight light = pointLightsShadow.lights[i];
            reflectance = pbr_point(pass, reflectance, light, 1);
        }
        for (int i = 0; i < directionalLightsShadow.lights.length(); i++) {
            PBRDirectionalLight light = directionalLightsShadow.lights[i];
            reflectance = pbr_directional(pass, reflectance, light, 1);
        }
        for (int i = 0; i < spotLightsShadow.lights.length(); i++) {
            PBRSpotLight light = spotLightsShadow.lights[i];
            reflectance = pbr_spot(pass, reflectance, light, 1);
        }
    } else {
        for (int i = 0; i < pointLightsShadow.lights.length(); i++) {
            PBRPointLight light = pointLightsShadow.lights[i];
            float shadow = sampleShadow(fPos, light.position.xyz, globs.viewPosition.xyz, pointLightShadowMaps, i, light.farPlane.x);
            reflectance = pbr_point(pass, reflectance, light, shadow);
        }

        for (int i = 0; i < directionalLightsShadow.lights.length(); i++) {
            PBRDirectionalLight light = directionalLightsShadow.lights[i];
            vec4 fragPosLightSpace = dirLightTransforms.transforms[i] * vec4(fPos, 1);
            float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                    dirLightShadowMaps,
                                                    i,
                                                    fNorm,
                                                    vec3(0),
                                                    fPos);
            reflectance = pbr_directional(pass, reflectance, light, shadow);
        }
        for (int i = 0; i < spotLightsShadow.lights.length(); i++) {
            PBRSpotLight light = spotLightsShadow.lights[i];
            vec4 fragPosLightSpace = spotLightTransforms.transforms[i] * vec4(fPos, 1);
            float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                    spotLightShadowMaps,
                                                    i,
                                                    fNorm,
                                                    light.position.xyz,
                                                    fPos);
            reflectance = pbr_spot(pass, reflectance, light, shadow);
        }
    }

    oColor = vec4(pbr_finish(pass, reflectance), 1);
    gl_FragDepth = gDepth;
}
