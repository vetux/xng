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

#ifndef XENGINE_FGNODEVECTOR_HPP
#define XENGINE_FGNODEVECTOR_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    /**
     * Construct a vector from the given components.
     *
     * The vector size depends on the assigned inputs,
     * for example, if x,y,z is assigned, the resulting vector is a Vector3.
     */
    struct FGNodeVector final : FGShaderNode {
        FGShaderNodeInput x = FGShaderNodeInput("x");
        FGShaderNodeInput y = FGShaderNodeInput("y");
        FGShaderNodeInput z = FGShaderNodeInput("z");
        FGShaderNodeInput w = FGShaderNodeInput("w");

        FGShaderNodeOutput vector = FGShaderNodeOutput("vector");

        NodeType getType() override {
            return VECTOR;
        }

        const std::vector<FGShaderNodeInput> &getInputs() override {
            return {x, y, z, w};
        }

        const std::vector<FGShaderNodeOutput> &getOutputs() override {
            return {vector};
        }
    };
}

#endif //XENGINE_FGNODEVECTOR_HPP
