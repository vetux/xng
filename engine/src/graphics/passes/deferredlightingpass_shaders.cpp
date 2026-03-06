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

#include "xng/graphics/passes/deferredlightingpass.hpp"

#include "xng/graphics/shaderlib/pbr.hpp"
#include "xng/graphics/shaderlib/shadowmapping.hpp"
#include "xng/rendergraph/rendergraphtexture.hpp"

using namespace xng::ShaderScript;

namespace xng
{
    namespace {
    DefineStruct(ShaderData,
                 Vec4f, viewPosition_gamma,
                 Vec4i, iblPresent_prefilterMipCount)

    DefineStruct(PBRPointLight,
                 Vec4f, position,
                 Vec4f, color,
                 Vec4f, farPlane)

    DefineStruct(PBRDirectionalLight,
                 Vec4f, direction,
                 Vec4f, color,
                 Vec4f, farPlane)

    DefineStruct(PBRSpotLight,
                 Vec4f, position,
                 Vec4f, direction_quadratic,
                 Vec4f, color,
                 Vec4f, farPlane,
                 Vec4f, cutOff_outerCutOff_constant_linear)

    DefineStruct(TransformData, Mat4f, transform)
    }

    Shader DeferredLightingPass::createVertexShader()
    {
        BeginShader(Shader::VERTEX)

        Input(vec3, vPosition)
        Input(vec2, vUv)

        Output(vec4, fPosition)
        Output(vec2, fUv)

        DeclareStruct(ShaderData)
        DeclareStruct(PBRPointLight)
        DeclareStruct(PBRDirectionalLight)
        DeclareStruct(PBRSpotLight)
        DeclareStruct(TransformData)
        DeclareStruct(PbrPass)

        Buffer(ShaderData, shaderData)

        DynamicBuffer(PBRPointLight, pointLights)
        DynamicBuffer(PBRDirectionalLight, directionalLights)
        DynamicBuffer(PBRSpotLight, spotLights)

        DynamicBuffer(PBRPointLight, shadowPointLights)
        DynamicBuffer(PBRDirectionalLight, shadowDirectionalLights)
        DynamicBuffer(PBRSpotLight, shadowSpotLights)

        Texture(TEXTURE_2D, RGBA32F, gBufferPos)
        Texture(TEXTURE_2D, RGBA32F, gBufferNormal)
        Texture(TEXTURE_2D, RGBA32F, gBufferRoughnessMetallicAO)
        Texture(TEXTURE_2D, RGBA, gBufferAlbedo)
        Texture(TEXTURE_2D, RGBA32I, gBufferObjectShadows)
        Texture(TEXTURE_2D, DEPTH, gBufferDepth)

        Texture(TEXTURE_CUBE_MAP_ARRAY, DEPTH, pointLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, directionalLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, spotLightShadowMaps)

        DynamicBuffer(TransformData, directionalLightShadowTransforms)
        DynamicBuffer(TransformData, spotLightShadowTransforms)

        fPosition = vec4(vPosition, 1.0f);
        fUv = vUv;
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader DeferredLightingPass::createFragmentShader()
    {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fPosition)
        Input(vec2, fUv)

        Output(vec4, oColor)

        DeclareStruct(ShaderData)
        DeclareStruct(PBRPointLight)
        DeclareStruct(PBRDirectionalLight)
        DeclareStruct(PBRSpotLight)
        DeclareStruct(TransformData)
        DeclareStruct(PbrPass)

        Buffer(ShaderData, shaderData)

        DynamicBuffer(PBRPointLight, pointLights)
        DynamicBuffer(PBRDirectionalLight, directionalLights)
        DynamicBuffer(PBRSpotLight, spotLights)

        DynamicBuffer(PBRPointLight, shadowPointLights)
        DynamicBuffer(PBRDirectionalLight, shadowDirectionalLights)
        DynamicBuffer(PBRSpotLight, shadowSpotLights)

        Texture(TEXTURE_2D, RGBA32F, gBufferPos)
        Texture(TEXTURE_2D, RGBA32F, gBufferNormal)
        Texture(TEXTURE_2D, RGBA32F, gBufferRoughnessMetallicAO)
        Texture(TEXTURE_2D, RGBA, gBufferAlbedo)
        Texture(TEXTURE_2D, RGBA32I, gBufferObjectShadows)
        Texture(TEXTURE_2D, DEPTH, gBufferDepth)

        Texture(TEXTURE_CUBE_MAP_ARRAY, DEPTH, pointLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, directionalLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, spotLightShadowMaps)

        Texture(TEXTURE_CUBE_MAP, RGBA16F, iblPrefilter)
        Texture(TEXTURE_CUBE_MAP, RGBA16F, iblIrradiance)
        Texture(TEXTURE_2D, RG16F, iblBRDF)

        DynamicBuffer(TransformData, directionalLightShadowTransforms)
        DynamicBuffer(TransformData, spotLightShadowTransforms)

        shaderlib::pbr();
        shaderlib::shadowmapping::sampleShadowPoint();
        shaderlib::shadowmapping::sampleShadowDirectional();

        Float gDepth = textureSample(gBufferDepth, fUv).x();
        If(gDepth == 1)
            oColor = vec4(0, 0, 0, 0);
            setFragmentDepth(1);
            Return();
        Fi

        Int receiveShadows = textureSample(gBufferObjectShadows, fUv).y();

        vec3 fPos = textureSample(gBufferPos, fUv).xyz();
        vec3 fNorm = textureSample(gBufferNormal, fUv).xyz();
        vec3 roughnessMetallicAO = textureSample(gBufferRoughnessMetallicAO, fUv).xyz();
        vec3 albedo = textureSample(gBufferAlbedo, fUv).xyz();

        PbrPass pass;
        pass = pbr_begin(fPos,
                         fNorm,
                         albedo,
                         roughnessMetallicAO.y(),
                         roughnessMetallicAO.x(),
                         roughnessMetallicAO.z(),
                         shaderData.viewPosition_gamma.xyz(),
                         shaderData.viewPosition_gamma.w());

        vec3 reflectance;
        reflectance = vec3(0, 0, 0);

        For(Int, i, 0, i < pointLights.length(), i + 1)
            auto light = pointLights[i];
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), 1.0f);
        Done

        For(Int, i, 0, i < directionalLights.length(), i + 1)
            auto light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), 1.0f);
        Done

        For(Int, i, 0, i < spotLights.length(), i + 1)
            auto light = spotLights[i];
            reflectance = pbr_spot(pass,
                                   reflectance,
                                   light.position.xyz(),
                                   light.direction_quadratic.xyz(),
                                   light.direction_quadratic.w(),
                                   light.color.xyz(),
                                   light.cutOff_outerCutOff_constant_linear.x(),
                                   light.cutOff_outerCutOff_constant_linear.y(),
                                   light.cutOff_outerCutOff_constant_linear.z(),
                                   light.cutOff_outerCutOff_constant_linear.w(),
                                   1.0f);
        Done

        For(Int, i, 0, i < shadowPointLights.length(), i + 1)
            auto light = shadowPointLights[i];
            Float shadow = sampleShadowPoint(fPos,
                                             light.position.xyz(),
                                             shaderData.viewPosition_gamma.xyz(),
                                             pointLightShadowMaps,
                                             i,
                                             light.farPlane.x());
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), shadow);
        Done

        For(Int, i, 0, i < shadowDirectionalLights.length(), i + 1)
            auto light = shadowDirectionalLights[i];
            vec4 fragPosLightSpace = directionalLightShadowTransforms[i].transform * vec4(fPos, 1);
            Float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                   directionalLightShadowMaps,
                                                   i,
                                                   fNorm,
                                                   vec3(0, 0, 0),
                                                   fPos);
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), shadow);
        Done

        For(Int, i, 0, i < shadowSpotLights.length(), i + 1)
            auto light = shadowSpotLights[i];
            vec4 fragPosLightSpace = spotLightShadowTransforms[i].transform * vec4(fPos, 1);
            Float shadow = sampleShadowDirectional(fragPosLightSpace,
                                                   spotLightShadowMaps,
                                                   i,
                                                   fNorm,
                                                   light.position.xyz(),
                                                   fPos);
            reflectance = pbr_spot(pass,
                                   reflectance,
                                   light.position.xyz(),
                                   light.direction_quadratic.xyz(),
                                   light.direction_quadratic.w(),
                                   light.color.xyz(),
                                   light.cutOff_outerCutOff_constant_linear.x(),
                                   light.cutOff_outerCutOff_constant_linear.y(),
                                   light.cutOff_outerCutOff_constant_linear.z(),
                                   light.cutOff_outerCutOff_constant_linear.w(),
                                   shadow);
        Done

        If(shaderData.iblPresent_prefilterMipCount.x() == 0)
            pass.iblIrradiance = vec3(0);
            pass.iblPrefilter = vec3(0);
            pass.iblBRDF = vec2(0);
        Else
            // Sample IBL textures and store results in the PbrPass for pbr_finish
            vec3 irradiance = textureSampleCube(iblIrradiance, pass.N).xyz();
            vec3 R = reflect(pass.V * -1, pass.N);
            Float maxMip = Float(shaderData.iblPresent_prefilterMipCount.y() - 1);
            vec3 prefilteredColor = textureSampleCube(iblPrefilter, R, pass.roughness * maxMip).xyz();
            vec2 brdf = textureSample(iblBRDF, vec2(max(dot(pass.N, pass.V), 0.0f), pass.roughness)).xy();
            pass.iblIrradiance = irradiance;
            pass.iblPrefilter = prefilteredColor;
            pass.iblBRDF = brdf;
        Fi

        oColor = vec4(pbr_finish(pass, reflectance), 1);

        return BuildShader();
    }
}
