#version 460

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

layout(depth_any) out float gl_FragDepth;

void main() {
    gl_FragDepth = gl_FragCoord.z;
}