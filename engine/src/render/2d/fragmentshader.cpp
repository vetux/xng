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

#include "fragmentshader.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng {
    ShaderStage createFragmentShader() {
        const ShaderAttributeLayout inputLayout{
            {
                ShaderDataType::vec4(),
                ShaderDataType::vec2()
            }
        };
        const ShaderAttributeLayout outputLayout{
            {
                ShaderDataType::vec4()
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

        std::vector<ShaderTexture> textures;
        textures.reserve(12);
        for (auto i = 0; i < 12; i++) {
            textures.emplace_back(TEXTURE_2D, RGBA, true);
        }

        auto &builder = ShaderBuilder::instance();
        builder.setup(ShaderStage::VERTEX,
                      inputLayout,
                      outputLayout,
                      {},
                      {{"vars", buf}},
                      textures,
                      {});

        vec4 fPosition = attribute(0);
        vec2 fUv = attribute(1);

        vec4 color = vec4(1, 1, 1, 1);

        builder.If(buffer("vars", "texAtlasIndex") >= 0);

        vec2 uv = fUv;

        vec2 uvOffset_uvScale = buffer("vars", "uvOffset_uvScale");
        uv = uv * vec2(uvOffset_uvScale.z(), uvOffset_uvScale.w());
        uv = uv + vec2(uvOffset_uvScale.x(), uvOffset_uvScale.y());

        vec2 atlasScale_texSize = buffer("vars", "atlasScale_texSize");
        uv = uv * vec2(atlasScale_texSize.x(), atlasScale_texSize.y());

        vec4 texColor = vec4(1, 1, 1, 1);
        builder.If(buffer("vars", "texFilter") == 1);

        builder.Else();

        color = buffer("vars", "color");

        builder.EndIf();

        return builder.build();
    }
}
