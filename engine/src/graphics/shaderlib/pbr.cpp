/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "xng/graphics/shaderlib/pbr.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/pi.hpp"
#include "../../../include/xng/rendergraph/resource/rendergraphtexture.hpp"
#include "xng/rendergraph/shaderscript/macro/helpermacros.hpp"

using namespace xng::ShaderScript;

// Adapted from the tutorial at https://learnopengl.com/PBR/Lighting
namespace xng::shaderlib
{
    Float DistributionGGX(Param<vec3> N, Param<vec3> H, Param<Float> roughness)
    {
        IRFunction
        Float a = roughness * roughness;
        Float a2 = a * a;
        Float NdotH = max(dot(N, H), 0.0f);
        Float NdotH2 = NdotH * NdotH;

        Float nom = a2;
        Float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
        denom = pi() * denom * denom;

        IRReturn(Float(nom / denom));
        IRFunctionEnd
    }

    Float GeometrySchlickGGX(Param<Float> NdotV, Param<Float> roughness)
    {
        IRFunction
        Float r = (roughness + 1.0f);
        Float k = (r * r) / 8.0f;

        Float denom = NdotV * (1.0f - k) + k;

        IRReturn(Float(NdotV / denom));
        IRFunctionEnd
    }

    Float GeometrySmith(Param<vec3> N, Param<vec3> V, Param<vec3> L, Param<Float> roughness)
    {
        IRFunction
        Float NdotV = max(dot(N, V), 0.0f);
        Float NdotL = max(dot(N, L), 0.0f);
        Float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        Float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        IRReturn(Float(ggx1 * ggx2));
        IRFunctionEnd
    }

