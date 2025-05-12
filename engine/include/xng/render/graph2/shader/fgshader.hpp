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

#ifndef XENGINE_FGSHADER_HPP
#define XENGINE_FGSHADER_HPP

#include "xng/render/graph2/shader/fgshaderoperation.hpp"
#include "xng/render/graph2/buffer/fgbufferlayout.hpp"

namespace xng {
    typedef int FGShaderHandle;

    /**
     * FGShader represents a standalone set of instructions to run on the gpu.
     *
     * The Runtime generates shader source code from the data in FGShader.
     */
    class FGShader {
    public:
        enum ShaderStage {
            VERTEX,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage;

        FGBufferLayout inputLayout;
        FGBufferLayout outputLayout;

        std::vector<FGShaderOperation> operations;
    };
}
#endif //XENGINE_FGSHADER_HPP
