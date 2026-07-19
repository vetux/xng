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
#include "xng/renderer/shaderlib/virtualtexture.hpp"

using namespace xng::rg;
using namespace xng::ShaderScript;
using namespace xng::shaderlib::virtualtexture;

namespace xng {
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

        StorageBufferDynamic(ShaderTransform, transforms)
        StorageBufferDynamic(ShaderMaterial, materials)
        StorageBufferDynamic(ShaderDrawMesh, drawBuffer)

        Texture(TEXTURE_2D_ARRAY, RGBA8, atlasTexture)
        StorageBufferDynamic(UInt, tileMap)
        StorageBufferDynamic(UInt, tileMapOffsets)
        StorageBufferDynamic(UInt, residencyMap)
        StorageBufferDynamic(UInt, residencyMapOffsets)
        StorageBufferDynamicRW(UInt, readbackBuffer)

        Parameter(UInt, atlasSize)
        Parameter(UInt, tileSize)
        Parameter(UInt, tileBorder)
        Parameter(Float, maxAnisotropy)

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
        fB = normalize((model * vec4(normalize(bitangent), 0.0)).xyz());

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

        StorageBufferDynamic(ShaderTransform, transforms)
        StorageBufferDynamic(ShaderMaterial, materials)
        StorageBufferDynamic(ShaderDrawMesh, drawBuffer)

        Texture(TEXTURE_2D_ARRAY, RGBA8, atlasTexture)
        StorageBufferDynamic(UInt, tileMap)
        StorageBufferDynamic(UInt, tileMapOffsets)
        StorageBufferDynamic(UInt, residencyMap)
        StorageBufferDynamic(UInt, residencyMapOffsets)
        StorageBufferDynamicRW(UInt, readbackBuffer)

        Parameter(UInt, atlasSize)
        Parameter(UInt, tileSize)
        Parameter(UInt, tileBorder)
        Parameter(Float, maxAnisotropy)

        ShaderMaterial material = materials[fMaterialIndex];

        oPosition = vec4(fPos, 1);

        // Albedo
        If(material.albedo.textureSize_textureID_maxMip.z() < 0)
            oAlbedo = material.albedoColor;
        Else
            oAlbedo = sample_virtual_readback(material.albedo.textureSize_textureID_maxMip.z(),
                                              fUv,
                                              material.albedo.minFilter_magFilter_mipFilter_wrap.w(),
                                              material.albedo.minFilter_magFilter_mipFilter_wrap.x(),
                                              material.albedo.minFilter_magFilter_mipFilter_wrap.y(),
                                              material.albedo.minFilter_magFilter_mipFilter_wrap.z(),
                                              material.albedo.textureSize_textureID_maxMip.xy(),
                                              material.albedo.textureSize_textureID_maxMip.w(),
                                              atlasSize,
                                              tileSize,
                                              tileBorder,
                                              maxAnisotropy,
                                              tileMapOffsets,
                                              tileMap,
                                              residencyMapOffsets,
                                              residencyMap,
                                              readbackBuffer,
                                              atlasTexture);
        Fi

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        If(material.roughness.textureSize_textureID_maxMip.z() < 0)
            oRoughnessMetallicAO.x() = material.metallic_roughness_ambientOcclusion.y();
        Else
            oRoughnessMetallicAO.x() = sample_virtual_readback(material.roughness.textureSize_textureID_maxMip.z(),
                                                               fUv,
                                                               material.roughness.minFilter_magFilter_mipFilter_wrap.
                                                               w(),
                                                               material.roughness.minFilter_magFilter_mipFilter_wrap.
                                                               x(),
                                                               material.roughness.minFilter_magFilter_mipFilter_wrap.
                                                               y(),
                                                               material.roughness.minFilter_magFilter_mipFilter_wrap.
                                                               z(),
                                                               material.roughness.textureSize_textureID_maxMip.xy(),
                                                               material.roughness.textureSize_textureID_maxMip.w(),
                                                               atlasSize,
                                                               tileSize,
                                                               tileBorder,
                                                               maxAnisotropy,
                                                               tileMapOffsets,
                                                               tileMap,
                                                               residencyMapOffsets,
                                                               residencyMap,
                                                               readbackBuffer,
                                                               atlasTexture).x();
        Fi

