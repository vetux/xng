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
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight DIRECTIONAL_LIGHTS[MAX_LIGHTS];

uniform int DIRECTIONAL_LIGHTS_COUNT;

struct PointLight {
    vec3 position;
    float constantValue;
    float linearValue;
    float quadraticValue;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight POINT_LIGHTS[MAX_LIGHTS];

uniform int POINT_LIGHTS_COUNT;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constantValue;
    float linearValue;
    float quadraticValue;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform SpotLight SPOT_LIGHTS[MAX_LIGHTS];

uniform int SPOT_LIGHTS_COUNT;

struct LightComponents
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

LightComponents mana_calculate_light_directional(vec3 fPos, vec3 fNorm, vec4 diffuseColor, vec4 specularColor, float roughness, vec3 viewPosition, mat3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < DIRECTIONAL_LIGHTS_COUNT; i++)
    {
        vec3 ambient = DIRECTIONAL_LIGHTS[i].ambient * vec3(diffuseColor.xyz);

        vec3 norm = normalize(fNorm);
        vec3 lightDir = normalize(-DIRECTIONAL_LIGHTS[i].direction);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse =  DIRECTIONAL_LIGHTS[i].diffuse * vec3((diff * diffuseColor).xyz);

        vec3 viewDir = normalize(viewPosition - fPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
        vec3 specular = DIRECTIONAL_LIGHTS[i].specular * vec3((spec * specularColor).xyz);

        ret.ambient += ambient;
        ret.diffuse += diffuse;
        ret.specular += specular;
    }

    return ret;
}

LightComponents mana_calculate_light_point(vec3 fPos, vec3 fNorm, vec4 diffuseColor, vec4 specularColor, float shininess, vec3 viewPosition, mat3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < POINT_LIGHTS_COUNT; i++)
    {
        vec3 position = lightTransformation * POINT_LIGHTS[i].position;
        float distance    = length(position - fPos);
        float attenuation = 1.0 / (POINT_LIGHTS[i].constantValue + POINT_LIGHTS[i].linearValue * distance + POINT_LIGHTS[i].quadraticValue * (distance * distance));

        vec3 ambient = POINT_LIGHTS[i].ambient * vec3(diffuseColor.xyz);

        vec3 norm = normalize(fNorm);
        vec3 lightDir = normalize(position - fPos);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse =  POINT_LIGHTS[i].diffuse * vec3((diff * diffuseColor).xyz);

        vec3 viewDir = normalize(viewPosition - fPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = POINT_LIGHTS[i].specular * vec3((spec * specularColor).xyz);

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        ret.ambient += ambient;
        ret.diffuse += diffuse;
        ret.specular += specular;
    }

    return ret;
}

LightComponents mana_calculate_light_spot(vec3 fPos, vec3 fNorm, vec4 diffuseColor, vec4 specularColor, float roughness, vec3 viewPosition, mat3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < SPOT_LIGHTS_COUNT; i++)
    {
        vec3 position = lightTransformation * SPOT_LIGHTS[i].position;
        vec3 lightDir = normalize(position - fPos);

        vec3 ambient = SPOT_LIGHTS[i].ambient * diffuseColor.rgb;

        vec3 norm = normalize(fNorm);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = SPOT_LIGHTS[i].diffuse * diff * diffuseColor.rgb;

        vec3 viewDir = normalize(viewPosition - fPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
        vec3 specular = SPOT_LIGHTS[i].specular * spec * specularColor.rgb;

        float theta = dot(lightDir, normalize(-SPOT_LIGHTS[i].direction));
        float epsilon = (SPOT_LIGHTS[i].cutOff - SPOT_LIGHTS[i].outerCutOff);
        float intensity = clamp((theta - SPOT_LIGHTS[i].outerCutOff) / epsilon, 0.0, 1.0);

        diffuse  *= intensity;
        specular *= intensity;

        float distance    = length(position - fPos);
        float attenuation = 1.0 / (SPOT_LIGHTS[i].constantValue + SPOT_LIGHTS[i].linearValue * distance + SPOT_LIGHTS[i].quadraticValue * (distance * distance));

        diffuse   *= attenuation;
        specular *= attenuation;

        ret.ambient += ambient;
        ret.diffuse += diffuse;
        ret.specular += specular;
    }

    return ret;
}

LightComponents mana_calculate_light(vec3 fPos, vec3 fNorm, vec4 fDiffuse, vec4 fSpecular, float roughness, vec3 viewPosition, mat3 lightTransformation)
{
    LightComponents dirLight = mana_calculate_light_directional(fPos, fNorm, fDiffuse, fSpecular, roughness, viewPosition, lightTransformation);
    LightComponents pointLight = mana_calculate_light_point(fPos, fNorm, fDiffuse, fSpecular, roughness, viewPosition, lightTransformation);
    LightComponents spotLight = mana_calculate_light_spot(fPos, fNorm, fDiffuse, fSpecular, roughness, viewPosition, lightTransformation);

    LightComponents ret;
    ret.ambient = dirLight.ambient + pointLight.ambient + spotLight.ambient;
    ret.diffuse = dirLight.diffuse + pointLight.diffuse + spotLight.diffuse;
    ret.specular = dirLight.specular + pointLight.specular + spotLight.specular;
    return ret;
}
)###";

#endif //XENGINE_GLSL_PHONG_SHADING_HPP
