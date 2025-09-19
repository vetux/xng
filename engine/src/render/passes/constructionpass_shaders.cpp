/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/render/passes/constructionpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/render/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    ShaderAttributeLayout getVertexOutputLayout() {
        return ShaderAttributeLayout{
            {
                {"fPos", ShaderDataType::vec3()},
                {"fNorm", ShaderDataType::vec3()},
                {"fTan", ShaderDataType::vec3()},
                {"fUv", ShaderDataType::vec2()},
                {"vPos", ShaderDataType::vec4()},
                {"fT", ShaderDataType::vec3()},
                {"fB", ShaderDataType::vec3()},
                {"fN", ShaderDataType::vec3()}
            }
        };
    }

    std::unordered_map<std::string, ShaderBuffer> getBufferLayout() {
        ShaderBuffer buffer = {
            false,
            false,
            {
                {
                    {"model", ShaderDataType::mat4()},
                    {"mvp", ShaderDataType::mat4()},
                    {"objectID_boneOffset_shadows", ShaderDataType::ivec4()},
                    {"metallic_roughness_ambientOcclusion", ShaderDataType::vec4()},
                    {"albedoColor", ShaderDataType::vec4()},
                    {"normalIntensity", ShaderDataType::vec4()},
                    {"normal_level_index_filtering_assigned", ShaderDataType::ivec4()},
                    {"normal_atlasScale_texSize", ShaderDataType::vec4()},
                    {"metallic_level_index_filtering_assigned", ShaderDataType::ivec4()},
                    {"metallic_atlasScale_texSize", ShaderDataType::vec4()},
                    {"roughness_level_index_filtering_assigned", ShaderDataType::ivec4()},
                    {"roughness_atlasScale_texSize", ShaderDataType::vec4()},
                    {"ambientOcclusion_level_index_filtering_assigned", ShaderDataType::ivec4()},
                    {"ambientOcclusion_atlasScale_texSize", ShaderDataType::vec4()},
                    {"albedo_level_index_filtering_assigned", ShaderDataType::ivec4()},
                    {"albedo_atlasScale_texSize", ShaderDataType::vec4()},
                }
            }
        };
        ShaderBuffer boneBuffer = {
            false,
            true,
            {
                {
                    "matrix", ShaderDataType::mat4()
                }
            }
        };
        return {
            {"data", buffer},
            {"bones", boneBuffer}
        };
    }

    std::unordered_map<std::string, ShaderTextureArray> getTextureLayout() {
        ShaderTexture arrayTexture;
        arrayTexture.format = RGBA;
        arrayTexture.isArrayTexture = true;
        return {{"atlasTextures", ShaderTextureArray(arrayTexture, 12)}};
    }

    Shader ConstructionPass::createVertexShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::VERTEX,
                      Mesh::getDefaultVertexLayout(),
                      getVertexOutputLayout(),
                      {},
                      getBufferLayout(),
                      getTextureLayout(),
                      {});

        INPUT_ATTRIBUTE(position)
        INPUT_ATTRIBUTE(normal)
        INPUT_ATTRIBUTE(uv)
        INPUT_ATTRIBUTE(tangent)

        OUTPUT_ATTRIBUTE(vPos)
        OUTPUT_ATTRIBUTE(fPos)
        OUTPUT_ATTRIBUTE(fUv)
        OUTPUT_ATTRIBUTE(fNorm)
        OUTPUT_ATTRIBUTE(fTan)

        OUTPUT_ATTRIBUTE(fT)
        OUTPUT_ATTRIBUTE(fB)
        OUTPUT_ATTRIBUTE(fN)

        BUFFER_ELEMENT(data, mvp)
        BUFFER_ELEMENT(data, model)

        vPos = data_mvp * vec4(position, 1);
        fPos = (data_model * vec4(position, 1)).xyz();
        fUv = uv;
        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data_model * vec4(normal, 0.0)).xyz());
        fT = normalize((data_model * vec4(tangent, 0.0)).xyz());
        fB = normalize((data_model * vec4(cross(normalize(normal), normalize(tangent).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return builder.build();
    }

    Shader ConstructionPass::createSkinnedVertexShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::VERTEX,
                      SkinnedMesh::getDefaultVertexLayout(),
                      getVertexOutputLayout(),
                      {},
                      getBufferLayout(),
                      getTextureLayout(),
                      {});

        INPUT_ATTRIBUTE(position)
        INPUT_ATTRIBUTE(normal)
        INPUT_ATTRIBUTE(uv)
        INPUT_ATTRIBUTE(tangent)
        INPUT_ATTRIBUTE(boneIds)
        INPUT_ATTRIBUTE(boneWeights)

        OUTPUT_ATTRIBUTE(vPos)
        OUTPUT_ATTRIBUTE(fPos)
        OUTPUT_ATTRIBUTE(fUv)
        OUTPUT_ATTRIBUTE(fNorm)
        OUTPUT_ATTRIBUTE(fTan)

        OUTPUT_ATTRIBUTE(fT)
        OUTPUT_ATTRIBUTE(fB)
        OUTPUT_ATTRIBUTE(fN)

        BUFFER_ELEMENT(data, mvp)
        BUFFER_ELEMENT(data, model)
        BUFFER_ELEMENT(data, objectID_boneOffset_shadows)

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

            Int boneCount;
            boneCount = getDynamicBufferLength("bones");

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
                    mat4 boneMatrix;
                    boneMatrix = dynamicBufferElement("bones",
                                                      "matrix",
                                                      boneIds.x() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(position, 1.0f);
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
                    mat4 boneMatrix;
                    boneMatrix = dynamicBufferElement("bones",
                                                      "matrix",
                                                      boneIds.y() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(position, 1.0f);
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
                    mat4 boneMatrix;
                    boneMatrix = dynamicBufferElement("bones",
                                                      "matrix",
                                                      boneIds.z() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(position, 1.0f);
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
                    mat4 boneMatrix;
                    boneMatrix = dynamicBufferElement("bones",
                                                      "matrix",
                                                      boneIds.w() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(position, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf();
            }
            EndIf();

            Return(totalPosition);
        }
        EndFunction();

        vec4 pos = Call("getSkinnedVertexPosition", data_objectID_boneOffset_shadows.y());

        vPos = data_mvp * pos;
        fPos = (data_model * pos).xyz();
        fUv = uv;

        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((data_model * vec4(normal, 0.0)).xyz());
        fT = normalize((data_model * vec4(tangent, 0.0)).xyz());
        fB = normalize((data_model * vec4(cross(normalize(normal), normalize(tangent).xyz()) * 1, 0.0)).xyz());

        setVertexPosition(vPos);

        return builder.build();
    }

    DEFINE_FUNCTION3(textureAtlas)

    Shader ConstructionPass::createFragmentShader() {
        const auto outputLayout = ShaderAttributeLayout{
            {
                {"oPosition", ShaderDataType::vec4()},
                {"oNormal", ShaderDataType::vec4()},
                {"oTangent", ShaderDataType::vec4()},
                {"oRoughnessMetallicAO", ShaderDataType::vec4()},
                {"oAlbedo", ShaderDataType::vec4()},
                {"oObjectShadows", ShaderDataType::ivec4()}
            }
        };

        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::FRAGMENT,
                      getVertexOutputLayout(),
                      outputLayout,
                      {},
                      getBufferLayout(),
                      getTextureLayout(),
                      {});

        shaderlib::textureBicubic();

        Function("textureAtlas",
                 {
                     {"level_index_filtering_assigned", ShaderDataType::ivec4()},
                     {"atlasScale_texSize", ShaderDataType::vec4()},
                     {"inUv", ShaderDataType::vec2()}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(level_index_filtering_assigned)
            ARGUMENT(atlasScale_texSize)
            ARGUMENT(inUv)

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

        INPUT_ATTRIBUTE(fPos)
        INPUT_ATTRIBUTE(fNorm)
        INPUT_ATTRIBUTE(fTan)
        INPUT_ATTRIBUTE(fUv)
        INPUT_ATTRIBUTE(vPos)
        INPUT_ATTRIBUTE(fT)
        INPUT_ATTRIBUTE(fB)
        INPUT_ATTRIBUTE(fN)

        OUTPUT_ATTRIBUTE(oPosition)
        OUTPUT_ATTRIBUTE(oNormal)
        OUTPUT_ATTRIBUTE(oTangent)
        OUTPUT_ATTRIBUTE(oRoughnessMetallicAO)
        OUTPUT_ATTRIBUTE(oAlbedo)
        OUTPUT_ATTRIBUTE(oObjectShadows)

        BUFFER_ELEMENT(data, model)

        BUFFER_ELEMENT(data, objectID_boneOffset_shadows)
        BUFFER_ELEMENT(data, metallic_roughness_ambientOcclusion)
        BUFFER_ELEMENT(data, albedoColor)
        BUFFER_ELEMENT(data, normalIntensity)

        BUFFER_ELEMENT(data, normal_level_index_filtering_assigned)
        BUFFER_ELEMENT(data, normal_atlasScale_texSize)

        BUFFER_ELEMENT(data, metallic_level_index_filtering_assigned)
        BUFFER_ELEMENT(data, metallic_atlasScale_texSize)

        BUFFER_ELEMENT(data, roughness_level_index_filtering_assigned)
        BUFFER_ELEMENT(data, roughness_atlasScale_texSize)

        BUFFER_ELEMENT(data, ambientOcclusion_level_index_filtering_assigned)
        BUFFER_ELEMENT(data, ambientOcclusion_atlasScale_texSize)

        BUFFER_ELEMENT(data, albedo_level_index_filtering_assigned)
        BUFFER_ELEMENT(data, albedo_atlasScale_texSize)

        oPosition = vec4(fPos, 1);

        If(data_albedo_level_index_filtering_assigned.w() == 0);
        {
            oAlbedo = data_albedoColor;
        }
        Else();
        {
            oAlbedo = textureAtlas(data_albedo_level_index_filtering_assigned,
                                   data_albedo_atlasScale_texSize,
                                   fUv);
        }

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        oRoughnessMetallicAO.x() = textureAtlas(data_roughness_level_index_filtering_assigned,
                                                data_roughness_atlasScale_texSize,
                                                fUv).x()
                                   + data_metallic_roughness_ambientOcclusion.y();

        // Metallic
        oRoughnessMetallicAO.y() = textureAtlas(data_metallic_level_index_filtering_assigned,
                                                data_metallic_atlasScale_texSize,
                                                fUv).x()
                                   + data_metallic_roughness_ambientOcclusion.x();

        // Ambient Occlusion
        oRoughnessMetallicAO.z() = textureAtlas(data_ambientOcclusion_level_index_filtering_assigned,
                                                data_ambientOcclusion_atlasScale_texSize,
                                                fUv).x()
                                   + data_metallic_roughness_ambientOcclusion.z();

        mat3 normalMatrix = matrix3(transpose(inverse(data_model)));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(data_normal_level_index_filtering_assigned.w() != 0);
        {
            mat3 tbn;
            tbn = matrix(fT, fB, fN);
            vec3 texNormal;
            texNormal = textureAtlas(data_normal_level_index_filtering_assigned,
                                     data_normal_atlasScale_texSize,
                                     fUv).xyz()
                        * vec3(data_normalIntensity.x(), data_normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            oNormal = vec4(normalize(texNormal), 1);
        }
        EndIf();

        oObjectShadows.x() = data_objectID_boneOffset_shadows.x();
        oObjectShadows.y() = data_objectID_boneOffset_shadows.z();
        oObjectShadows.z() = 0;
        oObjectShadows.w() = 1;

        return builder.build();
    }
}
