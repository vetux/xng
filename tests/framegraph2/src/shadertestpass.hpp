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

#include "xng/render/graph2/fgbuilder.hpp"
#include "xng/render/graph2/shader/fgbranchbuilder.hpp"
#include "xng/render/graph2/shader/fgloopbuilder.hpp"
#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

using namespace xng;
using namespace xng::FGShaderFactory;

class ShaderTestPass {
public:
    FGShaderBuffer dataBuffer;
    FGShaderBuffer colorBuffer;

    FGTexture texture;

    FGAttributeLayout vertexLayout;
    FGAttributeLayout fragmentLayout;
    FGAttributeLayout colorLayout;

    ShaderTestPass() {
        dataBuffer.elements.emplace_back("mvp", FGShaderValue(FGShaderValue::MAT4, FGShaderValue::FLOAT));
        colorBuffer.elements.emplace_back("color", FGShaderValue(FGShaderValue::VECTOR4, FGShaderValue::FLOAT));
        vertexLayout.elements.emplace_back(FGShaderValue::VECTOR3, FGShaderValue::FLOAT);
        fragmentLayout.elements.emplace_back(FGShaderValue::VECTOR4, FGShaderValue::FLOAT);
        colorLayout.elements.emplace_back(FGShaderValue::VECTOR4, FGShaderValue::FLOAT);
    }

    void setup(FGBuilder &builder) {
        auto vs = builder.createShader(createVertexShader());
        auto fs = builder.createShader(createFragmentShader());
        auto pip = builder.createPipeline({vs, fs});

        builder.addPass("Example Pass", [pip](FGContext &ctx) {
            const auto src = ctx.getShaderSource(pip);
            std::cout << "----------Vertex Shader----------" << std::endl << src.at(FGShaderSource::VERTEX) <<
                    std::endl;
            std::cout << "----------Fragment Shader----------" << std::endl << src.at(FGShaderSource::FRAGMENT) <<
                    std::endl;
        });
    }

    FGShaderSource createVertexShader() {
        const auto vPos = attributeRead(0);
        const auto mvp = bufferRead("data", "mvp");

        std::vector<std::unique_ptr<FGShaderNode> > body;

        body.emplace_back(createVariable("fPos",
                                         FGShaderValue(FGShaderValue::VECTOR4, FGShaderValue::FLOAT),
                                         vector(FGShaderValue::vec4(),
                                                getX(vPos),
                                                getY(vPos),
                                                getZ(vPos),
                                                literal(1.0f))));
        body.emplace_back(assignVariable("fPos",
                                         multiply(mvp, variable("fPos"))));

        FGBranchBuilder branch;
        branch.If(compareEqual(getX(vPos), literal(1.0f)));
        branch.add(assignVariable("fPos",
                                  multiply(
                                      variable("fPos"),
                                      literal(2.0f))));
        branch.Else();

        FGBranchBuilder nestedBranch;
        nestedBranch.If(compareGreater(getX(vPos), literal(1.0f)));
        nestedBranch.add(assignVariable("fPos", vector(FGShaderValue::vec4(),
                                                       getX(vPos),
                                                       getY(vPos),
                                                       literal(1.0f),
                                                       literal(1.0f))));
        nestedBranch.Else();
        nestedBranch.add(assignVariable("fPos",
                                        vector(FGShaderValue::vec4(),
                                               getX(vPos),
                                               getY(vPos),
                                               literal(5.0f),
                                               literal(5.0f))));

        FGLoopBuilder loop;
        loop.loopFor("i", literal(0), literal(10));

        loop.add(assignVariable("fPos",
                                add(variable("fPos"),
                                    vector(FGShaderValue::vec4(),
                                           variable("i"),
                                           variable("i"),
                                           variable("i"),
                                           variable("i")))));

        loop.endLoop();

        nestedBranch.add(loop.build());

        nestedBranch.EndIf();

        branch.add(nestedBranch.build());

        branch.EndIf();

        body.emplace_back(branch.build());

        body.emplace_back(attributeWrite(0, variable("fPos")));

        return FGShaderBuilder().build(FGShaderSource::VERTEX,
                                       vertexLayout,
                                       fragmentLayout,
                                       {{"color", FGShaderValue::vec4()}},
                                       {{"colorBuffer", colorBuffer}, {"data", dataBuffer}},
                                       {{"texture", texture}},
                                       body);
    }

    FGShaderSource createFragmentShader() {
        std::vector<std::unique_ptr<FGShaderNode> > body;
        body.emplace_back(createVariable("color",
                                         FGShaderValue::vec4(),
                                         bufferRead("colorBuffer", "color")));
        body.emplace_back(assignVariable("color",
                                         multiply(
                                             variable("color"),
                                             parameterRead("color"))));
        body.emplace_back(attributeWrite(0, variable("color")));

        return FGShaderBuilder().build(FGShaderSource::FRAGMENT,
                                       fragmentLayout,
                                       colorLayout,
                                       {{"color", FGShaderValue::vec4()}},
                                       {{"data", dataBuffer}, {"colorBuffer", colorBuffer}},
                                       {{"texture", texture}},
                                       body);
    }
};
#endif //XENGINE_SHADERTESTPASS_HPP
