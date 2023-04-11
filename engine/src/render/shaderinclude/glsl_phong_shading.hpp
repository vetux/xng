/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_GLSL_PHONG_SHADING_HPP
#define XENGINE_GLSL_PHONG_SHADING_HPP

static const char *GLSL_PHONG_SHADING = R"###(
struct DirectionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
};

struct PointLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 constant_linear_quadratic;
};

struct SpotLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 direction_quadratic;
    vec4 cutOff_outerCutOff_constant_linear;
};

struct LightComponents
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

LightComponents phong_directional(vec3 fPos,
                                    vec3 fNorm,
                                    vec4 diffuseColor,
                                    vec4 specularColor,
                                    float roughness,
                                    vec3 viewPosition,
                                    mat3 lightTransformation,
                                    DirectionalLight light)
{
    LightComponents ret;

    vec3 ambient = light.ambient.xyz * vec3(diffuseColor.xyz);

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(-light.direction.xyz);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  light.diffuse.xyz * vec3((diff * diffuseColor).xyz);

    vec3 viewDir = normalize(viewPosition - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
    vec3 specular = light.specular.xyz * vec3((spec * specularColor).xyz);

    ret.ambient = ambient;
    ret.diffuse = diffuse;
    ret.specular = specular;

    return ret;
}

LightComponents phong_point(vec3 fPos,
                                vec3 fNorm,
                                vec4 diffuseColor,
                                vec4 specularColor,
                                float shininess,
                                vec3 viewPosition,
                                mat3 lightTransformation,
                                PointLight light)
{
    LightComponents ret;

    vec3 position = lightTransformation * light.position.xyz;
    float distance    = length(position - fPos);
    float attenuation = 1.0 / (light.constant_linear_quadratic.x + light.constant_linear_quadratic.y * distance + light.constant_linear_quadratic.z * (distance * distance));

    vec3 ambient = light.ambient.xyz * vec3(diffuseColor.xyz);

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(position - fPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  light.diffuse.xyz * vec3((diff * diffuseColor).xyz);

    vec3 viewDir = normalize(viewPosition - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular.xyz * vec3((spec * specularColor).xyz);

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    ret.ambient = ambient;
    ret.diffuse = diffuse;
    ret.specular = specular;

    return ret;
}

LightComponents phong_spot(vec3 fPos,
                            vec3 fNorm,
                            vec4 diffuseColor,
                            vec4 specularColor,
                            float roughness,
                            vec3 viewPosition,
                            mat3 lightTransformation,
                            SpotLight light)
{
    LightComponents ret;

    vec3 position = lightTransformation * light.position.xyz;
    vec3 lightDir = normalize(position - fPos);

    vec3 ambient = light.ambient.xyz * diffuseColor.rgb;

    vec3 norm = normalize(fNorm);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse.xyz * diff * diffuseColor.rgb;

    vec3 viewDir = normalize(viewPosition - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
    vec3 specular = light.specular.xyz * spec * specularColor.rgb;

    float theta = dot(lightDir, normalize(-light.direction_quadratic.xyz));
    float epsilon = (light.cutOff_outerCutOff_constant_linear.x - light.cutOff_outerCutOff_constant_linear.y);
    float intensity = clamp((theta - light.cutOff_outerCutOff_constant_linear.y) / epsilon, 0.0, 1.0);

    diffuse  *= intensity;
    specular *= intensity;

    float distance    = length(position - fPos);
    float attenuation = 1.0 / (light.cutOff_outerCutOff_constant_linear.z + light.cutOff_outerCutOff_constant_linear.w * distance + light.direction_quadratic.w * (distance * distance));

    diffuse   *= attenuation;
    specular *= attenuation;

    ret.ambient = ambient;
    ret.diffuse = diffuse;
    ret.specular = specular;

    return ret;
}
)###";

#endif //XENGINE_GLSL_PHONG_SHADING_HPP
