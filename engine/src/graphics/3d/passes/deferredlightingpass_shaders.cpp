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

#include "xng/graphics/3d/passes/deferredlightingpass.hpp"

#include "xng/graphics/shaderlib/pbr.hpp"
#include "xng/graphics/shaderlib/shadowmapping.hpp"

using namespace xng::ShaderScript;

namespace xng {
    Shader DeferredLightingPass::createVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(ShaderDataType::vec3(), vPosition);
        Input(ShaderDataType::vec2(), vUv);

        Output(ShaderDataType::vec4(), fPosition);
        Output(ShaderDataType::vec2(), fUv);

        Struct(ShaderData,
               {{ShaderDataType::vec4(), "viewPosition"}});

        Struct(PBRPointLight,
               {{ShaderDataType::vec4(), "position"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"}});

        Struct(PBRDirectionalLight,
               {{ShaderDataType::vec4(), "direction"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"}});

        Struct(PBRSpotLight,
               {{ShaderDataType::vec4(), "position"},
               {ShaderDataType::vec4(), "direction_quadratic"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"},
               {ShaderDataType::vec4(), "cutOff_outerCutOff_constant_linear"} });

        Struct(Transform, {{ShaderDataType::mat4(), "transform"}});

        Buffer(shaderData, ShaderData);

        DynamicBuffer(pointLights, PBRPointLight);
        DynamicBuffer(directionalLights, PBRDirectionalLight);
        DynamicBuffer(spotLights, PBRSpotLight);

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        Texture(gBufferPos, TEXTURE_2D, RGBA32F);
        Texture(gBufferNormal, TEXTURE_2D, RGBA32F);
        Texture(gBufferRoughnessMetallicAO, TEXTURE_2D, RGBA32F);
        Texture(gBufferAlbedo, TEXTURE_2D, RGBA);
        Texture(gBufferObjectShadows, TEXTURE_2D, RGBA32I);
        Texture(gBufferDepth, TEXTURE_2D, DEPTH);

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH);
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);

        DynamicBuffer(directionalLightShadowTransforms, Transform);
        DynamicBuffer(spotLightShadowTransforms, Transform);

        fPosition = vec4(vPosition, 1.0f);
        fUv = vUv;
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader DeferredLightingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(ShaderDataType::vec4(), fPosition);
        Input(ShaderDataType::vec2(), fUv);

        Output(ShaderDataType::vec4(), oColor);

        Struct(ShaderData,
               {{ShaderDataType::vec4(), "viewPosition"}});

        Struct(PBRPointLight,
               {{ShaderDataType::vec4(), "position"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"}});

        Struct(PBRDirectionalLight,
               {{ShaderDataType::vec4(), "direction"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"}});

        Struct(PBRSpotLight,
               {{ShaderDataType::vec4(), "position"},
               {ShaderDataType::vec4(), "direction_quadratic"},
               {ShaderDataType::vec4(), "color"},
               {ShaderDataType::vec4(), "farPlane"},
               {ShaderDataType::vec4(), "cutOff_outerCutOff_constant_linear"} });

        Struct(Transform, {{ShaderDataType::mat4(), "transform"}});

        Buffer(shaderData, ShaderData);

        DynamicBuffer(pointLights, PBRPointLight);
        DynamicBuffer(directionalLights, PBRDirectionalLight);
        DynamicBuffer(spotLights, PBRSpotLight);

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        Texture(gBufferPos, TEXTURE_2D, RGBA32F);
        Texture(gBufferNormal, TEXTURE_2D, RGBA32F);
        Texture(gBufferRoughnessMetallicAO, TEXTURE_2D, RGBA32F);
        Texture(gBufferAlbedo, TEXTURE_2D, RGBA);
        Texture(gBufferObjectShadows, TEXTURE_2D, RGBA32I);
        Texture(gBufferDepth, TEXTURE_2D, DEPTH);

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH);
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);

        DynamicBuffer(directionalLightShadowTransforms, Transform);
        DynamicBuffer(spotLightShadowTransforms, Transform);

        shaderlib::pbr();
        shaderlib::shadowmapping::sampleShadowPoint();
        shaderlib::shadowmapping::sampleShadowDirectional();

        Float gDepth = texture(gBufferDepth, fUv).x();
        If(gDepth == 1);
        {
            oColor = vec4(0, 0, 0, 0);
            setFragmentDepth(1);
            Return();
        }
        EndIf();

        Int receiveShadows = texture(gBufferObjectShadows, fUv).y();

        vec3 fPos = texture(gBufferPos, fUv).xyz();
        vec3 fNorm = texture(gBufferNormal, fUv).xyz();
        vec3 roughnessMetallicAO = texture(gBufferRoughnessMetallicAO, fUv).xyz();
        vec3 albedo = texture(gBufferAlbedo, fUv).xyz();

        auto pass = New("PbrPass");
        pass = pbr_begin(fPos,
                              fNorm,
                              albedo,
                              roughnessMetallicAO.y(),
                              roughnessMetallicAO.x(),
                              roughnessMetallicAO.z(),
                              shaderData["viewPosition"].xyz());

        vec3 reflectance;
        reflectance = vec3(0, 0, 0);

        Int i;
        i = Int(0);
        For(i, 0, pointLights.length() - 1, 1);
        {
            auto light = pointLights[i];
            reflectance = pbr_point(pass, reflectance, light["position"].xyz(), light["color"].xyz(), 1.0f);
        }
        EndFor();

        For(i, 0, directionalLights.length() - 1,  1);
        {
            auto light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light["direction"].xyz(), light["color"].xyz(), 1.0f);
        }
        EndFor();

        For(i, 0, spotLights.length() - 1,  1);
        {
            auto light = spotLights[i];
            reflectance = pbr_spot(pass,
                                   reflectance,
                                   light["position"].xyz(),
                                   light["direction_quadratic"].xyz(),
                                   light["direction_quadratic"].w(),
                                   light["color"].xyz(),
                                   light["cutOff_outerCutOff_constant_linear"].x(),
                                   light["cutOff_outerCutOff_constant_linear"].y(),
                                   light["cutOff_outerCutOff_constant_linear"].z(),
                                   light["cutOff_outerCutOff_constant_linear"].w(),
                                   1.0f);
        }
        EndFor();

        For(i, 0, shadowPointLights.length() - 1,  1);
        {
            auto light = shadowPointLights[i];
            Float shadow = sampleShadowPoint(fPos,
                                             light["position"].xyz(),
                                             shaderData["viewPosition"].xyz(),
                                             pointLightShadowMaps,
                                             i,
                                             light["farPlane"].x());
            reflectance = pbr_point(pass, reflectance, light["position"].xyz(), light["color"].xyz(), shadow);
        }
        EndFor();

        For(i, 0, shadowDirectionalLights.length() - 1,  1);
        {
            auto light = shadowDirectionalLights[i];
            vec4 fragPosLightSpace = directionalLightShadowTransforms[i]["transform"] * vec4(fPos, 1);
            Float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                   directionalLightShadowMaps,
                                                   i,
                                                   fNorm,
                                                   vec3(0, 0, 0),
                                                   fPos);
            reflectance = pbr_directional(pass, reflectance, light["direction"].xyz(), light["color"].xyz(), shadow);
        }
        EndFor();

        For(i, 0, shadowSpotLights.length() - 1,  1);
        {
            auto light = shadowSpotLights[i];
            vec4 fragPosLightSpace = spotLightShadowTransforms[i]["transform"] * vec4(fPos, 1);
            Float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                   spotLightShadowMaps,
                                                   i,
                                                   fNorm,
                                                   light["position"].xyz(),
                                                   fPos);
            reflectance = pbr_spot(pass,
                                   reflectance,
                                   light["position"].xyz(),
                                   light["direction_quadratic"].xyz(),
                                   light["direction_quadratic"].w(),
                                   light["color"].xyz(),
                                   light["cutOff_outerCutOff_constant_linear"].x(),
                                   light["cutOff_outerCutOff_constant_linear"].y(),
                                   light["cutOff_outerCutOff_constant_linear"].z(),
                                   light["cutOff_outerCutOff_constant_linear"].w(),
                                   shadow);
        }
        EndFor();

        oColor = vec4(pbr_finish(pass, reflectance), 1);

        return BuildShader();
    }
}
