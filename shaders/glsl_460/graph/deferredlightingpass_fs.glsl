#version 460

#include "phong.glsl"

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;
layout(depth_any) out float gl_FragDepth;

layout(binding = 0, std140) uniform ShaderUniformBuffer {
    vec4 viewPosition;
} globs;

layout(binding = 1, std140) buffer PointLightsData
{
    PointLight lights[];
} pLights;

layout(binding = 2, std140) buffer SpotLightsData
{
    SpotLight lights[];
} sLights;

layout(binding = 3, std140) buffer DirectionalLightsData
{
    DirectionalLight lights[];
} dLights;

layout(binding = 4) uniform sampler2D gBufferPos;
layout(binding = 5) uniform sampler2D gBufferNormal;
layout(binding = 6) uniform sampler2D gBufferRoughnessMetallicAO;
layout(binding = 7) uniform sampler2D gBufferAlbedo;
layout(binding = 8) uniform sampler2D gBufferAmbient;
layout(binding = 9) uniform sampler2D gBufferSpecular;
layout(binding = 10) uniform isampler2D gBufferModelObject;
layout(binding = 11) uniform sampler2D gBufferDepth;

void main() {
    float gDepth = texture(gBufferDepth, fUv).r;
    if (gDepth == 1) {
        discard;
        return;
    }

    int model = texture(gBufferModelObject, fUv).x;
    if (model == 0)
    {
        // PBR
        oColor = vec4(1, 0, 1, 1);
        gl_FragDepth = 0;
    } else if (model == 1) {
        // Phong
        vec3 fPos = texture(gBufferPos, fUv).xyz;
        vec3 fNorm = texture(gBufferNormal, fUv).xyz;
        vec4 diffuseColor = texture(gBufferAlbedo, fUv);
        vec4 specularColor = texture(gBufferSpecular, fUv);
        float shininess = texture(gBufferRoughnessMetallicAO, fUv).x;

        LightComponents comp;

        for (int i = 0; i < pLights.lights.length(); i++)
        {
            PointLight light = pLights.lights[i];
            LightComponents c = phong_point(fPos,
                                            fNorm,
                                            diffuseColor,
                                            specularColor,
                                            shininess,
                                            globs.viewPosition.xyz,
                                            mat3(1),
                                            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < sLights.lights.length(); i++)
        {
            SpotLight light = sLights.lights[i];
            LightComponents c = phong_spot(fPos,
                                            fNorm,
                                            diffuseColor,
                                            specularColor,
                                            shininess,
                                            globs.viewPosition.xyz,
                                            mat3(1),
                                            light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        for (int i = 0; i < dLights.lights.length(); i++)
        {
            DirectionalLight light = dLights.lights[i];
            LightComponents c = phong_directional(fPos,
                                                    fNorm,
                                                    diffuseColor,
                                                    specularColor,
                                                    shininess,
                                                    globs.viewPosition.xyz,
                                                    mat3(1),
                                                    light);
            comp.ambient += c.ambient;
            comp.diffuse += c.diffuse;
            comp.specular += c.specular;
        }

        vec3 color = comp.ambient + comp.diffuse + comp.specular;
        oColor = vec4(color, 1);
        gl_FragDepth = gDepth;
    } else {
        oColor = vec4(1, 0, 1, 1);
        gl_FragDepth = 0;
    }
}
