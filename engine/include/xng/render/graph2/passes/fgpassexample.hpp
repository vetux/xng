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

#ifndef XENGINE_FGPASSEXAMPLE_HPP
#define XENGINE_FGPASSEXAMPLE_HPP

#include "xng/render/graph2/fgbuilder.hpp"
#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

namespace xng {
    class FGPassExample {
    public:
        std::string vbName;
        std::string ibName;
        std::string vsName;
        std::string fsName;

        void setup(FGBuilder &builder) {
            FGResource vb;
            FGResource ib;
            if (vbName.empty()) {
                vb = builder.createVertexBuffer(1);
                ib = builder.createIndexBuffer(1);
                vbName = "vb";
                ibName = "ib";
                builder.exportResource(vbName, vb);
                builder.exportResource(ibName, ib);
            } else {
                vb = builder.importResource(vbName);
                ib = builder.importResource(ibName);
            }

            FGResource vs;
            FGResource fs;
            if (vsName.empty()) {
                vs = builder.createShader(createVertexShader());
                fs = builder.createShader(createFragmentShader());
                vsName = "vs";
                fsName = "fs";
                builder.exportResource(vsName, vs);
                builder.exportResource(fsName, fs);
            } else {
                vs = builder.importResource(vsName);
                fs = builder.importResource(fsName);
            }

            builder.addPass("Example Pass", [vb, ib, vs, fs](FGContext &ctx) {
                // ctx.upload(vb, ....

                ctx.bindVertexBuffer(vb);
                ctx.bindIndexBuffer(ib);
                ctx.bindShaders({vs, fs});

                ctx.draw(FGDrawCall());
            });
        }

        FGShaderSource createVertexShader() {
            // Shader can be dynamically changed at runtime
            auto builder = FGShaderBuilder();

            // Example arithmetic
            auto varA = builder.variable("varA",
                                         FGShaderVariable::SINGLE,
                                         FGShaderVariable::FLOAT,
                                         10);
            auto varB = builder.variable("varB",
                                         FGShaderVariable::SINGLE,
                                         FGShaderVariable::FLOAT,
                                         5);

            auto varC = builder.variable("varC",
                             FGShaderVariable::VECTOR3,
                             FGShaderVariable::FLOAT,
                             Vec3f(0, 0, 0));

            builder.add(varA, varB, builder.getX(varC));

            auto varArr = builder.array(10);
            for (auto i = 0; i < 10; i = i + 1) {
                auto element = builder.subscript(varArr, i);
                builder.add(varA, varB, element);
            }

            builder.assign(varA, builder.subscript(varArr, varB));

            // Example vertex transformation
            auto vPos = builder.variable("vPos",
                                         FGShaderVariable::VECTOR3,
                                         FGShaderVariable::FLOAT);
            builder.readAttribute(0, vPos);

            auto mvp = builder.variable("mvp",
                                        FGShaderVariable::MAT4,
                                        FGShaderVariable::FLOAT);
            builder.readParameter("mvp", mvp);

            auto fPos = builder.variable("fPos",
                                         FGShaderVariable::VECTOR4,
                                         FGShaderVariable::FLOAT);

            builder.assign(builder.getX(fPos), builder.getX(vPos));
            builder.assign(builder.getY(fPos), builder.getY(vPos));
            builder.assign(builder.getZ(fPos), builder.getZ(vPos));
            builder.assign(builder.getW(fPos), 1);

            // Apply transformation and store result in fPos
            builder.multiply(mvp, fPos, fPos);

            // Write out the attribute for the fragment shader
            builder.writeAttribute(0, fPos);

            return builder.build(FGShaderSource::VERTEX, {}, {});
        }

        FGShaderSource createFragmentShader() {
            auto builder = FGShaderBuilder();
            return builder.build(FGShaderSource::FRAGMENT, {}, {});
        }
    };
}
#endif //XENGINE_FGPASSEXAMPLE_HPP
