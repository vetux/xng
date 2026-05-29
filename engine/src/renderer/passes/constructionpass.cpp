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

        //TODO: Find cause of artifacts when sampling from normal textures with BILINEAR or BICUBIC filtering.

        TextureArray(TEXTURE_2D_ARRAY, RGBA8, 12, textures)

        ivec2 level_index = textureDef.value().level_index;
        vec4 atlasScale_texSize = textureDef.value().scale_texSize;
        ivec4 minFilter_magFilter_mipFilter_wrap = textureDef.value().minFilter_magFilter_mipFilter_wrap;

        vec2 wrappedUv;
        If(minFilter_magFilter_mipFilter_wrap.w() == WRAP_REPEAT)
            wrappedUv = fract(inUv); // REPEAT
        Else
            If(minFilter_magFilter_mipFilter_wrap.w() == WRAP_CLAMP_TO_EDGE)
                wrappedUv = clamp(inUv, vec2(0.0f), vec2(1.0f)); // CLAMP
            Else
                wrappedUv = 1.0f - abs(mod(inUv, vec2(2.0f)) - 1.0f); // MIRROR
            Fi
        Fi

        vec2 uv = wrappedUv * atlasScale_texSize.xy();

        // Actual pixel dimensions of this image within the layer
        vec2 imagePixelSize = atlasScale_texSize.xy() * atlasScale_texSize.zw();

        // Derivatives in texel space — use inUv not uv, before the scale bakes in
        vec2 dx = partialDerivativeX(inUv * imagePixelSize);
        vec2 dy = partialDerivativeY(inUv * imagePixelSize);
        // Use min to avoid over-mipping at UV seams: one gradient vector is large at the seam
        // (crosses the UV discontinuity) while the other is normal; min picks the normal one.
        Float lod = 0.5f * log2(min(dot(dx, dx), dot(dy, dy)) + 1e-10f);

        // Clamp so you never sample a mip level coarser than the image content
        Float maxLod = log2(min(imagePixelSize.x(), imagePixelSize.y()));
        lod = clamp(lod, 0.0f, maxLod);

        Int layer = level_index.y();
        Int arrayIndex = level_index.x();

        vec4 ret;
        If(lod <= 0.0f)
            // Magnification
            Int magFilter = minFilter_magFilter_mipFilter_wrap.y();
            If(magFilter == FILTER_BICUBIC)
                ret = textureBicubicArray(textures[arrayIndex], vec3(uv.x(), uv.y(), layer), imagePixelSize);
            Else
                If(magFilter == FILTER_BILINEAR)
                    ret = textureBilinearArray(textures[arrayIndex], vec3(uv.x(), uv.y(), layer), imagePixelSize);
                Else
                    ret = textureSampleArray(textures[arrayIndex], vec3(uv.x(), uv.y(), layer));
                Fi
            Fi
        Else
            // Minification
            Int minFilter = minFilter_magFilter_mipFilter_wrap.x();
            Int mipFilter = minFilter_magFilter_mipFilter_wrap.z();

            Float lodFloor = floor(lod);
            Float lodCeil = min(lodFloor + 1.0f, maxLod); // clamp s1
            Float lodFrac = fract(lod);

            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    ret = textureBicubicArrayLod(textures[arrayIndex],
                                                 vec3(uv.x(), uv.y(), layer), imagePixelSize, lodFloor);
                Else
                    vec4 s0 = textureBicubicArrayLod(textures[arrayIndex],
                                                     vec3(uv.x(), uv.y(), layer), imagePixelSize, lodFloor);
                    vec4 s1 = textureBicubicArrayLod(textures[arrayIndex],
                                                     vec3(uv.x(), uv.y(), layer), imagePixelSize, lodCeil);
                    ret = mix(s0, s1, lodFrac);
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        ret = textureBilinearArrayLod(textures[arrayIndex],
                                                     vec3(uv.x(), uv.y(), layer), imagePixelSize, lodFloor);
                    Else
                        vec4 s0 = textureBilinearArrayLod(textures[arrayIndex],
                                                         vec3(uv.x(), uv.y(), layer), imagePixelSize, lodFloor);
                        vec4 s1 = textureBilinearArrayLod(textures[arrayIndex],
                                                         vec3(uv.x(), uv.y(), layer), imagePixelSize, lodCeil);
                        ret = mix(s0, s1, lodFrac);
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        ret = textureSampleArrayLod(textures[arrayIndex], vec3(uv.x(), uv.y(), layer), lodFloor);
                    Else
                        vec4 s0 = textureSampleArrayLod(textures[arrayIndex],
                                                        vec3(uv.x(), uv.y(), layer), lodFloor);
                        vec4 s1 = textureSampleArrayLod(textures[arrayIndex],
                                                        vec3(uv.x(), uv.y(), layer), lodCeil);
                        ret = mix(s0, s1, lodFrac);
                    Fi
                Fi
            Fi
        Fi

        IRReturn(ret);

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

        If(material.albedo.level_index.x() < 0)
            oAlbedo = material.albedoColor;
        Else
            oAlbedo = texture_atlas(material.albedo, fUv);
        Fi

        oRoughnessMetallicAO = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Roughness
        If(material.roughness.level_index.x() < 0)
            oRoughnessMetallicAO.x() = material.metallic_roughness_ambientOcclusion.y();
        Else
            oRoughnessMetallicAO.x() = texture_atlas(material.roughness, fUv).x();
        Fi

        // Metallic
        If(material.metallic.level_index.x() < 0)
            oRoughnessMetallicAO.y() = material.metallic_roughness_ambientOcclusion.x();
        Else
            oRoughnessMetallicAO.y() = texture_atlas(material.metallic, fUv).x();
        Fi

        // Ambient Occlusion
        If(material.ambientOcclusion.level_index.x() < 0)
            oRoughnessMetallicAO.z() = material.metallic_roughness_ambientOcclusion.z();
        Else
            oRoughnessMetallicAO.z() = texture_atlas(material.ambientOcclusion, fUv).x();
        Fi

        mat3 normalMatrix = mat3(transpose(inverse(fModel)));
        oNormal = vec4(normalize(normalMatrix * fNorm), 1);
        oTangent = vec4(normalize(normalMatrix * fTan), 1);

        If(material.normal.level_index.x() >= 0)
            mat3 tbn = mat3(fT, fB, fN);
            vec3 texNormal = texture_atlas(material.normal, fUv).xyz()
                             * vec3(material.normalIntensity.x(), material.normalIntensity.x(), 1);
            texNormal = tbn * normalize(texNormal * 2.0f - 1.0f);
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
