#version 460

#include "phong.glsl"
#include "pbr.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

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

void main()
{
    fPos = vec4(vPosition, 1);
    fUv = vUv;
    gl_Position = fPos;
}
