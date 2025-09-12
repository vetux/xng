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

#include "xng/render/shaderlib/noise.hpp"

using namespace xng;
using namespace xng::ShaderScript;

class ShaderTestPass {
public:
    ShaderBuffer dataBuffer;
    ShaderBuffer colorBuffer;

    RenderGraphTexture textureDef;

    ShaderAttributeLayout vertexLayout;
    ShaderAttributeLayout fragmentLayout;
    ShaderAttributeLayout colorLayout;

    ShaderTestPass() {
        dataBuffer.elements.emplace_back("mvp", ShaderDataType(ShaderDataType::MAT4, ShaderDataType::FLOAT));
        colorBuffer.elements.emplace_back("color", ShaderDataType(ShaderDataType::VECTOR4, ShaderDataType::FLOAT));
        vertexLayout.elements.emplace_back(ShaderDataType::VECTOR3, ShaderDataType::FLOAT);
        fragmentLayout.elements.emplace_back(ShaderDataType::VECTOR4, ShaderDataType::FLOAT);
        colorLayout.elements.emplace_back(ShaderDataType::VECTOR4, ShaderDataType::FLOAT);
    }

    void setup(RenderGraphBuilder &builder) {
        auto vs = builder.createShader(createVertexShader());
        auto fs = builder.createShader(createFragmentShader());
        auto pip = builder.createPipeline({vs, fs});

        builder.addPass("Example Pass", [pip](RenderGraphContext &ctx) {
            const auto src = ctx.getShaderSource(pip);
            std::cout << "----------Vertex Shader----------" << std::endl << src.at(ShaderStage::VERTEX) <<
                    std::endl;
            std::cout << "----------Fragment Shader----------" << std::endl << src.at(ShaderStage::FRAGMENT) <<
                    std::endl;
        });
    }

    ShaderStage createVertexShader() {
        std::unordered_map<std::string, ShaderFunction> funcs;
        funcs["simplex2D"] = shaderlib::noise::simplex2D();

        auto &builder = ShaderBuilder::instance();
        builder.setup(ShaderStage::VERTEX,
                      vertexLayout,
                      vertexLayout,
                      {},
                      {{"data", dataBuffer}},
                      {{"tex", textureDef}},
                      funcs);

        builder.Function("test",
                         {{"a", ShaderDataType::integer()}},
                         ShaderDataType::integer());
        Return(5 * (3 + argument("a")));
        builder.EndFunction();

        // Equivalent to int b[4] = {1, 2, 3, 4}
        ArrayInt<4> b = ArrayInt<4>{1, 2, 3, 4};

        Int r = Call("test", {1});
        Int a = r;

        Int v = Call("simplex2D", {vec2(0.5f, 0.5f)});

        vec2 f = vec2(5.0f, 1.0f, 1.0f, 1.0f);
        f = f * a;

        vec4 color = texture("tex", vec2(0.5f, 0.5f));

        builder.If(a == 5);

        f = f + vec2(1.0f, 1.0f, 1.0f, 1.0f);

        builder.Else();

        Int i = 0;
        builder.For(i, 0, i < 10, i + 1);

        builder.If(i == 1);

        f = 3 * (f + vec2(i, i, i, i));

        builder.EndIf();

        builder.EndFor();

        builder.EndIf();

        builder.If(true);

        Float p = f.x();
        p = 3 * (p + color.y());
        f.setX(p);

        builder.EndIf();

        mat4 mvp = buffer("data", "mvp");
        vec3 vPos = attribute(0);
        vec4 fPos = vec4(vPos.x(), vPos.y(), vPos.z(), Float(1));
        fPos = mvp * fPos;
        fPos = fPos * vec4(f.x(), f.y(), 0, 1);

        fPos.setX(fPos.x() + color.x());

        writeAttribute(0, fPos);

        return builder.build();
    }

    ShaderStage createFragmentShader() {
        auto &builder = ShaderBuilder::instance();
        builder.setup(ShaderStage::FRAGMENT,
                      vertexLayout,
                      vertexLayout,
                      {},
                      {{"data", dataBuffer}},
                      {{"tex", textureDef}},
                      {});

        writeAttribute(0, vec4(1, 0, 0, 1));

        return builder.build();
    }
};
#endif //XENGINE_SHADERTESTPASS_HPP
