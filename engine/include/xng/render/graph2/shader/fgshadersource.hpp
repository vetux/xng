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

#ifndef XENGINE_FGSHADERSOURCE_HPP
#define XENGINE_FGSHADERSOURCE_HPP

#include "xng/render/graph2/shader/fgshaderoperation.hpp"
#include "xng/render/graph2/shader/fgvertexlayout.hpp"

namespace xng {
    /**
     * FGShaderSource represents a standalone set of instructions to run on the gpu.
     *
     * The Runtime generates shader source code from the data in FGShaderSource.
     */
    class FGShaderSource {
    public:
        enum ShaderStage {
            VERTEX,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage;

        FGVertexLayout inputLayout;
        FGVertexLayout outputLayout;

        std::vector<FGShaderOperation> operations;
    };
}
#endif //XENGINE_FGSHADERSOURCE_HPP
