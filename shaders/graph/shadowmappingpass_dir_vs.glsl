#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;

struct DrawData {
    ivec4 boneOffset;
    mat4 model;
};

layout(binding = 0, std140) buffer DrawDataBuffer
{
    DrawData data[];
} drawData;

layout(binding = 1, std140) buffer BoneBuffer
{
    mat4 matrices[];
} bones;

layout(binding = 2, std140) buffer DirLightDataBuffer
{
    ivec4 layer;
    mat4 shadowMatrix;
} lightData;

vec4 getSkinnedVertexPosition(int offset) {
    if (offset < 0) {
        return vec4(vPosition, 1.0f);
    }

    int boneCount = bones.matrices.length();

    vec4 totalPosition = vec4(0, 0, 0, 0);
    if (boneIds.x > -1)
    {
        if (boneIds.x + offset >= boneCount) {
            return vec4(vPosition, 1.0f);
        } else {
            vec4 localPosition = bones.matrices[boneIds.x + offset] * vec4(vPosition, 1.0f);
            totalPosition += localPosition * boneWeights.x;
        }
    }

    if (boneIds.y > -1)
    {
        if (boneIds.y + offset >= boneCount) {
            return vec4(vPosition, 1.0f);
        } else {
            vec4 localPosition = bones.matrices[boneIds.y + offset] * vec4(vPosition, 1.0f);
            totalPosition += localPosition * boneWeights.y;
        }
    }

    if (boneIds.z > -1)
    {
        if (boneIds.z + offset >= boneCount) {
            return vec4(vPosition, 1.0f);
        } else {
            vec4 localPosition = bones.matrices[boneIds.z + offset] * vec4(vPosition, 1.0f);
            totalPosition += localPosition * boneWeights.z;
        }
    }

    if (boneIds.w > -1)
    {
        if (boneIds.w + offset >= boneCount) {
            return vec4(vPosition, 1.0f);
        } else {
            vec4 localPosition = bones.matrices[boneIds.w + offset] * vec4(vPosition, 1.0f);
            totalPosition += localPosition * boneWeights.w;
        }
    }

    return totalPosition;
}

void main()
{
    gl_Position = lightData.shadowMatrix * drawData.data[gl_DrawID].model * getSkinnedVertexPosition(drawData.data[gl_DrawID].boneOffset.x);
}