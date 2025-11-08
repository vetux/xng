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

using namespace xng::ShaderScript;

namespace xng {
    DefineStruct(ShaderData,
                 vec4, viewPosition_gamma)

    DefineStruct(PBRPointLight,
                 vec4, position,
                 vec4, color,
                 vec4, farPlane)

    DefineStruct(PBRDirectionalLight,
                 vec4, direction,
                 vec4, color,
                 vec4, farPlane)

    DefineStruct(PBRSpotLight,
                 vec4, position,
                 vec4, direction_quadratic,
                 vec4, color,
                 vec4, farPlane,
                 vec4, cutOff_outerCutOff_constant_linear)

    DefineStruct(TransformData, mat4, transform);

    Shader DeferredLightingPass::createVertexShader() {
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

        Buffer(shaderData, ShaderData)

        DynamicBuffer(pointLights, PBRPointLight)
        DynamicBuffer(directionalLights, PBRDirectionalLight)
        DynamicBuffer(spotLights, PBRSpotLight)

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        Texture(gBufferPos, TEXTURE_2D, RGBA32F)
        Texture(gBufferNormal, TEXTURE_2D, RGBA32F)
        Texture(gBufferRoughnessMetallicAO, TEXTURE_2D, RGBA32F)
        Texture(gBufferAlbedo, TEXTURE_2D, RGBA)
        Texture(gBufferObjectShadows, TEXTURE_2D, RGBA32I)
        Texture(gBufferDepth, TEXTURE_2D, DEPTH)

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH)
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH)
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH)

        DynamicBuffer(directionalLightShadowTransforms, TransformData)
        DynamicBuffer(spotLightShadowTransforms, TransformData)

        fPosition = vec4(vPosition, 1.0f);
        fUv = vUv;
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader DeferredLightingPass::createFragmentShader() {
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

        Buffer(shaderData, ShaderData)

        DynamicBuffer(pointLights, PBRPointLight)
        DynamicBuffer(directionalLights, PBRDirectionalLight)
        DynamicBuffer(spotLights, PBRSpotLight)

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        Texture(gBufferPos, TEXTURE_2D, RGBA32F)
        Texture(gBufferNormal, TEXTURE_2D, RGBA32F)
        Texture(gBufferRoughnessMetallicAO, TEXTURE_2D, RGBA32F)
        Texture(gBufferAlbedo, TEXTURE_2D, RGBA)
        Texture(gBufferObjectShadows, TEXTURE_2D, RGBA32I)
        Texture(gBufferDepth, TEXTURE_2D, DEPTH)

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH)
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH)
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH)

        DynamicBuffer(directionalLightShadowTransforms, TransformData)
        DynamicBuffer(spotLightShadowTransforms, TransformData)

        shaderlib::pbr();
        shaderlib::shadowmapping::sampleShadowPoint();
        shaderlib::shadowmapping::sampleShadowDirectional();

        Float gDepth = textureSample(gBufferDepth, fUv).x();
        If(gDepth == 1)
        {
            oColor = vec4(0, 0, 0, 0);
            setFragmentDepth(1);
            Return();
        }
        EndIf

        Int receiveShadows = textureSample(gBufferObjectShadows, fUv).y();

        vec3 fPos = textureSample(gBufferPos, fUv).xyz();
        vec3 fNorm = textureSample(gBufferNormal, fUv).xyz();
        vec3 roughnessMetallicAO = textureSample(gBufferRoughnessMetallicAO, fUv).xyz();
        vec3 albedo = textureSample(gBufferAlbedo, fUv).xyz();

        PbrPass pass = pbr_begin(fPos,
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
        EndFor

        For(Int, i, 0, i < directionalLights.length(), i + 1)
            auto light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), 1.0f);
        EndFor

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
        EndFor

        For(Int, i, 0, i < shadowPointLights.length(), i + 1)
            auto light = shadowPointLights[i];
            Float shadow = sampleShadowPoint(fPos,
                                             light.position.xyz(),
                                             shaderData.viewPosition_gamma.xyz(),
                                             pointLightShadowMaps,
                                             i,
                                             light.farPlane.x());
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), shadow);
        EndFor

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
        EndFor

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
        EndFor

        oColor = vec4(pbr_finish(pass, reflectance), 1);

        return BuildShader();
    }
}
