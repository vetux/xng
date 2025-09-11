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
#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

using namespace xng;

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
        auto builder = FGShaderBuilder();

        const auto vPos = builder.attributeRead(0);
        const auto mvp = builder.bufferRead("data", "mvp");

        std::vector<std::unique_ptr<FGShaderNode> > body;

        body.emplace_back(builder.createVariable("fPos",
                                                 FGShaderValue(FGShaderValue::VECTOR4, FGShaderValue::FLOAT),
                                                 builder.vector(builder.getX(vPos),
                                                                builder.getY(vPos),
                                                                builder.getZ(vPos),
                                                                builder.literal(1.0f))));
        body.emplace_back(builder.assignVariable("fPos", builder.multiply(mvp, builder.variable("fPos"))));

        FGBranchBuilder branch;
        branch.If(builder.compareEqual(builder.getX(vPos), builder.literal(1.0f)));
        branch.add(builder.assignVariable("fPos", builder.multiply(builder.variable("fPos"),
                                                                   builder.literal(2.0f))));
        branch.Else();

        FGBranchBuilder nestedBranch;
        nestedBranch.If(builder.compareGreater(builder.getX(vPos), builder.literal(1.0f)));
        nestedBranch.add(builder.assignVariable("fPos", builder.vector(builder.getX(vPos),
                                                                       builder.getY(vPos),
                                                                       builder.literal(1.0f),
                                                                       builder.literal(1.0f))));
        nestedBranch.Else();
        nestedBranch.add(builder.assignVariable("fPos", builder.vector(builder.getX(vPos),
                                                                       builder.getY(vPos),
                                                                       builder.literal(5.0f),
                                                                       builder.literal(5.0f))));

        nestedBranch.EndIf();

        branch.add(nestedBranch.build(builder));

        branch.EndIf();

        body.emplace_back(branch.build(builder));

        body.emplace_back(builder.attributeWrite(0, builder.variable("fPos")));

        return builder.build(FGShaderSource::VERTEX,
                             vertexLayout,
                             fragmentLayout,
                             {},
                             {{"color", colorBuffer}, {"data", dataBuffer}},
                             {{"texture", texture}},
                             body);
    }

    FGShaderSource createFragmentShader() {
        auto builder = FGShaderBuilder();

        std::vector<std::unique_ptr<FGShaderNode> > body;
        body.emplace_back(builder.attributeWrite(0, builder.attributeRead(0)));

        return builder.build(FGShaderSource::FRAGMENT,
                             fragmentLayout,
                             colorLayout,
                             {},
                             {{"data", dataBuffer}, {"color", colorBuffer}},
                             {{"texture", texture}},
                             body);
    }
};
#endif //XENGINE_SHADERTESTPASS_HPP
