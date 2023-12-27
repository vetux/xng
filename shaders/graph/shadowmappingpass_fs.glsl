#version 460

layout(location = 0) in vec4 FragPos;

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

layout(binding = 2, std140) buffer PointLightDataBuffer
{
    vec4 lightPosFarPlane;
    ivec4 layer;
    mat4 shadowMatrices[6];
} lightData;

void main() {
    float lightDistance = length(FragPos.xyz - lightData.lightPosFarPlane.xyz);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / lightData.lightPosFarPlane.w;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}