#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;

layout(location = 0) out vec3 fPos;

layout(binding = 0, std140) buffer ShaderData {
    mat4 viewProjection;
} globs;

void main()
{
    fPos = vPosition;
    gl_Position = globs.viewProjection * vec4(vPosition, 1);
}
