#version 460

layout(location = 0) in vec3 fPos;

layout(location = 0) out vec4 oColor;

layout(binding = 0, std140) buffer ShaderData {
    mat4 viewProjection;
} globs;

layout(binding = 1) uniform samplerCube skyboxTexture;

void main() {
    oColor = vec4(texture(skyboxTexture, fPos).rgb, 1);
}
