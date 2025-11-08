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

#include "xng/graphics/passes/constructionpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    DefineStruct(AtlasTexture,
                 ivec4, level_index_filtering_assigned,
                 vec4, atlasScale_texSize);

    DefineStruct(BufferLayout,
                 mat4, model,
                 mat4, mvp,
                 ivec4, objectID_boneOffset_shadows,
                 vec4, metallic_roughness_ambientOcclusion,
                 vec4, albedoColor,
                 vec4, normalIntensity,
                 AtlasTexture, normal,
                 AtlasTexture, metallic,
                 AtlasTexture, roughness,
                 AtlasTexture, ambientOcclusion,
                 AtlasTexture, albedo);

    DefineStruct(BoneBufferLayout,
                 mat4, matrix);

    Shader ConstructionPass::createVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, position)
        Input(vec3, normal)
        Input(vec2, uv)
        Input(vec3, tangent)
        Input(vec3, bitangent)

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

        Buffer(data, BufferLayout)
        DynamicBuffer(bones, BoneBufferLayout)

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12)

        vPos = data.mvp * vec4(position, 1);
        fPos = (data.model * vec4(position, 1)).xyz();
        fUv = uv;
        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data.model * vec4(normal, 0.0)).xyz());
        fT = normalize((data.model * vec4(tangent, 0.0)).xyz());
        fB = normalize((data.model * vec4(cross(normalize(tangent), normalize(normal).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return BuildShader();
    }

    Shader ConstructionPass::createSkinnedVertexShader() {
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

        Buffer(data, BufferLayout)
        DynamicBuffer(bones, BoneBufferLayout)

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12)

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument(ShaderDataType::Int(), "offset")},
                 ShaderDataType::vec4());
        {
            ARGUMENT(Int, offset)

            If(offset < 0)
            {
                Return(vec4(position, 1.0f));
            }
            EndIf

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1)
            {
                If(boneIds.x() + offset >= boneCount)
                {
                    Return(vec4(position, 1.0f));
                }
                Else
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                }
                EndIf
            }
            EndIf

            If(boneIds.y() > -1)
            {
                If(boneIds.y() + offset >= boneCount)
                {
                    Return(vec4(position, 1.0f));
                }
                Else
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                }
                EndIf
            }
            EndIf

            If(boneIds.z() > -1)
            {
                If(boneIds.z() + offset >= boneCount)
                {
                    Return(vec4(position, 1.0f));
                }
                Else
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                }
                EndIf
            }
            EndIf

            If(boneIds.w() > -1)
            {
                If(boneIds.w() + offset >= boneCount);
                {
                    Return(vec4(position, 1.0f));
                }
                Else
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset].matrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf
            }
            EndIf

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

    Shader ConstructionPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec3, fPos)
        Input(vec3, fNorm)
        Input(vec3, fTan)
        Input(vec2, fUv)
        Input(vec4, vPos)
        Input(vec3, fT)
        Input(vec3, fB)
        Input(vec3, fN)

        Output(vec4, oPosition)
        Output(vec4, oNormal)
        Output(vec4, oTangent)
        Output(vec4, oRoughnessMetallicAO)
        Output(vec4, oAlbedo)
        Output(ivec4, oObjectShadows)

        DeclareStruct(AtlasTexture)
        DeclareStruct(BufferLayout)
        DeclareStruct(BoneBufferLayout)

        Buffer(data, BufferLayout)
        DynamicBuffer(bones, BoneBufferLayout)

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12)

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
            {
                Return(vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }
            Else
            {
                vec2 uv = inUv * atlasScale_texSize.xy();
                If(level_index_filtering_assigned.z() == 1)
                {
                    Return(textureBicubic(atlasTextures[level_index_filtering_assigned.x()],
                                          vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                                          atlasScale_texSize.zw()));
                }
                Else
                {
                    Return(textureSampleArray(atlasTextures[level_index_filtering_assigned.x()],
                                              vec3(uv.x(), uv.y(), level_index_filtering_assigned.y())));
                }
                EndIf
            }
            EndIf
        }
        EndFunction();

        oPosition = vec4(fPos, 1);

        If(data.albedo.level_index_filtering_assigned.w() == 0)
        {
            oAlbedo = data.albedoColor;
        }
        Else
        {
            oAlbedo = texture_atlas(data.albedo, fUv);
        }
        EndIf

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        If(data.roughness.level_index_filtering_assigned.w() == 0)
        {
            oRoughnessMetallicAO.x() = data.metallic_roughness_ambientOcclusion.y();
        }
        Else
        {
            oRoughnessMetallicAO.x() = texture_atlas(data.roughness, fUv).x();
        }
        EndIf


        // Metallic
        If(data.metallic.level_index_filtering_assigned.w() == 0)
        {
            oRoughnessMetallicAO.y() = data.metallic_roughness_ambientOcclusion.x();
        }
        Else
        {
            oRoughnessMetallicAO.y() = texture_atlas(data.metallic, fUv).x();
        }
        EndIf

        // Ambient Occlusion
        If(data.ambientOcclusion.level_index_filtering_assigned.w() == 0)
        {
            oRoughnessMetallicAO.z() = data.metallic_roughness_ambientOcclusion.z();
        }
        Else
        {
            oRoughnessMetallicAO.z() = texture_atlas(data.ambientOcclusion, fUv).x();
        }
        EndIf

        mat3 normalMatrix = mat3(transpose(inverse(data.model)));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(data.normal.level_index_filtering_assigned.w() != 0)
        {
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = texture_atlas(data.normal, fUv).xyz()
                             * vec3(data.normalIntensity.x(), data.normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            oNormal = vec4(normalize(texNormal), 1);
        }
        EndIf

        oObjectShadows.x() = data.objectID_boneOffset_shadows.x();
        oObjectShadows.y() = data.objectID_boneOffset_shadows.z();
        oObjectShadows.z() = 0;
        oObjectShadows.w() = 1;

        return BuildShader();
    }
}
