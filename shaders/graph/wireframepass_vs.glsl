#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec4 vPos;
layout(location = 5) out vec3 fT;
layout(location = 6) out vec3 fB;
layout(location = 7) out vec3 fN;
layout(location = 8) flat out uint drawID;

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;
    ivec4 objectID_boneOffset;
    vec4 wireColor;
};

layout(binding = 0, std140) buffer ShaderUniformBuffer
{
    ShaderDrawData data[];
} globs;

layout(binding = 1, std140) buffer BoneBuffer
{
    mat4 matrices[];
} bones;

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
    ShaderDrawData data = globs.data[gl_DrawID];

    vec4 pos = getSkinnedVertexPosition(data.objectID_boneOffset.y);

    vPos = data.mvp * pos;
    fPos = (data.model * pos).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
    fN = normalize((data.model * vec4(normalize(vNormal), 0.0)).xyz);
    fT = normalize((data.model * vec4(normalize(vTangent), 0.0)).xyz);
    fB = normalize((data.model * vec4(cross(normalize(vNormal), normalize(vTangent).xyz) * 1, 0.0)).xyz);

    gl_Position = vPos;

    drawID = gl_DrawID;
}
