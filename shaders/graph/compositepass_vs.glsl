#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D depthTex;

void main()
{
    fPos = vec4(vPosition, 1);
    fUv = vUv;
    gl_Position = fPos;
}
