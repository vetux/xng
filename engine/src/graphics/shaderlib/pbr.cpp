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

using namespace xng::ShaderScript;

// Adapted from the tutorial at https://learnopengl.com/PBR/Lighting
namespace xng::shaderlib {
    DEFINE_FUNCTION3(DistributionGGX)
    DEFINE_FUNCTION2(GeometrySchlickGGX)
    DEFINE_FUNCTION4(GeometrySmith)
    DEFINE_FUNCTION2(FresnelSchlick)

    void pbr() {
        auto &builder = ShaderBuilder::instance();
        Struct(PbrPass,
               {ShaderDataType::vec3(), "N"},
               {ShaderDataType::vec3(), "V"},
               {ShaderDataType::vec3(), "F0"},
               {ShaderDataType::vec3(), "WorldPos"},
               {ShaderDataType::vec3(), "Normal"},
               {ShaderDataType::vec3(), "albedo"},
               {ShaderDataType::float32(), "metallic"},
               {ShaderDataType::float32(), "roughness"},
               {ShaderDataType::float32(), "ao"},
               {ShaderDataType::vec3(), "camPos"});

        Function("DistributionGGX",
                 {
                     {"N", ShaderDataType::vec3()},
                     {"H", ShaderDataType::vec3()},
                     {"roughness", ShaderDataType::float32()},
                 },
                 ShaderDataType::float32());
        {
            ARGUMENT(N)
            ARGUMENT(H)
            ARGUMENT(roughness)

            Float a = roughness * roughness;
            Float a2 = a * a;
            Float NdotH = max(dot(N, H), 0.0f);
            Float NdotH2 = NdotH * NdotH;

            Float nom = a2;
            Float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
            denom = pi() * denom * denom;

            Return(nom / denom);
        }
        EndFunction();

        Function("GeometrySchlickGGX",
                 {
                     {"NdotV", ShaderDataType::float32()},
                     {"roughness", ShaderDataType::float32()},
                 },
                 ShaderDataType::float32());
        {
            ARGUMENT(NdotV)
            ARGUMENT(roughness)

            Float r = (roughness + 1.0f);
            Float k = (r * r) / 8.0f;

            Float denom = NdotV * (1.0f - k) + k;

            Return(NdotV / denom);
        }
        EndFunction();

        Function("GeometrySmith",
                 {
                     {"N", ShaderDataType::vec3()},
                     {"V", ShaderDataType::vec3()},
                     {"L", ShaderDataType::vec3()},
                     {"roughness", ShaderDataType::float32()}
                 },
                 ShaderDataType::float32());
        {
            ARGUMENT(N)
            ARGUMENT(V)
            ARGUMENT(L)
            ARGUMENT(roughness)

            Float NdotV = max(dot(N, V), 0.0f);
            Float NdotL = max(dot(N, L), 0.0f);
            Float ggx2 = GeometrySchlickGGX(NdotV, roughness);
            Float ggx1 = GeometrySchlickGGX(NdotL, roughness);

            Return(ggx1 * ggx2);
        }
        EndFunction();

        Function("FresnelSchlick",
                 {
                     {"cosTheta", ShaderDataType::float32()},
                     {"F0", ShaderDataType::vec3()}
                 },
                 ShaderDataType::vec3());
        {
            ARGUMENT(cosTheta)
            ARGUMENT(F0)
            Return(F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f));
        }
        EndFunction();

        Function("pbr_begin",
                 {
                     {"WorldPos", ShaderDataType::vec3()},
                     {"Normal", ShaderDataType::vec3()},
                     {"albedo", ShaderDataType::vec3()},
                     {"metallic", ShaderDataType::float32()},
                     {"roughness", ShaderDataType::float32()},
                     {"ao", ShaderDataType::float32()},
                     {"camPos", ShaderDataType::vec3()}
                 },
                 PbrPass);
        {
            ARGUMENT(WorldPos)
            ARGUMENT(Normal)
            ARGUMENT(albedo)
            ARGUMENT(metallic)
            ARGUMENT(roughness)
            ARGUMENT(ao)
            ARGUMENT(camPos)

            Object<PbrPass> ret;

            vec3 N = normalize(Normal);
            vec3 V = normalize(camPos - WorldPos);

            // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
            // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
            vec3 F0;
            F0 = vec3(0.04f, 0.04f, 0.04f);
            F0 = mix(F0, albedo, metallic);

            ret["N"] = N;
            ret["V"] = V;
            ret["F0"] = F0;

            ret["WorldPos"] = WorldPos;
            ret["Normal"] = Normal;
            ret["albedo"] = albedo;
            ret["metallic"] = metallic;
            ret["roughness"] = roughness;
            ret["ao"] = ao;
            ret["camPos"] = camPos;

            Return(ret);
        }
        EndFunction();

