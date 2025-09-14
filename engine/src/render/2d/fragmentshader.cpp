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

        std::vector<ShaderTextureArray> textureArrays;
        textureArrays.emplace_back(ShaderTexture(TEXTURE_2D, RGBA, true), 12);

        auto &builder = ShaderBuilder::instance();
        builder.setup(ShaderStage::FRAGMENT,
                      inputLayout,
                      outputLayout,
                      {},
                      {{"vars", buf}},
                      textureArrays,
                      {});

        shaderlib::textureBicubic();

        vec4 fPosition;
        fPosition = attribute(0);
        vec2 fUv;
        fUv = attribute(1);

        vec4 color;
        color = vec4(1, 1, 1, 1);

        builder.If(buffer("vars", "texAtlasIndex") >= 0);
        {
            vec2 uv = fUv;
            uv = uv * buffer("vars", "uvOffset_uvScale").zw();
            uv = uv + buffer("vars", "uvOffset_uvScale").xy();
            uv = uv * buffer("vars", "atlasScale_texSize").xy();

            vec4 texColor;
            If(buffer("vars", "texFilter") == 1);
            {
                texColor = textureBicubic(textureSampler(0, buffer("vars", "texAtlasLevel")),
                                          vec3(uv.x(), uv.y(), buffer("vars", "texAtlasIndex")),
                                          buffer("vars", "atlasScale_texSize").zw());
            }
            Else();
            {
                texColor = texture(textureSampler(0, buffer("vars", "texAtlasLevel")),
                                   vec3(uv.x(), uv.y(), buffer("vars", "texAtlasIndex")));
            }
            EndIf();
            If(buffer("vars", "colorFactor") != 0);
            {
                color = buffer("vars", "color") * texColor;
            }
            Else();
            {
                vec4 buffColor;
                buffColor = buffer("vars", "color");
                color.xyz() = mix(texColor.xyz(), buffColor.xyz(), buffer("vars", "colorMixFactor"));
                color.w() = mix(texColor.w(), buffColor.w(), buffer("vars", "alphaMixFactor"));
            }
            EndIf();
        }
        builder.Else();
        {
            color = buffer("vars", "color");
        }
        builder.EndIf();

        writeAttribute(0, color);

        return builder.build();
    }
}
