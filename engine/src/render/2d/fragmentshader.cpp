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
    Shader createFragmentShader() {
        const ShaderAttributeLayout inputLayout{
            {
                {"fPosition", ShaderDataType::vec4()},
                {"fUv", ShaderDataType::vec2()}
            }
        };
        const ShaderAttributeLayout outputLayout{
            {
                {"color", ShaderDataType::vec4()}
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
        builder.setup(Shader::FRAGMENT,
                      inputLayout,
                      outputLayout,
                      {},
                      {{"vars", buf}},
                      {{"atlasTextures", ShaderTextureArray{ShaderTexture(TEXTURE_2D, RGBA, true), 12}}},
                      {});

        shaderlib::textureBicubic();

        vec4 fPosition;
        fPosition = readAttribute("fPosition");
        vec2 fUv;
        fUv = readAttribute("fUv");

        vec4 color;
        color = vec4(1, 1, 1, 1);

        builder.If(readBuffer("vars", "texAtlasIndex") >= 0);
        {
            vec2 uv = fUv;
            uv = uv * readBuffer("vars", "uvOffset_uvScale").zw();
            uv = uv + readBuffer("vars", "uvOffset_uvScale").xy();
            uv = uv * readBuffer("vars", "atlasScale_texSize").xy();

            vec4 texColor;
            If(readBuffer("vars", "texFilter") == 1);
            {
                texColor = textureBicubic(textureSampler("atlasTextures", readBuffer("vars", "texAtlasLevel")),
                                          vec3(uv.x(), uv.y(), readBuffer("vars", "texAtlasIndex")),
                                          readBuffer("vars", "atlasScale_texSize").zw());
            }
            Else();
            {
                texColor = texture(textureSampler("atlasTextures", readBuffer("vars", "texAtlasLevel")),
                                   vec3(uv.x(), uv.y(), readBuffer("vars", "texAtlasIndex")));
            }
            EndIf();
            If(readBuffer("vars", "colorFactor") != 0);
            {
                color = readBuffer("vars", "color") * texColor;
            }
            Else();
            {
                vec4 buffColor;
                buffColor = readBuffer("vars", "color");
                color.xyz() = mix(texColor.xyz(), buffColor.xyz(), readBuffer("vars", "colorMixFactor"));
                color.w() = mix(texColor.w(), buffColor.w(), readBuffer("vars", "alphaMixFactor"));
            }
            EndIf();
        }
        builder.Else();
        {
            color = readBuffer("vars", "color");
        }
        builder.EndIf();

        writeAttribute("color", color);

        return builder.build();
    }
}
