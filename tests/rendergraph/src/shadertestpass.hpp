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

#ifndef XENGINE_SHADERTESTPASS_HPP
#define XENGINE_SHADERTESTPASS_HPP

#include <iostream>
#include <ostream>

#include "xng/rendergraph/rendergraphbuilder.hpp"
#include "xng/rendergraph/shaderscript/shaderscript.hpp"

#include "xng/graphics/shaderlib/noise.hpp"

using namespace xng;
using namespace xng::ShaderScript;

class ShaderTestPass {
public:
    ShaderBuffer dataBuffer;
    ShaderBuffer colorBuffer;

    ShaderTexture textureDef{};

    ShaderAttributeLayout vertexLayout;
    ShaderAttributeLayout fragmentLayout;
    ShaderAttributeLayout colorLayout;

    ShaderTestPass() {
        dataBuffer.elements.emplace_back("mvp", ShaderDataType(ShaderDataType::MAT4, ShaderDataType::FLOAT));
        colorBuffer.elements.emplace_back("albedo", ShaderDataType(ShaderDataType::VECTOR4, ShaderDataType::FLOAT));
        colorBuffer.dynamic = true;
        vertexLayout.addElement("position", ShaderDataType::vec3());
        fragmentLayout.addElement("fPosition", ShaderDataType::vec4());
        colorLayout.addElement("color", ShaderDataType::vec4());
    }

    void setup(RenderGraphBuilder &builder) {
        auto pip = builder.createPipeline({{createVertexShader(), createFragmentShader()}});

        builder.addPass("Example Pass", [pip](RenderGraphContext &ctx) {
            const auto src = ctx.getShaderSource(pip);
            std::cout << "----------Vertex Shader----------" << std::endl << src.at(Shader::VERTEX) <<
                    std::endl;
            std::cout << "----------Fragment Shader----------" << std::endl << src.at(Shader::FRAGMENT) <<
                    std::endl;
        });
    }

    Shader createVertexShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::VERTEX,
                      vertexLayout,
                      fragmentLayout,
                      {},
                      {{"data", dataBuffer},  {"color", colorBuffer}},
                      {{"texture", ShaderTextureArray(textureDef)}},
                      {});

        shaderlib::noise::simplex();

        Function("test",
                 {{"texArg", textureDef}},
                 ShaderDataType::float32());
        {
            ARGUMENT(texArg)
            Return(5 * (3 + texture(texArg, vec2(0.5f, 0.5f)).x()));
        }
        EndFunction();

        // Equivalent to int b[4] = {1, 2, 3, 4}
        ArrayInt<4> b = ArrayInt<4>(std::vector<Int>{1, 2, 3, 4});

        Float r = Call("test", textureSampler("texture"));

        Float v = simplex(vec2(0.5f, 0.5f));

        vec2 f;
        f = vec2(5.0f, 1.0f);
        f = f * r;

        vec4 color;
        color = texture(textureSampler("texture"), vec2(0.5f, 0.5f));

        If(r == 5);
        {
            f = f + vec2(1.0f, 1.0f);
        }
        Else();
        {
            Int i;
            i = Int(0);
            For(i, 0, 10, 1);
            {
                If(i == 1);
                {
                    f = 3 * (f + vec2(i, i));
                }
                EndIf();
            }
            EndFor();
        }
        EndIf();

        If(true);
        {
            Float p = f.x();
            p = 3 * (p + color.y());
            f.x() = p;
        }
        EndIf();

        INPUT_ATTRIBUTE(position)
        OUTPUT_ATTRIBUTE(fPosition)

        BUFFER_ELEMENT(data, mvp)

        UInt size = getDynamicBufferLength("color");
        Int i;
        i = Int(0);
        For(i, 0, size, 1);
        {
            DYNAMIC_BUFFER_ELEMENT(color, albedo, i)
            vec4 albedo;
            albedo = color_albedo.xyzw();
            color_albedo = albedo * 5;
        }
        EndFor();

        vec4 fPos;
        fPos = vec4(position.x(), position.y(), position.z(), Float(1));
        fPos = data_mvp * fPos;
        fPos = fPos * vec4(f.x(), f.y(), 0, 1);

        data_mvp = matrix(fPos, fPos, fPos, fPos);

        fPos.x() = fPos.x() + color.x();

        fPosition = fPos;

        return builder.build();
    }

    Shader createFragmentShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(Shader::FRAGMENT,
                      vertexLayout,
                      colorLayout,
                      {},
                      {{"data", dataBuffer},  {"color", colorBuffer}},
                      {{"texture", ShaderTextureArray(textureDef)}},
                      {});

        writeAttribute("color", vec4(1, 0, 0, 1));

        return builder.build();
    }
};
#endif //XENGINE_SHADERTESTPASS_HPP
