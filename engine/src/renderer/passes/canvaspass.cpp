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

#include "xng/renderer/passes/canvaspass.hpp"

#include "xng/shaderscript/macro/helpermacros.hpp"
#include "xng/shaderscript/shaderscript.hpp"

#include "xng/renderer/shaderlib/virtualtexture.hpp"

using namespace xng::rg;
using namespace xng::ShaderScript;
using namespace xng::shaderlib::virtualtexture;

namespace xng {
    rg::Shader CanvasPass::compileVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, position)
        Input(vec2, uv)

        Output(vec4, fPos)
        Output(vec2, fUv)

        Texture(TEXTURE_2D_ARRAY, RGBA8, atlasTexture)
        DynamicBuffer(UInt, tileMap)
        DynamicBuffer(UInt, tileMapOffsets)
        DynamicBuffer(UInt, residencyMap)
        DynamicBuffer(UInt, residencyMapOffsets)
        DynamicBufferRW(UInt, readbackBuffer)

        Parameter(UInt, atlasSize)
        Parameter(UInt, tileSize)
        Parameter(UInt, tileBorder)
        Parameter(Float, maxAnisotropy)

        Parameter(mat4, mvp)
        Parameter(vec4, color)
        Parameter(ivec4, textureSize_textureID_maxMip)
        Parameter(ivec4, minFilter_magFilter_mipFilter_wrap)
        Parameter(Bool, grayscale)

        vec4 pos = vec4(position, 1.0f);

        fPos = mvp * pos;
        fUv = uv;

        setVertexPosition(fPos);

        EndShader();

        return BuildShader();
    }

    rg::Shader CanvasPass::compileFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fPos)
        Input(vec2, fUv)

        Output(vec4, oColor)

        Texture(TEXTURE_2D_ARRAY, RGBA8, atlasTexture)
        DynamicBuffer(UInt, tileMap)
        DynamicBuffer(UInt, tileMapOffsets)
        DynamicBuffer(UInt, residencyMap)
        DynamicBuffer(UInt, residencyMapOffsets)
        DynamicBufferRW(UInt, readbackBuffer)

        Parameter(UInt, atlasSize)
        Parameter(UInt, tileSize)
        Parameter(UInt, tileBorder)
        Parameter(Float, maxAnisotropy)

        Parameter(mat4, mvp)
        Parameter(vec4, color)
        Parameter(ivec4, textureSize_textureID_maxMip)
        Parameter(ivec4, minFilter_magFilter_mipFilter_wrap)
        Parameter(Bool, grayscale)

        vec4 texColor = sample_virtual_readback(textureSize_textureID_maxMip.z(),
                                                fUv,
                                                minFilter_magFilter_mipFilter_wrap.w(),
                                                minFilter_magFilter_mipFilter_wrap.x(),
                                                minFilter_magFilter_mipFilter_wrap.y(),
                                                minFilter_magFilter_mipFilter_wrap.z(),
                                                textureSize_textureID_maxMip.xy(),
                                                textureSize_textureID_maxMip.w(),
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

        If(grayscale)
            oColor = color;
            oColor.w() = texColor.x();
        Else
            oColor = texColor;
        Fi

        EndShader();

        return BuildShader();
    }
}
