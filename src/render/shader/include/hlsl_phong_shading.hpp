/**
 *  Mana - 3D Game Engine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MANA_HLSL_PHONG_SHADING_HPP
#define MANA_HLSL_PHONG_SHADING_HPP

static const char *HLSL_PHONG_SHADING = R"###(
struct DirectionalLight {
    float3 direction;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

DirectionalLight DIRECTIONAL_LIGHTS[MAX_LIGHTS];

int DIRECTIONAL_LIGHTS_COUNT;

struct PointLight {
    float3 position;
    float constantValue;
    float linearValue;
    float quadraticValue;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

PointLight POINT_LIGHTS[MAX_LIGHTS];

int POINT_LIGHTS_COUNT;

struct SpotLight {
    float3 position;
    float3 direction;
    float cutOff;
    float outerCutOff;
    float constantValue;
    float linearValue;
    float quadraticValue;
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

SpotLight SPOT_LIGHTS[MAX_LIGHTS];

int SPOT_LIGHTS_COUNT;

struct LightComponents
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
};

LightComponents mana_calculate_light_directional(float3 fPos, float3 fNorm, float4 diffuseColor, float4 specularColor, float roughness, float3 viewPosition, float3x3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < DIRECTIONAL_LIGHTS_COUNT; i++)
    {
        float3 ambient = DIRECTIONAL_LIGHTS[i].ambient * float3(diffuseColor.xyz);

        float3 norm = normalize(fNorm);
        float3 lightDir = normalize(-DIRECTIONAL_LIGHTS[i].direction);

        float diff = max(dot(norm, lightDir), 0.0);
        float3 diffuse =  DIRECTIONAL_LIGHTS[i].diffuse * float3((diff * diffuseColor).xyz);

        float3 viewDir = normalize(viewPosition - fPos);
        float3 reflectDir = normalize(reflect(-lightDir, norm));
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
        float3 specular = DIRECTIONAL_LIGHTS[i].specular * float3((spec * specularColor).xyz);

        ret.ambient += ambient;
        ret.diffuse += diffuse;
        ret.specular += specular;
    }

    return ret;
}

LightComponents mana_calculate_light_point(float3 fPos, float3 fNorm, float4 diffuseColor, float4 specularColor, float shininess, float3 viewPosition, float3x3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < POINT_LIGHTS_COUNT; i++)
    {
        float3 position = POINT_LIGHTS[i].position * lightTransformation;
        float distance    = length(position - fPos);
        float attenuation = 1.0 / (POINT_LIGHTS[i].constantValue + POINT_LIGHTS[i].linearValue * distance + POINT_LIGHTS[i].quadraticValue * (distance * distance));

        float3 ambient = POINT_LIGHTS[i].ambient * float3(diffuseColor.xyz);

        float3 norm = normalize(fNorm);
        float3 lightDir = normalize(position - fPos);

        float diff = max(dot(norm, lightDir), 0.0);
        float3 diffuse =  POINT_LIGHTS[i].diffuse * float3((diff * diffuseColor).xyz);

        float3 viewDir = normalize(viewPosition - fPos);
        float3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        float3 specular = POINT_LIGHTS[i].specular * float3((spec * specularColor).xyz);

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        ret.ambient += ambient;
        ret.diffuse += diffuse;
        ret.specular += specular;
    }

    return ret;
}

LightComponents mana_calculate_light_spot(float3 fPos, float3 fNorm, float4 diffuseColor, float4 specularColor, float roughness, float3 viewPosition, float3x3 lightTransformation)
{
    LightComponents ret;

    for (int i = 0; i < SPOT_LIGHTS_COUNT; i++)
    {
        float3 position = SPOT_LIGHTS[i].position * lightTransformation;
        float3 lightDir = normalize(position - fPos);

        float3 ambient = SPOT_LIGHTS[i].ambient * diffuseColor.rgb;

        float3 norm = normalize(fNorm);
        float diff = max(dot(norm, lightDir), 0.0);
        float3 diffuse = SPOT_LIGHTS[i].diffuse * diff * diffuseColor.rgb;

        float3 viewDir = normalize(viewPosition - fPos);
        float3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), roughness);
        float3 specular = SPOT_LIGHTS[i].specular * spec * specularColor.rgb;

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

LightComponents mana_calculate_light(float3 fPos, float3 fNorm, float4 fDiffuse, float4 fSpecular, float roughness, float3 viewPosition, float3x3 lightTransformation)
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

#endif //MANA_HLSL_PHONG_SHADING_HPP
