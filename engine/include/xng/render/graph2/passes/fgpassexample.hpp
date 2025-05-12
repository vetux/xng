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
        void setup(FGBuilder &builder){
            auto vb = builder.createVertexBuffer();
            auto ib = builder.createIndexBuffer();
            auto vs = builder.createShader(FGShaderBuilder().build(FGShader::VERTEX, {}, {}));
            auto fs = builder.createShader(FGShaderBuilder().build(FGShader::FRAGMENT, {}, {}));

            builder.addPass("Example Pass", [vb, ib, vs, fs](FGContext &ctx){
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
