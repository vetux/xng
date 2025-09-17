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

        vec3 vPosition = readAttribute("position");
        vec3 vNormal = readAttribute("normal");
        vec2 vUv = readAttribute("uv");
        vec3 vTangent = readAttribute("tangent");

        vec4 vPos;
        vPos = readBuffer("data", "mvp") * vec4(vPosition, 1);

        mat4 model;
        model = readBuffer("data", "model");

        writeAttribute("vPos", vPos);
        writeAttribute("fPos", (model * vec4(vPosition, 1)).xyz());
        writeAttribute("fUv", vUv);
        writeAttribute("fNorm", normalize(vNormal));
        writeAttribute("fTan", normalize(vTangent));

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        writeAttribute("fN", normalize((model * vec4(vNormal, 0.0)).xyz()));
        writeAttribute("fT", normalize((model * vec4(vTangent, 0.0)).xyz()));
        writeAttribute("fB", normalize((model * vec4(cross(vNormal, vTangent.xyz())) * 1, 0.0)).xyz());

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

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument("offset", ShaderDataType::integer())},
                 ShaderDataType::vec4());
        {
            vec3 vPosition;
            vPosition = readAttribute("position");
            Int offset;
            offset = argument("offset");
            If(offset < 0);
            {
                Return(vec4(vPosition, 1.0f));
            }
            EndIf();

            Int boneCount;
            boneCount = getDynamicBufferLength("bones");

            ivec4 boneIds;
            boneIds = readAttribute("boneIds");
            vec4 boneWeights;
            boneWeights = readAttribute("boneWeights");

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1);
            {
                If(boneIds.x() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    mat4 boneMatrix;
                    boneMatrix = readDynamicBuffer("bones",
                                                   "matrix",
                                                   boneIds.x() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.y() > -1);
            {
                If(boneIds.y() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    mat4 boneMatrix;
                    boneMatrix = readDynamicBuffer("bones",
                                                   "matrix",
                                                   boneIds.y() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.z() > -1);
            {
                If(boneIds.z() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    mat4 boneMatrix;
                    boneMatrix = readDynamicBuffer("bones",
                                                   "matrix",
                                                   boneIds.z() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.w() > -1);
            {
                If(boneIds.w() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    mat4 boneMatrix;
                    boneMatrix = readDynamicBuffer("bones",
                                                   "matrix",
                                                   boneIds.w() + offset);
                    vec4 localPosition;
                    localPosition = boneMatrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf();
            }
            EndIf();

            Return(totalPosition);
        }
        EndFunction();

        ivec4 objectID_boneOffset_shadows = readBuffer("data", "objectID_boneOffset_shadows");

        vec4 pos;
        pos = Call("getSkinnedVertexPosition", objectID_boneOffset_shadows.y());

        vec3 vNormal;
        vNormal = readAttribute("normal");
        vec2 vUv;
        vUv = readAttribute("uv");
        vec3 vTangent;
        vTangent = readAttribute("tangent");

        mat4 mvp;
        mvp = readBuffer("data", "mvp");
        mat4 model;
        model = readBuffer("data", "model");

        vec4 vPos;
        vPos = mvp * pos;

        writeAttribute("vPos", vPos);
        writeAttribute("fPos", (model * pos).xyz());
        writeAttribute("fUv", vUv);

        writeAttribute("fNorm", normalize(vNormal));
        writeAttribute("fTan", normalize(vTangent));

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        writeAttribute("fN", normalize((model * vec4(vNormal, 0.0)).xyz()));
        writeAttribute("fT", normalize((model * vec4(vTangent, 0.0)).xyz()));
        writeAttribute("fB",
                       normalize((model * vec4(cross(normalize(vNormal), normalize(vTangent).xyz()) * 1, 0.0)).xyz()));

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
            ivec4 level_index_filtering_assigned = argument("level_index_filtering_assigned");
            vec4 atlasScale_texSize = argument("atlasScale_texSize");
            vec2 inUv = argument("inUv");

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

        vec3 fPos = readAttribute("fPos");
        vec3 fNorm = readAttribute("fNorm");
        vec3 fTan = readAttribute("fTan");
        vec2 fUv = readAttribute("fUv");
        vec4 vPos = readAttribute("vPos");
        vec3 fT = readAttribute("fT");
        vec3 fB = readAttribute("fB");
        vec3 fN = readAttribute("fN");

        writeAttribute("oPosition", vec4(fPos, 1));

        // Albedo
        ivec4 albedo_level_index_filtering_assigned = readBuffer("data", "albedo_level_index_filtering_assigned");
        vec4 albedo_atlasScale_texSize = readBuffer("data", "albedo_atlasScale_texSize");

        If(albedo_level_index_filtering_assigned.w() == 0);
        {
            writeAttribute("oAlbedo", readBuffer("data", "albedoColor"));
        }
        Else();
        {
            writeAttribute("oAlbedo", textureAtlas(albedo_level_index_filtering_assigned,
                                                   albedo_atlasScale_texSize,
                                                   fUv));
        }

        vec4 metallic_roughness_ambientOcclusion = readBuffer("data", "metallic_roughness_ambientOcclusion");

        vec4 oRoughnessMetallicAO;
        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        ivec4 roughness_level_index_filtering_assigned = readBuffer("data", "roughness_level_index_filtering_assigned");
        vec4 roughness_atlasScale_texSize = readBuffer("data", "roughness_atlasScale_texSize");

        oRoughnessMetallicAO.x() = textureAtlas(roughness_level_index_filtering_assigned,
                                                roughness_atlasScale_texSize,
                                                fUv).x()
                                   + metallic_roughness_ambientOcclusion.y();

        // Metallic
        ivec4 metallic_level_index_filtering_assigned = readBuffer("data", "metallic_level_index_filtering_assigned");
        vec4 metallic_atlasScale_texSize = readBuffer("data", "metallic_atlasScale_texSize");

        oRoughnessMetallicAO.y() = textureAtlas(metallic_level_index_filtering_assigned,
                                                metallic_atlasScale_texSize,
                                                fUv).x()
                                   + metallic_roughness_ambientOcclusion.x();

        // Ambient Occlusion
        ivec4 ambientOcclusion_level_index_filtering_assigned = readBuffer(
            "data", "ambientOcclusion_level_index_filtering_assigned");
        vec4 ambientOcclusion_atlasScale_texSize = readBuffer("data", "ambientOcclusion_atlasScale_texSize");

        oRoughnessMetallicAO.z() = textureAtlas(ambientOcclusion_level_index_filtering_assigned,
                                                ambientOcclusion_atlasScale_texSize,
                                                fUv).x()
                                   + metallic_roughness_ambientOcclusion.z();

        writeAttribute("oRoughnessMetallicAO", oRoughnessMetallicAO);

        mat4 model = readBuffer("data", "model");

        mat3 normalMatrix;
        mat4 invModel;
        invModel = transpose(inverse(model));
        normalMatrix = matrix(invModel.column(0).xyz(),
                              invModel.column(1).xyz(),
                              invModel.column(2).xyz());

        vec4 oTangent;
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        writeAttribute("oTangent", oTangent);

        // Normals;
        vec4 oNormal;
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);

        ivec4 normal_level_index_filtering_assigned = readBuffer("data", "normal_level_index_filtering_assigned");
        vec4 normal_atlasScale_texSize = readBuffer("data", "normal_atlasScale_texSize");

        vec4 normalIntensity = readBuffer("data", "normalIntensity");
        If(normal_level_index_filtering_assigned.w() != 0);
        {
            mat3 tbn;
            tbn = matrix(fT, fB, fN);
            vec3 texNormal;
            texNormal = textureAtlas(normal_level_index_filtering_assigned, normal_atlasScale_texSize, fUv).xyz()
                        * vec3(normalIntensity.x(), normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            oNormal = vec4(normalize(texNormal), 1);
        }
        EndIf();

        writeAttribute("oNormal", oNormal);

        ivec4 objectID_boneOffset_shadows = readBuffer("data", "objectID_boneOffset_shadows");

        ivec4 oObjectShadows;
        oObjectShadows = ivec4(objectID_boneOffset_shadows.x(), objectID_boneOffset_shadows.y(), 0, 1);

        writeAttribute("oObjectShadows", oObjectShadows);

        return builder.build();
    }
}
