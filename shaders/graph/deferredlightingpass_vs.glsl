#version 460

#include "phong.glsl"
#include "pbr.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

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

void main()
{
    fPos = vec4(vPosition, 1);
    fUv = vUv;
    gl_Position = fPos;
}
