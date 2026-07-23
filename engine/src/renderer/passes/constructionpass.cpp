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

using namespace xng::rg;
using namespace xng::ShaderScript;

namespace xng {
    using namespace RenderPipelineCompilerStubs;

    Shader ConstructionPass::compileVertexShader() {
        BeginShader(Shader::VERTEX)

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

        vec4 position = vec4(getVertexAttribute(POSITION), 1.0f);
        vec2 uv = vec2(getVertexAttribute(UV));
        vec3 normal = vec3(getVertexAttribute(NORMAL));
        vec3 tangent = vec3(getVertexAttribute(TANGENT));
        vec3 bitangent = vec3(getVertexAttribute(BITANGENT));

        mat4 model = mat4(getModel());
        mat4 mvp = mat4(getModelViewProjection());

        vPos = mvp * position;
        fPos = (model * position).xyz();
        fUv = uv;

        fNorm = normalize(normal);
        fTan = normalize(tangent);

        //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
        fN = normalize((getModel() * vec4(normalize(normal), 0.0)).xyz());
        fT = normalize((getModel() * vec4(normalize(tangent), 0.0)).xyz());
        fB = normalize((getModel() * vec4(normalize(bitangent), 0.0)).xyz());

        fModel = model;

        If(getMaterialProperty(PBRMaterial::RECEIVE_SHADOWS) == true)
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

        sideEffect(writeAttachment(GBUFFER_POSITION, vec4(fPos, 1)));

        // Albedo
        If(getMaterialProperty(PBRMaterial::MATERIAL_ALBEDO_HAS_TEXTURE) == true)
            sideEffect(writeAttachment(GBUFFER_ALBEDO,
                                       sampleMaterialTexture(
                                           PBRMaterial::MATERIAL_ALBEDO_TEXTURE,
                                           fUv
                                       )));
        Else
            sideEffect(writeAttachment(GBUFFER_ALBEDO,
                                       getMaterialProperty(
                                           PBRMaterial::MATERIAL_ALBEDO_COLOR
                                       )));
        Fi

        vec4 roughnessMetallicAO;
        roughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        If(getMaterialProperty(PBRMaterial::MATERIAL_ROUGHNESS_HAS_TEXTURE) == true)
            roughnessMetallicAO.x() = vec4(sampleMaterialTexture(PBRMaterial::MATERIAL_ROUGHNESS_TEXTURE, fUv)).x();
        Else
            roughnessMetallicAO.x() = getMaterialProperty(PBRMaterial::MATERIAL_ROUGHNESS_COLOR);
        Fi

        If(getMaterialProperty(PBRMaterial::MATERIAL_METALLIC_HAS_TEXTURE) == true)
            roughnessMetallicAO.y() = vec4(sampleMaterialTexture(PBRMaterial::MATERIAL_METALLIC_TEXTURE, fUv)).x();
        Else
            roughnessMetallicAO.y() = getMaterialProperty(PBRMaterial::MATERIAL_METALLIC_COLOR);
        Fi

        If(getMaterialProperty(PBRMaterial::MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE) == true)
            roughnessMetallicAO.z() = vec4(sampleMaterialTexture(PBRMaterial::MATERIAL_AMBIENT_OCCLUSION_TEXTURE, fUv)).
                    x();
        Else
            roughnessMetallicAO.z() = getMaterialProperty(PBRMaterial::MATERIAL_AMBIENT_OCCLUSION_COLOR);
        Fi

        sideEffect(writeAttachment(GBUFFER_ROUGHNESS_METALLIC_AO, roughnessMetallicAO));

        // TODO: Fix broken normal mapping (Normal map not applying)
        // Normals
        mat3 normalMatrix = mat3(transpose(inverse(fModel)));
        vec4 oNormal;
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        vec4 oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(getMaterialProperty(PBRMaterial::MATERIAL_NORMAL_HAS_TEXTURE) == true)
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal;
            texNormal = vec4(sampleMaterialTexture(PBRMaterial::MATERIAL_NORMAL_TEXTURE, fUv)).xyz();
            texNormal = texNormal * 2.0f - 1.0f;
            If(getMaterialProperty(PBRMaterial::MATERIAL_NORMAL_FLIP) == true)
                texNormal.y() = texNormal.y() * -1.0f;
            Fi
            texNormal = vec3(texNormal.x() * getMaterialProperty(PBRMaterial::MATERIAL_NORMAL_INTENSITY),
                             texNormal.y() * getMaterialProperty(PBRMaterial::MATERIAL_NORMAL_INTENSITY),
                             texNormal.z());
            texNormal = tbn * normalize(texNormal);
            oNormal = vec4(normalize(texNormal), 1);
        Fi

        sideEffect(writeAttachment(GBUFFER_NORMAL, oNormal));

        ivec4 oObjectShadows(0, getMaterialProperty(PBRMaterial::RECEIVE_SHADOWS), 0, 0);
        sideEffect(writeAttachment(GBUFFER_OBJECT_ID_RECEIVE_SHADOWS, oObjectShadows));

        EndShader();

        return BuildShader()
    }
}
