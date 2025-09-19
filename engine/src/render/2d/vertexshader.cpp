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

#include "vertexshader.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng {
    Shader createVertexShader() {
        const ShaderAttributeLayout inputLayout{
            {
                {"position", ShaderDataType::vec2()},
                {"uv", ShaderDataType::vec2()}
            }
        };

        const ShaderAttributeLayout outputLayout{
            {
                {"fPosition", ShaderDataType::vec4()},
                {"fUv", ShaderDataType::vec2()}
            }
        };

        const ShaderBuffer buf{
            true,
            false,
            {
                {"color", ShaderDataType::vec4()},
                {"colorMixFactor", ShaderDataType::float32()},
                {"alphaMixFactor", ShaderDataType::float32()},
                {"colorFactor", ShaderDataType::float32()},
                {"texAtlasLevel", ShaderDataType::integer()},
                {"texAtlasIndex", ShaderDataType::integer()},
                {"texFilter", ShaderDataType::integer()},
                {"mvp", ShaderDataType::mat4()},
                {"uvOffset_uvScale", ShaderDataType::vec4()},
                {"atlasScale_texSize", ShaderDataType::vec4()},
                {"_padding", ShaderDataType::float32()},
            }
        };

        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::VERTEX,
                      inputLayout,
                      outputLayout,
                      {},
                      {{"vars", buf}},
                      {{"atlasTextures", ShaderTextureArray(ShaderTexture(TEXTURE_2D, RGBA, true), 12)}},
                      {});

        INPUT_ATTRIBUTE(position)
        INPUT_ATTRIBUTE(uv)

        OUTPUT_ATTRIBUTE(fPosition)
        OUTPUT_ATTRIBUTE(fUv)

        BUFFER_ELEMENT(vars, mvp)

        fPosition = vars_mvp * vec4(position.x(), position.y(), 0.0, 1.0);
        fUv = uv;

        setVertexPosition(fPosition);

        return builder.build();
    }
}
