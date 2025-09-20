/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::VERTEX,
                      getVertexLayout(),
                      ShaderAttributeLayout({
                          {"fUv", ShaderDataType::vec2()},
                      }),
                      {},
                      {},
                      {
                          {"layerColor", ShaderTextureArray(ShaderTexture(TEXTURE_2D, RGBA, false))},
                          {"layerDepth", ShaderTextureArray(ShaderTexture(TEXTURE_2D, DEPTH, false))},
                      },
                      {});

        INPUT_ATTRIBUTE(position)
        INPUT_ATTRIBUTE(uv)

        OUTPUT_ATTRIBUTE(fUv)

        fUv = uv;

        setVertexPosition(vec4(position, 1));

        return builder.build();
    }

    Shader CompositingPass::createFragmentShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::FRAGMENT,
                      ShaderAttributeLayout({
                          {"fUv", ShaderDataType::vec2()},
                      }),
                      ShaderAttributeLayout({
                          {"color", ShaderDataType::vec4()}
                      }),
                      {},
                      {},
                      {
                          {"layerColor", ShaderTextureArray(ShaderTexture(TEXTURE_2D, RGBA, false))},
                          {"layerDepth", ShaderTextureArray(ShaderTexture(TEXTURE_2D, DEPTH, false))},
                      },
                      {});

        INPUT_ATTRIBUTE(fUv)

        OUTPUT_ATTRIBUTE(color)

        color = texture(textureSampler("layerColor"), fUv);

        setFragmentDepth(texture(textureSampler("layerDepth"), fUv).x());

        return builder.build();
    }
}