        // Metallic
        If(material.metallic.textureSize_textureID_maxMip.z() < 0)
            oRoughnessMetallicAO.y() = material.metallic_roughness_ambientOcclusion.x();
        Else
            oRoughnessMetallicAO.y() = sample_virtual_readback(material.metallic.textureSize_textureID_maxMip.z(),
                                                               fUv,
                                                               material.metallic.minFilter_magFilter_mipFilter_wrap.w(),
                                                               material.metallic.minFilter_magFilter_mipFilter_wrap.x(),
                                                               material.metallic.minFilter_magFilter_mipFilter_wrap.y(),
                                                               material.metallic.minFilter_magFilter_mipFilter_wrap.z(),
                                                               material.metallic.textureSize_textureID_maxMip.xy(),
                                                               material.metallic.textureSize_textureID_maxMip.w(),
                                                               atlasSize,
                                                               tileSize,
                                                               tileBorder,
                                                               maxAnisotropy,
                                                               tileMapOffsets,
                                                               tileMap,
                                                               residencyMapOffsets,
                                                               residencyMap,
                                                               readbackBuffer,
                                                               atlasTexture).x();
        Fi

        // Ambient Occlusion
        If(material.ambientOcclusion.textureSize_textureID_maxMip.z() < 0)
            oRoughnessMetallicAO.z() = material.metallic_roughness_ambientOcclusion.z();
        Else
            oRoughnessMetallicAO.z() = sample_virtual_readback(
                material.ambientOcclusion.textureSize_textureID_maxMip.z(),
                fUv,
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap.w(),
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap.x(),
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap.y(),
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap.z(),
                material.ambientOcclusion.textureSize_textureID_maxMip.xy(),
                material.ambientOcclusion.textureSize_textureID_maxMip.w(),
                atlasSize,
                tileSize,
                tileBorder,
                maxAnisotropy,
                tileMapOffsets,
                tileMap,
                residencyMapOffsets,
                residencyMap,
                readbackBuffer,
                atlasTexture).x();
        Fi

        // Normals
        mat3 normalMatrix = mat3(transpose(inverse(fModel)));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(material.normal.textureSize_textureID_maxMip.z() >= 0)
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = sample_virtual_readback(material.normal.textureSize_textureID_maxMip.z(),
                                                     fUv,
                                                     material.normal.minFilter_magFilter_mipFilter_wrap.w(),
                                                     material.normal.minFilter_magFilter_mipFilter_wrap.x(),
                                                     material.normal.minFilter_magFilter_mipFilter_wrap.y(),
                                                     material.normal.minFilter_magFilter_mipFilter_wrap.z(),
                                                     material.normal.textureSize_textureID_maxMip.xy(),
                                                     material.normal.textureSize_textureID_maxMip.w(),
                                                     atlasSize,
                                                     tileSize,
                                                     tileBorder,
                                                     maxAnisotropy,
                                                     tileMapOffsets,
                                                     tileMap,
                                                     residencyMapOffsets,
                                                     residencyMap,
                                                     readbackBuffer,
                                                     atlasTexture).xyz();
            texNormal = texNormal * 2.0f - 1.0f;
            If(material.normalIntensity_flipNormal.y() != 0.0f)
                texNormal.y() = texNormal.y() * -1.0f;
            Fi
            texNormal = vec3(texNormal.x() * material.normalIntensity_flipNormal.x(),
                             texNormal.y() * material.normalIntensity_flipNormal.x(),
                             texNormal.z());
            texNormal = tbn * normalize(texNormal);
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
