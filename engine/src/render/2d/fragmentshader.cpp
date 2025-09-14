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

#include "xng/render/shaderlib/texfilter.hpp"

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

        std::vector<ShaderTextureArray> textures;
        textures.reserve(12);
        for (auto i = 0; i < 12; i++) {
            textures.emplace_back(ShaderTexture(TEXTURE_2D, RGBA, true));
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
        {
            vec2 uv = fUv;
            uv = uv * buffer("vars", "uvOffset_uvScale").zw();
            uv = uv + buffer("vars", "uvOffset_uvScale").xy();
            uv = uv * buffer("vars", "atlasScale_texSize").xy();

            /*vec4 texColor;
            If (buffer("vars", "texFilter") == 1);
            {
                textureSampler()
                texColor = textureBicubic(atlasTextures[vars.passes[drawID].texAtlasLevel],
                vec3(uv.x, uv.y, vars.passes[drawID].texAtlasIndex),
                vars.passes[drawID].atlasScale_texSize.zw);
            }
            else
            {
                texColor = texture(atlasTextures[vars.passes[drawID].texAtlasLevel],
                vec3(uv.x, uv.y, vars.passes[drawID].texAtlasIndex));
            }
            if (vars.passes[drawID].colorFactor != 0) {
                color = vars.passes[drawID].color * texColor;
            } else {
                color.rgb = mix(texColor.rgb, vars.passes[drawID].color.rgb, vars.passes[drawID].colorMixFactor);
                color.a = mix(texColor.a, vars.passes[drawID].color.a, vars.passes[drawID].alphaMixFactor);
            }*/
        }
        builder.Else();
        {
            color = buffer("vars", "color");
        }
        builder.EndIf();

        return builder.build();
    }
}
