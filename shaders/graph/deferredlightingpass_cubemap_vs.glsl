#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout(location = 0) out vec3 WorldPos;

layout (binding = 0, std140) buffer TransformData {
    mat4 projection;
    mat4 view;
} data;

void main()
{
    WorldPos = position;
    gl_Position =  data.projection * data.view * vec4(WorldPos, 1.0);
}
