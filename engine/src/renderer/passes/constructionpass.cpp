/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/renderer/passes/constructionpass.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"

#include "xng/renderer/shadertypes.hpp"
#include "xng/renderer/shaderlib/texfilter.hpp"

using namespace xng::rg;
using namespace xng::ShaderScript;
using namespace xng::shaderlib::texfilter;

namespace xng {
    static vec4 getSkinnedVertexPosition(Param<Int> offset,
                                         Param<vec3> position,
                                         Param<ivec4> boneIdsA,
                                         Param<vec4> boneWeightsA) {
        IRFunction

        DynamicBuffer(ShaderTransform, bones)

        ivec4 boneIds = boneIdsA;
        vec4 boneWeights = boneWeightsA;

        If(offset < 0)
            IRReturn(vec4(position, 1.0f));
        Fi

        Int boneCount = bones.length();

        vec4 totalPosition;
        totalPosition = vec4(0, 0, 0, 0);

        If(boneIds.x() > -1)
            If(boneIds.x() + offset >= boneCount)
                IRReturn(vec4(position, 1.0f));
            Else
                vec4 localPosition;
                localPosition = bones[boneIds.x() + offset].transform * vec4(position, 1.0f);
                totalPosition += localPosition * boneWeights.x();
            Fi
        Fi

        If(boneIds.y() > -1)
            If(boneIds.y() + offset >= boneCount)
                IRReturn(vec4(position, 1.0f));
            Else
                vec4 localPosition;
                localPosition = bones[boneIds.y() + offset].transform * vec4(position, 1.0f);
                totalPosition += localPosition * boneWeights.y();
            Fi
        Fi

        If(boneIds.z() > -1)
            If(boneIds.z() + offset >= boneCount)
                IRReturn(vec4(position, 1.0f));
            Else
                vec4 localPosition;
                localPosition = bones[boneIds.z() + offset].transform * vec4(position, 1.0f);
                totalPosition += localPosition * boneWeights.z();
            Fi
        Fi

        If(boneIds.w() > -1)
            If(boneIds.w() + offset >= boneCount)
                IRReturn(vec4(position, 1.0f));
            Else
                vec4 localPosition;
                localPosition = bones[boneIds.w() + offset].transform * vec4(position, 1.0f);
                totalPosition += localPosition * boneWeights.w();
            Fi
        Fi

        IRReturn(totalPosition);

        IRFunctionEnd
    }

    static vec4 texture_atlas(Param<ShaderTexture> textureDef, Param<vec2> inUv) {
        IRFunction

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        ivec4 level_index_filtering_assigned = textureDef.value().level_index_filtering_assigned;
        vec4 atlasScale_texSize = textureDef.value().scale_texSize;

        If(level_index_filtering_assigned.w() == 0)
            IRReturn(vec4(0.0f, 0.0f, 0.0f, 0.0f));
        Else
            vec2 uv = inUv * atlasScale_texSize.xy();
            If(level_index_filtering_assigned.z() == 1)
                IRReturn(textureBicubicArray(atlasTextures[level_index_filtering_assigned.x()],
                    vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                    atlasScale_texSize.zw()));
            Else
                IRReturn(vec4(textureSampleArray(atlasTextures[level_index_filtering_assigned.x()],
                    vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()))));
            Fi
        Fi

