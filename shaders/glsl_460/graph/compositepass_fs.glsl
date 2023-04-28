#version 460

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;
layout(depth_any) out float gl_FragDepth;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D depthTex;

void main() {
    oColor = texture(colorTex, fUv);
    gl_FragDepth = texture(depthTex, fUv).r;
}
