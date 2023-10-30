#version 460

#include "phong.glsl"
#include "pbr.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

layout(binding = 0, std140) uniform ShaderUniformBuffer {
    vec4 viewPosition;
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
layout(binding = 11) uniform isampler2D gBufferModelObject;
layout(binding = 12) uniform sampler2D gBufferDepth;

void main()
{
    fPos = vec4(vPosition, 1);
    fUv = vUv;
    gl_Position = fPos;
}