        IRFunctionEnd
    }

    Shader ConstructionPass::compileVertexShader() {
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
        Output(mat4, fModel)

        Parameter(UInt, objectId)
        Parameter(UInt, transformIndex)
        Parameter(UInt, materialIndex)
        Parameter(UInt, boneBaseIndex)
        Parameter(Bool, receiveShadows)

        Buffer(ShaderCamera, camera)
        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMaterial, materials)

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        mat4 model = transforms[transformIndex].transform;
        mat4 mvp = camera.projection * camera.view * model;

        vPos = mvp * vec4(position, 1);
        fPos = (model * vec4(position, 1)).xyz();
        fUv = uv;
        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((model * vec4(normal, 0.0)).xyz());
        fT = normalize((model * vec4(tangent, 0.0)).xyz());
        fB = normalize((model * vec4(cross(normalize(tangent), normalize(normal).xyz()) * 1, 0.0)).xyz());

        fModel = model;

        setVertexPosition(vPos);

        EndShader();
        return BuildShader();
    }

    Shader ConstructionPass::compileSkinnedVertexShader() {
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
        Output(mat4, fModel)

        Parameter(UInt, objectId)
        Parameter(UInt, transformIndex)
        Parameter(UInt, materialIndex)
        Parameter(UInt, boneBaseIndex)
        Parameter(Bool, receiveShadows)

        Buffer(ShaderCamera, camera)
        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMaterial, materials)

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        vec4 pos = getSkinnedVertexPosition(boneBaseIndex, position, boneIds, boneWeights);

        mat4 model = transforms[transformIndex].transform;
        mat4 mvp = camera.projection * camera.view * model;

        vPos = mvp * pos;
        fPos = (model * pos).xyz();
        fUv = uv;

        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((model * vec4(normalize(normal), 0.0)).xyz());
        fT = normalize((model * vec4(normalize(tangent), 0.0)).xyz());
        fB = normalize((model * vec4(cross(normalize(tangent), normalize(normal).xyz()) * 1, 0.0)).xyz());

        fModel = model;

        setVertexPosition(vPos);

        EndShader();

        return BuildShader()
    }

    Shader ConstructionPass::compileFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec3, fPos)
        Input(vec3, fNorm)
        Input(vec3, fTan)
        Input(vec2, fUv)
        Input(vec4, vPos)
        Input(vec3, fT)
        Input(vec3, fB)
        Input(vec3, fN)
        Input(mat4, fModel)

        Output(vec4, oPosition)
        Output(vec4, oNormal)
        Output(vec4, oTangent)
        Output(vec4, oRoughnessMetallicAO)
        Output(vec4, oAlbedo)
        Output(ivec4, oObjectShadows)

        Parameter(UInt, objectId)
        Parameter(UInt, transformIndex)
        Parameter(UInt, materialIndex)
        Parameter(UInt, boneBaseIndex)
        Parameter(Bool, receiveShadows)

        Buffer(ShaderCamera, camera)
        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMaterial, materials)
        DynamicBuffer(ShaderTransform, bones)

        TextureArray(TEXTURE_2D_ARRAY, RGBA, 12, atlasTextures)

        ShaderMaterial material = materials[materialIndex];

        oPosition = vec4(fPos, 1);

        If(material.albedo.level_index_filtering_assigned.w() == 0)
            oAlbedo = material.albedoColor;
        Else
            oAlbedo = texture_atlas(material.albedo, fUv);
        Fi

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        If(material.roughness.level_index_filtering_assigned.w() == 0)
            oRoughnessMetallicAO.x() = material.metallic_roughness_ambientOcclusion.y();
        Else
            oRoughnessMetallicAO.x() = texture_atlas(material.roughness, fUv).x();
        Fi


        // Metallic
        If(material.metallic.level_index_filtering_assigned.w() == 0)
            oRoughnessMetallicAO.y() = material.metallic_roughness_ambientOcclusion.x();
        Else
            oRoughnessMetallicAO.y() = texture_atlas(material.metallic, fUv).x();
        Fi

        // Ambient Occlusion
        If(material.ambientOcclusion.level_index_filtering_assigned.w() == 0)
            oRoughnessMetallicAO.z() = material.metallic_roughness_ambientOcclusion.z();
        Else
            oRoughnessMetallicAO.z() = texture_atlas(material.ambientOcclusion, fUv).x();
        Fi

        mat3 normalMatrix = mat3(transpose(inverse(fModel)));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(material.normal.level_index_filtering_assigned.w() != 0)
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = texture_atlas(material.normal, fUv).xyz()
                             * vec3(material.normalIntensity.x(), material.normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0 - 1.0);
            oNormal = vec4(normalize(texNormal), 1);
        Fi

        oObjectShadows.x() = objectId;
        oObjectShadows.y() = receiveShadows;
        oObjectShadows.z() = 0;
        oObjectShadows.w() = 1;

        EndShader();

        return BuildShader()
    }
}
