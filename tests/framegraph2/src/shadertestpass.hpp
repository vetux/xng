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
#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

using namespace xng;

class ShaderTestPass {
public:
    std::string vsName;
    std::string fsName;

    FGShaderBuffer dataBuffer;

    FGAttributeLayout vertexLayout;
    FGAttributeLayout fragmentLayout;
    FGAttributeLayout colorLayout;

    ShaderTestPass() {
        dataBuffer.elements.emplace_back("mvp", FGShaderValue(FGShaderValue::MAT4, FGShaderValue::FLOAT));
        vertexLayout.elements.emplace_back(FGShaderValue::VECTOR3, FGShaderValue::FLOAT);
        fragmentLayout.elements.emplace_back(FGShaderValue::VECTOR4, FGShaderValue::FLOAT);
        colorLayout.elements.emplace_back(FGShaderValue::VECTOR4, FGShaderValue::FLOAT);
    }

    void setup(FGBuilder &builder) {
        auto vs = builder.createShader(createVertexShader());
        auto fs = builder.createShader(createFragmentShader());
        vsName = "vs";
        fsName = "fs";

        builder.addPass("Example Pass", [vs, fs](FGContext &ctx) {
            const auto vsStr = ctx.getShaderSource(vs);
            const auto fsStr = ctx.getShaderSource(fs);

            std::cout << "Vertex Shader:" << std::endl << vsStr << std::endl;
            std::cout << "Fragment Shader:" << std::endl << fsStr << std::endl;
        });
    }

    FGShaderSource createVertexShader() {
        auto builder = FGShaderBuilder();

        const auto vPos = builder.attributeRead(0);
        const auto mvp = builder.bufferRead("data", "mvp");

        auto fPos = builder.vector(builder.getX(vPos),
                                   builder.getY(vPos),
                                   builder.getZ(vPos),
                                   builder.literal(1));

        fPos = builder.multiply(mvp, fPos);

        builder.attributeWrite(0, fPos);

        return builder.build(FGShaderSource::VERTEX,
            vertexLayout,
            fragmentLayout,
            {},
            {{"data", dataBuffer}},
            {});
    }

    FGShaderSource createFragmentShader() {
        auto builder = FGShaderBuilder();

        const auto fPos = builder.attributeRead(0);
        builder.attributeWrite(0, fPos);

        return builder.build(FGShaderSource::FRAGMENT, fragmentLayout, colorLayout, {}, {}, {});
    }
};
#endif //XENGINE_SHADERTESTPASS_HPP
