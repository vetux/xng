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
                vs = builder.createShader(FGShaderBuilder().build(FGShaderSource::VERTEX, {}, {}));
                fs = builder.createShader(FGShaderBuilder().build(FGShaderSource::FRAGMENT, {}, {}));
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
                ctx.bindShader(vs);
                ctx.bindShader(fs);

                ctx.draw(FGDrawCall());
            });
        }
    };
}
#endif //XENGINE_FGPASSEXAMPLE_HPP
