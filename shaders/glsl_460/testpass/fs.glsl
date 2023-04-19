#version 460

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    vec4 visualizeDepth_near_far;
} globs;

layout(binding = 1) uniform sampler2D tex;

void main() {
    oColor = texture(tex, fUv);
    if (globs.visualizeDepth_near_far.x != 0)
    {
        float near = globs.visualizeDepth_near_far.y;
        float far = globs.visualizeDepth_near_far.z;
        float ndc = oColor.r * 2.0 - 1.0;
        float linearDepth = 1 - ((2.0 * near * far) / (far + near - ndc * (far - near)) / far);
        oColor = vec4(linearDepth, linearDepth, linearDepth, 1);
    }
}
