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

#include "xng/graphics/3d/passes/canvasrenderpass.hpp"


#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/texfilter.hpp"

using namespace xng::ShaderScript;

namespace xng {
    Shader CanvasRenderPass::createVertexShader() {
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

    Shader CanvasRenderPass::createFragmentShader() {
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

        INPUT_ATTRIBUTE(fPosition)
        INPUT_ATTRIBUTE(fUv)

        OUTPUT_ATTRIBUTE(color)

        BUFFER_ELEMENT(vars, color)
        BUFFER_ELEMENT(vars, colorMixFactor)
        BUFFER_ELEMENT(vars, alphaMixFactor)
        BUFFER_ELEMENT(vars, colorFactor)
        BUFFER_ELEMENT(vars, texAtlasLevel)
        BUFFER_ELEMENT(vars, texAtlasIndex)
        BUFFER_ELEMENT(vars, texFilter)
        BUFFER_ELEMENT(vars, uvOffset_uvScale)
        BUFFER_ELEMENT(vars, atlasScale_texSize)

        TEXTURE_ARRAY_SAMPLER(atlasTextures, vars_texAtlasLevel, atlasTexture)

        builder.If(vars_texAtlasIndex >= 0);
        {
            vec2 uv = fUv.xy();
            uv = uv * vars_uvOffset_uvScale.zw();
            uv = uv + vars_uvOffset_uvScale.xy();
            uv = uv * vars_atlasScale_texSize.xy();

            vec4 texColor;
            texColor = vec4(0, 0, 0, 0);
            If(vars_texFilter == 1);
            {
                texColor = textureBicubic(atlasTexture,
                                          vec3(uv.x(), uv.y(), vars_texAtlasIndex),
                                          vars_atlasScale_texSize.zw());
            }
            Else();
            {
                texColor = texture(atlasTexture, vec3(uv.x(), uv.y(), vars_texAtlasIndex));
            }
            EndIf();
            If(vars_colorFactor != 0);
            {
                color = vars_color * texColor;
            }
            Else();
            {
                vec4 buffColor;
                buffColor = vars_color.xyzw();
                color.xyz() = mix(texColor.xyz(), buffColor.xyz(), vars_colorMixFactor);
                color.w() = mix(texColor.w(), buffColor.w(), vars_alphaMixFactor);
            }
            EndIf();
        }
        builder.Else();
        {
            color = vars_color;
        }
        builder.EndIf();

        return builder.build();
    }
}

