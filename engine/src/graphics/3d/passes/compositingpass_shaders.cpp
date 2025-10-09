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

#include "xng/graphics/3d/passes/compositingpass.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng {
    ShaderAttributeLayout getVertexLayout() {
        ShaderAttributeLayout ret;
        ret.addElement("position", ShaderDataType::vec3());
        ret.addElement("uv", ShaderDataType::vec2());
        return ret;
    }

    Shader CompositingPass::createVertexShader() {
        BeginShader(Shader::VERTEX);

        Input(ShaderDataType::vec3(), position);
        Input(ShaderDataType::vec2(), uv);

        Output(ShaderDataType::vec2(), fUv);

        Texture(layerColor, TEXTURE_2D, RGBA);
        Texture(layerDepth, TEXTURE_2D, DEPTH);

        fUv = uv;

        setVertexPosition(vec4(position, 1));

        return BuildShader();
    }

    Shader CompositingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT);

        Input(ShaderDataType::vec2(), fUv);

        Output(ShaderDataType::vec4(), color);

        Texture(layerColor, TEXTURE_2D, RGBA);
        Texture(layerDepth, TEXTURE_2D, DEPTH);

        // TODO: Implement supersampling
        color = texture(layerColor, fUv);

        setFragmentDepth(texture(layerDepth, fUv).x());

        return BuildShader();
    }
}
