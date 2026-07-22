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
using namespace xng::RenderPipelineCompilerStubs;

namespace xng {
    rg::Shader CanvasPass::compileVertexShader() {
        BeginShader(Shader::VERTEX)

        Output(vec4, fPos)
        Output(vec2, fUv)

        vec4 pos = vec4(getVertexAttribute(POSITION), 1.0f);

        fPos = getModelViewProjection() * pos;
        fUv = getVertexAttribute(UV);

        setVertexPosition(fPos);

        EndShader();

        return BuildShader();
    }

    rg::Shader CanvasPass::compileFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fPos)
        Input(vec2, fUv)

        vec4 texColor;
        texColor = vec4(0.0f);
        If(getMaterialProperty(CanvasMaterial::PAINT_HAS_TEXTURE) == Bool(true))
            texColor = sampleMaterialTexture(CanvasMaterial::PAINT_TEXTURE, fUv);
        Fi

        vec4 color = vec4(getMaterialProperty(CanvasMaterial::PAINT_COLOR));

        vec4 oColor = mix(texColor, color, getMaterialProperty(CanvasMaterial::PAINT_MIX));

        sideEffect(writeAttachment(0, oColor));

        EndShader();

        return BuildShader();
    }
}
