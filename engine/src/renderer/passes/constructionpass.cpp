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
    static vec4 texture_atlas(Param<ShaderTexture> textureDef, Param<vec2> inUv) {
        IRFunction

        TextureArray(TEXTURE_2D_ARRAY, RGBA8, 12, textures)

        ivec4 level_index_filtering_assigned = textureDef.value().level_index_filtering_assigned;
        vec4 atlasScale_texSize = textureDef.value().scale_texSize;

        If(level_index_filtering_assigned.w() == 0)
            IRReturn(vec4(0.0f, 0.0f, 0.0f, 0.0f));
        Else
            vec2 uv = inUv * atlasScale_texSize.xy();
            If(level_index_filtering_assigned.z() == 1)
                IRReturn(textureBicubicArray(textures[level_index_filtering_assigned.x()],
                    vec3(uv.x(), uv.y(), level_index_filtering_assigned.y()),
                    atlasScale_texSize.zw()));
            Else
                IRReturn(vec4(textureSampleArray(textures[level_index_filtering_assigned.x()],
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
        OutputFlat(mat4, fModel)
        OutputFlat(Int, fMaterialIndex)
        OutputFlat(Int, fObjectID)
        OutputFlat(Int, fReceiveShadows)

        //TODO: Nested buffer struct type definitions
        ShaderTexture _tex;

        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMaterial, materials)
        DynamicBuffer(ShaderDrawMesh, drawBuffer)

        TextureArray(TEXTURE_2D_ARRAY, RGBA8, 12, textures)

        UInt modelIndex = getBaseInstance() + getDrawID() + getInstanceID();

        vec4 pos = vec4(position, 1.0f);

        mat4 model = transforms[drawBuffer[modelIndex].transformIndex].transform;
        mat4 mvp = drawBuffer[modelIndex].mvp;

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

        fMaterialIndex = Int(drawBuffer[modelIndex].materialIndex);
        fObjectID = Int(drawBuffer[modelIndex].modelID);

        If(drawBuffer[modelIndex].receiveShadows)
            fReceiveShadows = Int(1);
        Else
            fReceiveShadows = Int(0);
        Fi

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
        InputFlat(mat4, fModel)
        InputFlat(Int, fMaterialIndex)
        InputFlat(Int, fObjectID)
        InputFlat(Int, fReceiveShadows)

        Output(vec4, oPosition)
        Output(vec4, oNormal)
        Output(vec4, oTangent)
        Output(vec4, oRoughnessMetallicAO)
        Output(vec4, oAlbedo)
        Output(ivec4, oObjectShadows)

        //TODO: Nested buffer struct type definitions
        ShaderTexture _tex;

        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMaterial, materials)
        DynamicBuffer(ShaderDrawMesh, drawBuffer)

        TextureArray(TEXTURE_2D_ARRAY, RGBA8, 12, textures)

        ShaderMaterial material = materials[fMaterialIndex];

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

        oObjectShadows.x() = Int(fObjectID);
        oObjectShadows.y() = Int(fReceiveShadows);
        oObjectShadows.z() = Int(0);
        oObjectShadows.w() = Int(0);

        EndShader();

        return BuildShader()
    }
}
