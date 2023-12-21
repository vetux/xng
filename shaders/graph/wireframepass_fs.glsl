#version 460

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;
layout(location = 5) in vec3 fT;
layout(location = 6) in vec3 fB;
layout(location = 7) in vec3 fN;
layout(location = 8) flat in uint drawID;

layout(location = 0) out vec4 oColor;

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

void main() {
    oColor = globs.data[drawID].wireColor;
}
