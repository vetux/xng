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

#include "xng/graphics/3d/passes/forwardlightingpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "xng/graphics/shaderlib/pbr.hpp"
#include "xng/graphics/shaderlib/shadowmapping.hpp"
#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    Shader ForwardLightingPass::createVertexShader() {
        BeginShader(Shader::VERTEX);

        Input(ShaderDataType::vec3(), position);
        Input(ShaderDataType::vec3(), normal);
        Input(ShaderDataType::vec2(), uv);
        Input(ShaderDataType::vec3(), tangent);
        Input(ShaderDataType::vec3(), bitangent);
        Input(ShaderDataType::ivec4(), boneIds);
        Input(ShaderDataType::vec4(), boneWeights);

        Output(ShaderDataType::vec3(), fPos);
        Output(ShaderDataType::vec3(), fNorm);
        Output(ShaderDataType::vec3(), fTan);
        Output(ShaderDataType::vec2(), fUv);
        Output(ShaderDataType::vec4(), vPos);
        Output(ShaderDataType::vec3(), fT);
        Output(ShaderDataType::vec3(), fB);
        Output(ShaderDataType::vec3(), fN);

        Struct(AtlasTexture,
               {ShaderDataType::ivec4(), "level_index_filtering_assigned"},
               {ShaderDataType::vec4(), "atlasScale_texSize"});

        Struct(BufferLayout,
               {ShaderDataType::mat4(), "model"},
               {ShaderDataType::mat4(), "mvp"},
               {ShaderDataType::ivec4(), "objectID_boneOffset_shadows"},
               {ShaderDataType::vec4(), "metallic_roughness_ambientOcclusion"},
               {ShaderDataType::vec4(), "albedoColor"},
               {AtlasTexture, "metallic"},
               {AtlasTexture, "roughness"},
               {AtlasTexture, "ambientOcclusion"},
               {AtlasTexture, "albedo"},
               {ShaderDataType::vec4(), "viewPosition"},
               {AtlasTexture, "normal"},
               {ShaderDataType::vec4(), "normalIntensity"});

        Struct(BoneBufferLayout, {ShaderDataType::mat4(), "matrix"});

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

        DynamicBuffer(pointLights, PBRPointLight);
        DynamicBuffer(directionalLights, PBRDirectionalLight);
        DynamicBuffer(spotLights, PBRSpotLight);

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        DynamicBuffer(directionalLightShadowTransforms, Transform);
        DynamicBuffer(spotLightShadowTransforms, Transform);

        Buffer(data, BufferLayout);
        DynamicBuffer(bones, BoneBufferLayout);

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH);
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12);

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument("offset", ShaderDataType::integer())},
                 ShaderDataType::vec4());
        {
            ARGUMENT(offset)

            If(offset < 0);
            {
                Return(vec4(position, 1.0f));
            }
            EndIf();

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1);
            {
                If(boneIds.x() + offset >= boneCount);
                {
                    Return(vec4(position, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset]["matrix"] * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.y() > -1);
            {
                If(boneIds.y() + offset >= boneCount);
                {
                    Return(vec4(position, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset]["matrix"] * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.z() > -1);
            {
                If(boneIds.z() + offset >= boneCount);
                {
                    Return(vec4(position, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset]["matrix"] * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.w() > -1);
            {
                If(boneIds.w() + offset >= boneCount);
                {
                    Return(vec4(position, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset]["matrix"] * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf();
            }
            EndIf();

            Return(totalPosition);
        }
        EndFunction();

        vec4 pos = Call("getSkinnedVertexPosition", data["objectID_boneOffset_shadows"].y());

        vPos = data["mvp"] * pos;
        fPos = (data["model"] * pos).xyz();
        fUv = uv;

        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data["model"] * vec4(normalize(normal), 0.0)).xyz());
        fT = normalize((data["model"] * vec4(normalize(tangent), 0.0)).xyz());
        fB = normalize((data["model"] * vec4(cross(normalize(normal), normalize(tangent).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return BuildShader();
    }

    DEFINE_FUNCTION2(textureAtlas)

    Shader ForwardLightingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT);

        Input(ShaderDataType::vec3(), fPos);
        Input(ShaderDataType::vec3(), fNorm);
        Input(ShaderDataType::vec3(), fTan);
        Input(ShaderDataType::vec2(), fUv);
        Input(ShaderDataType::vec4(), vPos);
        Input(ShaderDataType::vec3(), fT);
        Input(ShaderDataType::vec3(), fB);
        Input(ShaderDataType::vec3(), fN);

        Output(ShaderDataType::vec4(), oColor);

        Struct(AtlasTexture,
               {ShaderDataType::ivec4(), "level_index_filtering_assigned"},
               {ShaderDataType::vec4(), "atlasScale_texSize"});

        Struct(BufferLayout,
               {ShaderDataType::mat4(), "model"},
               {ShaderDataType::mat4(), "mvp"},
               {ShaderDataType::ivec4(), "objectID_boneOffset_shadows"},
               {ShaderDataType::vec4(), "metallic_roughness_ambientOcclusion"},
               {ShaderDataType::vec4(), "albedoColor"},
               {AtlasTexture, "metallic"},
               {AtlasTexture, "roughness"},
               {AtlasTexture, "ambientOcclusion"},
               {AtlasTexture, "albedo"},
               {ShaderDataType::vec4(), "viewPosition"},
               {AtlasTexture, "normal"},
               {ShaderDataType::vec4(), "normalIntensity"});

        Struct(BoneBufferLayout, {ShaderDataType::mat4(), "matrix"});

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

        DynamicBuffer(pointLights, PBRPointLight);
        DynamicBuffer(directionalLights, PBRDirectionalLight);
        DynamicBuffer(spotLights, PBRSpotLight);

        DynamicBuffer(shadowPointLights, PBRPointLight)
        DynamicBuffer(shadowDirectionalLights, PBRDirectionalLight)
        DynamicBuffer(shadowSpotLights, PBRSpotLight)

        DynamicBuffer(directionalLightShadowTransforms, Transform);
        DynamicBuffer(spotLightShadowTransforms, Transform);

        Buffer(data, BufferLayout);
        DynamicBuffer(bones, BoneBufferLayout);

        Texture(pointLightShadowMaps, TEXTURE_CUBE_MAP_ARRAY, DEPTH);
        Texture(directionalLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);
        Texture(spotLightShadowMaps, TEXTURE_2D_ARRAY, DEPTH);

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12);

        shaderlib::pbr();
        shaderlib::shadowmapping::sampleShadowPoint();
        shaderlib::shadowmapping::sampleShadowDirectional();
        shaderlib::textureBicubic();

        Function("textureAtlas",
                 {
                     {"textureDef", AtlasTexture},
                     {"inUv", ShaderDataType::vec2()}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(textureDef)
            ARGUMENT(inUv)

            ivec4 level_index_filtering_assigned = textureDef["level_index_filtering_assigned"];
            vec4 atlasScale_texSize = textureDef["atlasScale_texSize"];

            If(level_index_filtering_assigned.w() == 0);
            {
                Return(vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }
            Else();
            {
                vec2 uv = inUv * atlasScale_texSize.xy();
                If(level_index_filtering_assigned.z() == 1);
                {
                    Return(textureBicubic(textureSampler("atlasTextures", level_index_filtering_assigned.x()),
                                          vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                                          atlasScale_texSize.zw()));
                }
                Else();
                {
                    Return(texture(textureSampler("atlasTextures", level_index_filtering_assigned.x()),
                                   vec3(uv.x(), uv.y(), level_index_filtering_assigned.y())));
                }
                EndIf();
            }
            EndIf();
        }
        EndFunction();

        vec4 albedo;
        albedo = vec4(0.0f, 0.0f, 0.0f, 0.0f);

        If(data["albedo"]["level_index_filtering_assigned"].w() == 0);
        {
            albedo = data["albedoColor"];
        }
        Else();
        {
            albedo = textureAtlas(data["albedo"], fUv);
        }
        EndIf();

        vec3 roughnessMetallicAO;
        roughnessMetallicAO = vec3(0.0f, 0.0f, 0.0f);

        // Roughness
        roughnessMetallicAO.x() = textureAtlas(data["roughness"], fUv).x()
                                  + data["metallic_roughness_ambientOcclusion"].y();

        // Metallic
        roughnessMetallicAO.y() = textureAtlas(data["metallic"], fUv).x()
                                  + data["metallic_roughness_ambientOcclusion"].x();

        // Ambient Occlusion
        roughnessMetallicAO.z() = textureAtlas(data["ambientOcclusion"],
                                               fUv).x()
                                  + data["metallic_roughness_ambientOcclusion"].z();

        mat3 normalMatrix = matrix3(transpose(inverse(data["model"])));
        vec3 normal;
        normal = normalize(normalMatrix * fNorm);
        vec3 tangent = normalize(normalMatrix * fTan);

        If(data["normal"]["level_index_filtering_assigned"].w() != 0);
        {
            mat3 tbn = matrix(fT, fB, fN);
            vec3 texNormal = textureAtlas(data["normal"], fUv).xyz()
                             * vec3(data["normalIntensity"].x(), data["normalIntensity"].x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            normal = normalize(texNormal);
        }
        EndIf();

        auto pass = New("PbrPass");
        pass = pbr_begin(fPos,
                         normal,
                         albedo.xyz(),
                         roughnessMetallicAO.y(),
                         roughnessMetallicAO.x(),
                         roughnessMetallicAO.z(),
                         data["viewPosition"].xyz());

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

        For(i, 0, directionalLights.length() - 1, 1);
        {
            auto light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light["direction"].xyz(), light["color"].xyz(), 1.0f);
        }
        EndFor();

        For(i, 0, spotLights.length() - 1, 1);
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

        For(i, 0, shadowPointLights.length() - 1, 1);
        {
            auto light = shadowPointLights[i];
            Float shadow;
            shadow = Float(1.0f);
            If(data["objectID_boneOffset_shadows"].z() == 1);
            {
                shadow = sampleShadowPoint(fPos,
                                           light["position"].xyz(),
                                           data["viewPosition"].xyz(),
                                           pointLightShadowMaps,
                                           i,
                                           light["farPlane"].x());
            }
            EndIf();
            reflectance = pbr_point(pass, reflectance, light["position"].xyz(), light["color"].xyz(), shadow);
        }
        EndFor();

        For(i, 0, shadowDirectionalLights.length() - 1, 1);
        {
            auto light = shadowDirectionalLights[i];
            vec4 fragPosLightSpace = directionalLightShadowTransforms[i]["transform"] * vec4(fPos, 1);
            Float shadow;
            shadow = Float(1.0f);
            If(data["objectID_boneOffset_shadows"].z() == 1);
            {
                shadow = sampleShadowDirectional(fragPosLightSpace,
                                                 directionalLightShadowMaps,
                                                 i,
                                                 normal,
                                                 vec3(0, 0, 0),
                                                 fPos);
            }
            EndIf();
            reflectance = pbr_directional(pass, reflectance, light["direction"].xyz(), light["color"].xyz(), shadow);
        }
        EndFor();

        For(i, 0, shadowSpotLights.length() - 1, 1);
        {
            auto light = shadowSpotLights[i];
            vec4 fragPosLightSpace = spotLightShadowTransforms[i]["transform"] * vec4(fPos, 1);
            Float shadow;
            shadow = Float(1.0f);
            If(data["objectID_boneOffset_shadows"].z() == 1);
            {
                shadow = sampleShadowDirectional(fragPosLightSpace,
                                                 spotLightShadowMaps,
                                                 i,
                                                 normal,
                                                 light["position"].xyz(),
                                                 fPos);
            }
            EndIf();
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

        oColor = vec4(pbr_finish(pass, reflectance), albedo.w());
        return BuildShader();
    }
}
