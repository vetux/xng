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

#include "xng/graphics/passes/canvasrenderpass.hpp"


#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    DefineStruct(BufferData,
             vec4, color,
             Float, colorMixFactor,
             Float, alphaMixFactor,
             Float, colorFactor,
             Int, texAtlasLevel,
             Int, texAtlasIndex,
             Int, texFilter,
             mat4, mvp,
             vec4, uvOffset_uvScale,
             vec4, atlasScale_texSize,
             Float, useCustomTexture)

    Shader CanvasRenderPass::createVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec2, position)
        Input(vec2, uv)

        Output(vec4, fPosition)
        Output(vec2, fUv)

        DeclareStruct(BufferData)

        Buffer(vars, BufferData)

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12)
        Texture(customTexture, TEXTURE_2D, RGBA)

        fPosition = vars.mvp * vec4(position.x(), position.y(), 0.0, 1.0);
        fUv = uv;

        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader CanvasRenderPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fPosition)
        Input(vec2, fUv)

        Output(vec4, color)

        DeclareStruct(BufferData)

        Buffer(vars, BufferData)

        TextureArray(atlasTextures, TEXTURE_2D_ARRAY, RGBA, 12)
        Texture(customTexture, TEXTURE_2D, RGBA)

        shaderlib::textureBicubic();

        If(vars.texAtlasIndex >= 0)
        {
            vec2 uv = fUv.xy();
            uv = uv * vars.uvOffset_uvScale.zw();
            uv = uv + vars.uvOffset_uvScale.xy();
            uv = uv * vars.atlasScale_texSize.xy();

            vec4 texColor;
            texColor = vec4(0, 0, 0, 0);
            If(vars.texFilter == 1)
            {
                texColor = textureBicubic(atlasTextures[vars.texAtlasLevel],
                                          vec3(uv.x(), uv.y(), vars.texAtlasIndex),
                                          vars.atlasScale_texSize.zw());
            }
            Else
            {
                texColor = textureSampleArray(atlasTextures[vars.texAtlasLevel],
                                              vec3(uv.x(), uv.y(), vars.texAtlasIndex));
            }
            EndIf
            If(vars.colorFactor != 0)
            {
                color = vars.color * texColor;
            }
            Else
            {
                vec4 buffColor;
                buffColor = vars.color.xyzw();
                color.xyz() = mix(texColor.xyz(), buffColor.xyz(), vars.colorMixFactor);
                color.w() = mix(texColor.w(), buffColor.w(), vars.alphaMixFactor);
            }
            EndIf
        }
        Else
        {
            If(vars.useCustomTexture == 1)
            {
                color = textureSample(customTexture, fUv.xy());
            }
            Else
            {
                color = vars.color;
            }
            EndIf
        }
        EndIf

        return BuildShader();
    }
}
