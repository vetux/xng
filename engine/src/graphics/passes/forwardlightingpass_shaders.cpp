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

#include "xng/graphics/passes/forwardlightingpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "xng/graphics/shaderlib/pbr.hpp"
#include "xng/graphics/shaderlib/shadowmapping.hpp"
#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    DefineStruct(AtlasTexture,
                 ivec4, level_index_filtering_assigned,
                 vec4, atlasScale_texSize)

    DefineStruct(BufferLayout,
                 mat4, model,
                 mat4, mvp,
                 ivec4, objectID_boneOffset_shadows,
                 vec4, metallic_roughness_ambientOcclusion,
                 vec4, albedoColor,
                 AtlasTexture, metallic,
                 AtlasTexture, roughness,
                 AtlasTexture, ambientOcclusion,
                 AtlasTexture, albedo,
                 vec4, viewPosition_gamma,
                 AtlasTexture, normal,
                 vec4, normalIntensity)

    DefineStruct(BoneBufferLayout, mat4, matrix)

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

    DefineStruct(TransformData, mat4, transform)

    Shader ForwardLightingPass::createVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, position)
        Input(vec3, normal)
        Input(vec2, uv)
        Input(vec3, tangent)
        Input(vec3, bitangent)
        Input(ivec4, boneIds)
        Input(vec4, boneWeights)

        Output(vec3, fPos)
        Output(vec3, fNorm)
        Output(vec3, fTan)
        Output(vec2, fUv)
        Output(vec4, vPos)
        Output(vec3, fT)
        Output(vec3, fB)
        Output(vec3, fN)

        DeclareStruct(AtlasTexture)
        DeclareStruct(BufferLayout)
        DeclareStruct(BoneBufferLayout)
        DeclareStruct(PBRPointLight)
        DeclareStruct(PBRDirectionalLight)
        DeclareStruct(PBRSpotLight)
        DeclareStruct(TransformData)
        DeclareStruct(PbrPass);

        DynamicBuffer(PBRPointLight, pointLights)
        DynamicBuffer(PBRDirectionalLight, directionalLights)
        DynamicBuffer(PBRSpotLight, spotLights)

        DynamicBuffer(PBRPointLight, shadowPointLights)
        DynamicBuffer(PBRDirectionalLight, shadowDirectionalLights)
        DynamicBuffer(PBRSpotLight, shadowSpotLights)

        DynamicBuffer(TransformData, directionalLightShadowTransforms)
        DynamicBuffer(TransformData, spotLightShadowTransforms)

        Buffer(BufferLayout, data)
        DynamicBuffer(BoneBufferLayout, bones)

        Texture(TEXTURE_CUBE_MAP_ARRAY, DEPTH, pointLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, directionalLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, spotLightShadowMaps)

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument(ShaderDataType::Int(), "offset")},
                 ShaderDataType::vec4());
        {
            ARGUMENT(Int, offset)

            If(offset < 0)
                Return(vec4(position, 1.0f));
            Fi

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1)
                If(boneIds.x() + offset >= boneCount)
                    Return(vec4(position, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                Fi
            Fi

            If(boneIds.y() > -1)
                If(boneIds.y() + offset >= boneCount)
                    Return(vec4(position, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                Fi
            Fi

            If(boneIds.z() > -1)
                If(boneIds.z() + offset >= boneCount)
                    Return(vec4(position, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                Fi
            Fi

            If(boneIds.w() > -1)
                If(boneIds.w() + offset >= boneCount)
                    Return(vec4(position, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                Fi
            Fi

            Return(totalPosition);
        }
        EndFunction();

        vec4 pos = Call("getSkinnedVertexPosition", data.objectID_boneOffset_shadows.y());

        vPos = data.mvp * pos;
        fPos = (data.model * pos).xyz();
        fUv = uv;

        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data.model * vec4(normalize(normal), 0.0)).xyz());
        fT = normalize((data.model * vec4(normalize(tangent), 0.0)).xyz());
        fB = normalize((data.model * vec4(cross(normalize(tangent), normalize(normal).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return BuildShader();
    }

    DEFINE_FUNCTION2(texture_atlas)

    Shader ForwardLightingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec3, fPos)
        Input(vec3, fNorm)
        Input(vec3, fTan)
        Input(vec2, fUv)
        Input(vec4, vPos)
        Input(vec3, fT)
        Input(vec3, fB)
        Input(vec3, fN)

        Output(vec4, oColor)

        DeclareStruct(AtlasTexture)
        DeclareStruct(BufferLayout)
        DeclareStruct(BoneBufferLayout)
        DeclareStruct(PBRPointLight)
        DeclareStruct(PBRDirectionalLight)
        DeclareStruct(PBRSpotLight)
        DeclareStruct(TransformData)
        DeclareStruct(PbrPass);

        DynamicBuffer(PBRPointLight, pointLights)
        DynamicBuffer(PBRDirectionalLight, directionalLights)
        DynamicBuffer(PBRSpotLight, spotLights)

        DynamicBuffer(PBRPointLight, shadowPointLights)
        DynamicBuffer(PBRDirectionalLight, shadowDirectionalLights)
        DynamicBuffer(PBRSpotLight, shadowSpotLights)

        DynamicBuffer(TransformData, directionalLightShadowTransforms)
        DynamicBuffer(TransformData, spotLightShadowTransforms)

        Buffer(BufferLayout, data)
        DynamicBuffer(BoneBufferLayout, bones)

        Texture(TEXTURE_CUBE_MAP_ARRAY, DEPTH, pointLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, directionalLightShadowMaps)
        Texture(TEXTURE_2D_ARRAY, DEPTH, spotLightShadowMaps)

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        shaderlib::pbr();
        shaderlib::shadowmapping::sampleShadowPoint();
        shaderlib::shadowmapping::sampleShadowDirectional();
        shaderlib::textureBicubic();

        Function("texture_atlas",
                 {
                     {AtlasTexture::getShaderStruct().typeName, "textureDef"},
                     {ShaderDataType::vec2(), "inUv"}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(AtlasTexture, textureDef)
            ARGUMENT(vec2, inUv)

            ivec4 level_index_filtering_assigned = textureDef.level_index_filtering_assigned;
            vec4 atlasScale_texSize = textureDef.atlasScale_texSize;

            If(level_index_filtering_assigned.w() == 0)
                Return(vec4(0.0f, 0.0f, 0.0f, 0.0f));
            Else
                vec2 uv = inUv * atlasScale_texSize.xy();
                If(level_index_filtering_assigned.z() == 1)
                    Return(textureBicubic(atlasTextures[level_index_filtering_assigned.x()],
                                          vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                                          atlasScale_texSize.zw()));
                Else
                    Return(textureSampleArray(atlasTextures[level_index_filtering_assigned.x()],
                                              vec3(uv.x(), uv.y(), level_index_filtering_assigned.y())));
                Fi
            Fi
        }
        EndFunction();

        vec4 albedo;
        albedo = vec4(0.0f, 0.0f, 0.0f, 0.0f);

        If(data.albedo.level_index_filtering_assigned.w() == 0)
            albedo = data.albedoColor;
        Else
            albedo = texture_atlas(data.albedo, fUv);
        Fi

        vec3 roughnessMetallicAO;
        roughnessMetallicAO = vec3(0.0f, 0.0f, 0.0f);

        // Roughness
        If(data.roughness.level_index_filtering_assigned.w() == 0)
            roughnessMetallicAO.x() = data.metallic_roughness_ambientOcclusion.y();
        Else
            roughnessMetallicAO.x() = texture_atlas(data.roughness, fUv).x();
        Fi


        // Metallic
        If(data.metallic.level_index_filtering_assigned.w() == 0)
            roughnessMetallicAO.y() = data.metallic_roughness_ambientOcclusion.x();
        Else
            roughnessMetallicAO.y() = texture_atlas(data.metallic, fUv).x();
        Fi

        // Ambient Occlusion
        If(data.ambientOcclusion.level_index_filtering_assigned.w() == 0)
            roughnessMetallicAO.z() = data.metallic_roughness_ambientOcclusion.z();
        Else
            roughnessMetallicAO.z() = texture_atlas(data.ambientOcclusion, fUv).x();
        Fi

        mat3 normalMatrix = mat3(transpose(inverse(data.model)));
        vec3 normal;
        normal = normalize(normalMatrix * fNorm);
        vec3 tangent = normalize(normalMatrix * fTan);

        If(data.normal.level_index_filtering_assigned.w() != 0)
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = texture_atlas(data.normal, fUv).xyz()
                             * vec3(data.normalIntensity.x(), data.normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            normal = normalize(texNormal);
        Fi

        PbrPass pass = pbr_begin(fPos,
                                 normal,
                                 albedo.xyz(),
                                 roughnessMetallicAO.y(),
                                 roughnessMetallicAO.x(),
                                 roughnessMetallicAO.z(),
                                 data.viewPosition_gamma.xyz(),
                                 data.viewPosition_gamma.w());

        vec3 reflectance;
        reflectance = vec3(0, 0, 0);

        For(Int, i, 0, i < pointLights.length(), i + 1)
            PBRPointLight light = pointLights[i];
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), 1.0f);
        Done

        For(Int, i, 0, i < directionalLights.length(), i + 1)
            PBRDirectionalLight light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), 1.0f);
        Done

        For(Int, i, 0, i < spotLights.length(), i + 1)
            PBRSpotLight light = spotLights[i];
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
            PBRPointLight light = shadowPointLights[i];
            Float shadow;
            shadow = Float(1.0f);
            If(data.objectID_boneOffset_shadows.z() == 1)
                shadow = sampleShadowPoint(fPos,
                                           light.position.xyz(),
                                           data.viewPosition_gamma.xyz(),
                                           pointLightShadowMaps,
                                           i,
                                           light.farPlane.x());
            Fi
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), shadow);
        Done

        For(Int, i, 0, i < shadowDirectionalLights.length(), i + 1)
            PBRDirectionalLight light = shadowDirectionalLights[i];
            vec4 fragPosLightSpace = directionalLightShadowTransforms[i].transform * vec4(fPos, 1);
            Float shadow;
            shadow = Float(1.0f);
            If(data.objectID_boneOffset_shadows.z() == 1)
                shadow = sampleShadowDirectional(fragPosLightSpace,
                                                 directionalLightShadowMaps,
                                                 i,
                                                 normal,
                                                 vec3(0, 0, 0),
                                                 fPos);
            Fi
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), shadow);
        Done

        For(Int, i, 0, i < shadowSpotLights.length(), i + 1)
            PBRSpotLight light = shadowSpotLights[i];
            vec4 fragPosLightSpace = spotLightShadowTransforms[i].transform * vec4(fPos, 1);
            Float shadow;
            shadow = Float(1.0f);
            If(data.objectID_boneOffset_shadows.z() == 1)
                shadow = sampleShadowDirectional(fragPosLightSpace,
                                                 spotLightShadowMaps,
                                                 i,
                                                 normal,
                                                 light.position.xyz(),
                                                 fPos);
            Fi
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

        oColor = vec4(pbr_finish(pass, reflectance), albedo.w());

        return BuildShader();
    }
}
