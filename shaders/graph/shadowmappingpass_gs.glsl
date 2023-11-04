#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

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

layout(binding = 2, std140) buffer LightDataBuffer
{
    vec4 lightPosFarPlane;
    ivec4 layer;
    mat4 shadowMatrices[6];
} lightData;

layout(location = 0) out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = (lightData.layer.x * 6) + face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightData.shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