        Function("pbr_point", {
                     {"pass", PbrPass},
                     {"Lo", ShaderDataType::vec3()},
                     {"position", ShaderDataType::vec3()},
                     {"color", ShaderDataType::vec3()},
                     {"shadow", ShaderDataType::float32()},
                 },
                 ShaderDataType::vec3());
        {
            ARGUMENT(pass)
            ARGUMENT(Lo)
            ARGUMENT(position)
            ARGUMENT(color)
            ARGUMENT(shadow)

            vec3 N = pass["N"];
            vec3 V = pass["V"];
            vec3 F0 = pass["F0"];
            vec3 WorldPos = pass["WorldPos"];
            vec3 Normal = pass["Normal"];
            vec3 albedo = pass["albedo"];
            Float metallic = pass["metallic"];
            Float roughness = pass["roughness"];
            Float ao = pass["ao"];
            vec3 camPos = pass["camPos"];

            // calculate per-light radiance
            vec3 L = normalize(position - WorldPos);
            vec3 H = normalize(V + L);
            Float distance = length(position - WorldPos);
            Float attenuation = 1.0f / (distance * distance);
            vec3 radiance = color * attenuation;

            // Cook-Torrance BRDF
            Float NDF = DistributionGGX(N, H, roughness);
            Float G = GeometrySmith(N, V, L, roughness);
            vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

            vec3 numerator = NDF * G * F;
            Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
            // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;

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

            // scale light by NdotL
            Float NdotL = max(dot(N, L), 0.0f);

            // add to outgoing radiance Lo
            Lo += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
            // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

            Return(Lo);
        }
        EndFunction();

        Function("pbr_directional", {
                     {"pass", PbrPass},
                     {"Lo", ShaderDataType::vec3()},
                     {"direction", ShaderDataType::vec3()},
                     {"color", ShaderDataType::vec3()},
                     {"shadow", ShaderDataType::float32()},
                 },
                 ShaderDataType::vec3());
        {
            ARGUMENT(pass)
            ARGUMENT(Lo)
            ARGUMENT(direction)
            ARGUMENT(color)
            ARGUMENT(shadow)

            vec3 N = pass["N"];
            vec3 V = pass["V"];
            vec3 F0 = pass["F0"];
            vec3 WorldPos = pass["WorldPos"];
            vec3 Normal = pass["Normal"];
            vec3 albedo = pass["albedo"];
            Float metallic = pass["metallic"];
            Float roughness = pass["roughness"];
            Float ao = pass["ao"];
            vec3 camPos = pass["camPos"];

            // calculate per-light radiance

            vec3 L = normalize(direction);
            vec3 H = normalize(V + L);

            Float angle = acos(dot(normalize(direction), normalize(Normal)));

            vec3 radiance = color * cos(angle);

            // Cook-Torrance BRDF
            Float NDF = DistributionGGX(N, H, roughness);
            Float G = GeometrySmith(N, V, L, roughness);
            vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

            vec3 numerator = NDF * G * F;
            Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
            // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;

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

            // scale light by NdotL
            Float NdotL = max(dot(N, L), 0.0f);

            // add to outgoing radiance Lo
            Lo += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
            // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

            Return(Lo);
        }
        EndFunction();

        Function("pbr_spot", {
                     {"pass", PbrPass},
                     {"Lo", ShaderDataType::vec3()},
                     {"position", ShaderDataType::vec3()},
                     {"direction", ShaderDataType::vec3()},
                     {"quadratic", ShaderDataType::float32()},
                     {"color", ShaderDataType::vec3()},
                     {"cutOff", ShaderDataType::float32()},
                     {"outerCutOff", ShaderDataType::float32()},
                     {"constant", ShaderDataType::float32()},
                     {"linear", ShaderDataType::float32()},
                     {"shadow", ShaderDataType::float32()},
                 },
                 ShaderDataType::vec3());
        {
            ARGUMENT(pass)
            ARGUMENT(Lo)
            ARGUMENT(position)
            ARGUMENT(direction)
            ARGUMENT(quadratic)
            ARGUMENT(color)
            ARGUMENT(cutOff)
            ARGUMENT(outerCutOff)
            ARGUMENT(constant)
            ARGUMENT(linear)
            ARGUMENT(shadow)

            vec3 N = pass["N"];
            vec3 V = pass["V"];
            vec3 F0 = pass["F0"];
            vec3 WorldPos = pass["WorldPos"];
            vec3 Normal = pass["Normal"];
            vec3 albedo = pass["albedo"];
            Float metallic = pass["metallic"];
            Float roughness = pass["roughness"];
            Float ao = pass["ao"];
            vec3 camPos = pass["camPos"];

            vec3 lightDir = normalize(position - WorldPos);

            Float theta = dot(lightDir, normalize(direction * -1));
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

            vec3 numerator = NDF * G * F;
            Float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
            // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;

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

            // scale light by NdotL
            Float NdotL = max(dot(N, L), 0.0f);

            // add to outgoing radiance Lo
            Lo += (kD * albedo / pi() + specular) * radiance * NdotL * shadow;
            // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

            Return(Lo);
        }
        EndFunction();

        Function("pbr_finish",
                 {
                     {"pass", PbrPass},
                     {"Lo", ShaderDataType::vec3()}
                 },
                 ShaderDataType::vec3());
        {
            ARGUMENT(pass)
            ARGUMENT(Lo)

            // ambient lighting (note that the next IBL tutorial will replace
            // this ambient lighting with environment lighting).
            vec3 ambient = vec3(0.03f) * pass["albedo"] * pass["ao"];

            vec3 color = ambient + Lo;

            // HDR tonemapping
            //  color = color / (color + vec3(1.0));
            // gamma correct
            //  color = pow(color, vec3(1.0/2.2));

            Return(color);
        }
        EndFunction();
    }
}