    vec3 FresnelSchlick(Param<Float> cosTheta, Param<vec3> F0)
    {
        IRFunction
        IRReturn(vec3(F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f)));
        IRFunctionEnd
    }

    vec3 FresnelSchlickRoughness(Param<Float> cosTheta, Param<vec3> F0, Param<Float> roughness)
    {
        IRFunction
        IRReturn(vec3(F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f)));
        IRFunctionEnd
    }

    pbr::PbrPass pbr::pbr_begin(Param<vec3> WorldPos,
                                Param<vec3> Normal,
                                Param<vec3> albedo,
                                Param<Float> metallic,
                                Param<Float> roughness,
                                Param<Float> ao,
                                Param<vec3> camPos,
                                Param<Float> gamma)
    {
        IRFunction
        PbrPass ret;

        vec3 N = normalize(Normal);
        vec3 V = normalize(camPos - WorldPos);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
        vec3 F0;
        F0 = vec3(0.04f, 0.04f, 0.04f);
        F0 = mix(F0, albedo, metallic);

        ret.N = N;
        ret.V = V;
        ret.F0 = F0;

        ret.WorldPos = WorldPos;
        ret.Normal = Normal;
        ret.albedo = albedo;
        ret.metallic = metallic;
        ret.roughness = roughness;
        ret.ao = ao;
        ret.camPos = camPos;
        ret.gamma = gamma;
        ret.iblIrradiance = vec3(0.0f, 0.0f, 0.0f);
        ret.iblPrefilter = vec3(0.0f, 0.0f, 0.0f);
        ret.iblBRDF = vec2(0.0f, 0.0f);

        IRReturn(ret);
        IRFunctionEnd
    }

    vec3 pbr::pbr_point(Param<PbrPass> pass,
                        Param<vec3> Lo,
                        Param<vec3> position,
                        Param<vec3> color,
                        Param<Float> shadow)
    {
        IRFunction
        vec3 N = pass.value().N;
        vec3 V = pass.value().V;
        vec3 F0 = pass.value().F0;
        vec3 WorldPos = pass.value().WorldPos;
        vec3 Normal = pass.value().Normal;
        vec3 albedo = pass.value().albedo;
        Float metallic = pass.value().metallic;
        Float roughness = pass.value().roughness;
        Float ao = pass.value().ao;
        vec3 camPos = pass.value().camPos;

        // calculate per-light radiance
        vec3 L = normalize(position - WorldPos);
        vec3 H = normalize(V + L);
        Float distance = length(position - WorldPos);
        Float attenuation = 1.0f / (distance * distance);
        vec3 radiance = color * attenuation;

        // Cook-Torrance BRDF
        Float NDF = DistributionGGX(N, H, roughness);
        Float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;

        vec3 numerator = NDF * G * F;
        Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
        // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // scale light by NdotL
        Float NdotL = max(dot(N, L), 0.0f);

        // add to outgoing radiance Lo
        vec3 ret = Lo;
        ret += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

        IRReturn(ret);
        IRFunctionEnd
    }

    vec3 pbr::pbr_directional(Param<PbrPass> pass,
                              Param<vec3> Lo,
                              Param<vec3> direction,
                              Param<vec3> color,
                              Param<Float> shadow)
    {
        IRFunction
        vec3 N = pass.value().N;
        vec3 V = pass.value().V;
        vec3 F0 = pass.value().F0;
        vec3 WorldPos = pass.value().WorldPos;
        vec3 Normal = pass.value().Normal;
        vec3 albedo = pass.value().albedo;
        Float metallic = pass.value().metallic;
        Float roughness = pass.value().roughness;
        Float ao = pass.value().ao;
        vec3 camPos = pass.value().camPos;

        // calculate per-light radiance
        vec3 L = normalize(direction);
        vec3 H = normalize(V + L);

        Float angle = acos(dot(normalize(direction), normalize(Normal)));

        vec3 radiance = color * cos(angle);

        // Cook-Torrance BRDF
        Float NDF = DistributionGGX(N, H, roughness);
        Float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;

        vec3 numerator = NDF * G * F;
        Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
        // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // scale light by NdotL
        Float NdotL = max(dot(N, L), 0.0f);

        // add to outgoing radiance Lo
        vec3 ret = Lo;
        ret += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

        IRReturn(ret);
        IRFunctionEnd
    }

    vec3 pbr::pbr_spot(Param<PbrPass> pass,
                       Param<vec3> Lo,
                       Param<vec3> position,
                       Param<vec3> direction,
                       Param<Float> quadratic,
                       Param<vec3> color,
                       Param<Float> cutOff,
                       Param<Float> outerCutOff,
                       Param<Float> constant,
                       Param<Float> linear,
                       Param<Float> shadow)
    {
        IRFunction
        vec3 N = pass.value().N;
        vec3 V = pass.value().V;
        vec3 F0 = pass.value().F0;
        vec3 WorldPos = pass.value().WorldPos;
        vec3 Normal = pass.value().Normal;
        vec3 albedo = pass.value().albedo;
        Float metallic = pass.value().metallic;
        Float roughness = pass.value().roughness;
        Float ao = pass.value().ao;
        vec3 camPos = pass.value().camPos;

        vec3 lightDir = normalize(position - WorldPos);

        Float theta = dot(lightDir, normalize(direction));
        Float epsilon = (cutOff - outerCutOff);
        Float intensity = clamp((theta - outerCutOff) / epsilon, 0.0f, 1.0f);

        vec3 L = normalize(position - WorldPos);
        vec3 H = normalize(V + L);
        Float distance = length(position - WorldPos);
        Float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));

        vec3 radiance = color * attenuation * intensity;

        // Cook-Torrance BRDF
        Float NDF = DistributionGGX(N, H, roughness);
        Float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;

        vec3 numerator = NDF * G * F;
        Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
        // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // scale light by NdotL
        Float NdotL = max(dot(N, L), 0.0f);

        // add to outgoing radiance Lo
        vec3 ret = Lo;
        ret += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

        IRReturn(ret);
        IRFunctionEnd
    }

    vec3 pbr::pbr_finish(Param<PbrPass> pass, Param<vec3> Lo)
    {
        IRFunction
        // Image based lighting ambient contribution
        vec3 N = pass.value().N;
        vec3 V = pass.value().V;
        vec3 F0 = pass.value().F0;
        vec3 WorldPos = pass.value().WorldPos;
        vec3 Normal = pass.value().Normal;
        vec3 albedo = pass.value().albedo;
        Float metallic = pass.value().metallic;
        Float roughness = pass.value().roughness;
        Float ao = pass.value().ao;
        vec3 camPos = pass.value().camPos;

        // Use IBL values provided on the PbrPass by the lighting pass
        vec3 irradiance = pass.value().iblIrradiance;
        vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - metallic;

        vec3 diffuse = irradiance * albedo;

        vec3 prefilteredColor = pass.value().iblPrefilter;
        vec2 brdf = pass.value().iblBRDF;
        vec3 specular = prefilteredColor * (F0 * brdf.x() + brdf.y());

        vec3 ambient = (kD * diffuse + specular) * ao;

        vec3 color = ambient + Lo;

        // HDR tonemapping
        color = color / (color + vec3(1.0));
        // gamma correct
        color = pow(color, vec3(1.0 / pass.value().gamma));

        IRReturn(color);
        IRFunctionEnd
    }
}
