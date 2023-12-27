#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

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

void main()
{
    gl_Layer = lightData.layer.x; // Set the layer

    // Draw the triangle
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}

