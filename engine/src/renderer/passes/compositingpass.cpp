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

#include "xng/renderer/passes/compositingpass.hpp"

#include "xng/shaderscript/macro/helpermacros.hpp"
#include "xng/shaderscript/shaderscript.hpp"

#include "xng/renderer/shaderlib/virtualtexture.hpp"

using namespace xng::rg;
using namespace xng::ShaderScript;
using namespace xng::shaderlib::virtualtexture;

namespace xng {
    rg::Shader CompositingPass::compileVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, position)
        Input(vec2, uv)

        Output(vec4, fPos)
        Output(vec2, fUv)

        Texture(TEXTURE_2D, RGBA8, texture)

        fPos = vec4(position, 1.0f);
        fUv = uv;

        setVertexPosition(vec4(position, 1.0f));

        EndShader();

        return BuildShader();
    }

    rg::Shader CompositingPass::compileFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fPos)
        Input(vec2, fUv)

        Output(vec4, oColor)

        Texture(TEXTURE_2D, RGBA8, texture)

        oColor = textureSample(texture, fUv);

        EndShader();

        return BuildShader();
    }
}
