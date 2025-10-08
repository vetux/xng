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

#include "xng/graphics/3d/passes/constructionpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    Shader ConstructionPass::createVertexShader() {
        BeginShader(Shader::VERTEX);

        Input(ShaderDataType::vec3(), position);
        Input(ShaderDataType::vec3(), normal);
        Input(ShaderDataType::vec2(), uv);
        Input(ShaderDataType::vec3(), tangent);
        Input(ShaderDataType::vec3(), bitangent);

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
               {ShaderDataType::vec4(), "normalIntensity"},
               {AtlasTexture, "normal"},
               {AtlasTexture, "metallic"},
               {AtlasTexture, "roughness"},
               {AtlasTexture, "ambientOcclusion"},
               {AtlasTexture, "albedo"});

        Struct(BoneBufferLayout, {ShaderDataType::mat4(), "matrix"});

        Buffer(data, BufferLayout);
        DynamicBuffer(bones, BoneBufferLayout);

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12);

        vPos = data["mvp"] * vec4(position, 1);
        fPos = (data["model"] * vec4(position, 1)).xyz();
        fUv = uv;
        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data["model"] * vec4(normal, 0.0)).xyz());
        fT = normalize((data["model"] * vec4(tangent, 0.0)).xyz());
        fB = normalize((data["model"] * vec4(cross(normalize(normal), normalize(tangent).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return BuildShader();
    }

    Shader ConstructionPass::createSkinnedVertexShader() {
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
               {ShaderDataType::vec4(), "normalIntensity"},
               {AtlasTexture, "normal"},
               {AtlasTexture, "metallic"},
               {AtlasTexture, "roughness"},
               {AtlasTexture, "ambientOcclusion"},
               {AtlasTexture, "albedo"});

        Struct(BoneBufferLayout, {ShaderDataType::mat4(), "matrix"});

        Buffer(data, BufferLayout);
        DynamicBuffer(bones, BoneBufferLayout);

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

    Shader ConstructionPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT);

        Input(ShaderDataType::vec3(), fPos);
        Input(ShaderDataType::vec3(), fNorm);
        Input(ShaderDataType::vec3(), fTan);
        Input(ShaderDataType::vec2(), fUv);
        Input(ShaderDataType::vec4(), vPos);
        Input(ShaderDataType::vec3(), fT);
        Input(ShaderDataType::vec3(), fB);
        Input(ShaderDataType::vec3(), fN);

        Output(ShaderDataType::vec4(), oPosition);
        Output(ShaderDataType::vec4(), oNormal);
        Output(ShaderDataType::vec4(), oTangent);
        Output(ShaderDataType::vec4(), oRoughnessMetallicAO);
        Output(ShaderDataType::vec4(), oAlbedo);
        Output(ShaderDataType::ivec4(), oObjectShadows);

        Struct(AtlasTexture,
               {ShaderDataType::ivec4(), "level_index_filtering_assigned"},
               {ShaderDataType::vec4(), "atlasScale_texSize"});

        Struct(BufferLayout,
               {ShaderDataType::mat4(), "model"},
               {ShaderDataType::mat4(), "mvp"},
               {ShaderDataType::ivec4(), "objectID_boneOffset_shadows"},
               {ShaderDataType::vec4(), "metallic_roughness_ambientOcclusion"},
               {ShaderDataType::vec4(), "albedoColor"},
               {ShaderDataType::vec4(), "normalIntensity"},
               {AtlasTexture, "normal"},
               {AtlasTexture, "metallic"},
               {AtlasTexture, "roughness"},
               {AtlasTexture, "ambientOcclusion"},
               {AtlasTexture, "albedo"});

        Struct(BoneBufferLayout, {ShaderDataType::mat4(), "matrix"});

        Buffer(data, BufferLayout);
        DynamicBuffer(bones, BoneBufferLayout);

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12);

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
                    Return(textureBicubic(atlasTextures[level_index_filtering_assigned.x()],
                                          vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                                          atlasScale_texSize.zw()));
                }
                Else();
                {
                    Return(texture(atlasTextures[level_index_filtering_assigned.x()],
                                   vec3(uv.x(), uv.y(), level_index_filtering_assigned.y())));
                }
                EndIf();
            }
            EndIf();
        }
        EndFunction();

        oPosition = vec4(fPos, 1);

        If(data["albedo"]["level_index_filtering_assigned"].w() == 0);
        {
            oAlbedo = data["albedoColor"];
        }
        Else();
        {
            oAlbedo = textureAtlas(data["albedo"], fUv);
        }
        EndIf();

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        oRoughnessMetallicAO.x() = textureAtlas(data["roughness"], fUv).x()
                                   + data["metallic_roughness_ambientOcclusion"].y();

        // Metallic
        oRoughnessMetallicAO.y() = textureAtlas(data["metallic"], fUv).x()
                                   + data["metallic_roughness_ambientOcclusion"].x();

        // Ambient Occlusion
        oRoughnessMetallicAO.z() = textureAtlas(data["ambientOcclusion"],
                                                fUv).x()
                                   + data["metallic_roughness_ambientOcclusion"].z();

        mat3 normalMatrix = mat3(transpose(inverse(data["model"])));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(data["normal"]["level_index_filtering_assigned"].w() != 0);
        {
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = textureAtlas(data["normal"], fUv).xyz()
                             * vec3(data["normalIntensity"].x(), data["normalIntensity"].x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            oNormal = vec4(normalize(texNormal), 1);
        }
        EndIf();

        oObjectShadows.x() = data["objectID_boneOffset_shadows"].x();
        oObjectShadows.y() = data["objectID_boneOffset_shadows"].z();
        oObjectShadows.z() = 0;
        oObjectShadows.w() = 1;

        return BuildShader();
    }
}
