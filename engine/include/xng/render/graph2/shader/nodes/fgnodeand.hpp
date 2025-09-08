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

#ifndef XENGINE_FGNODEAND_HPP
#define XENGINE_FGNODEAND_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeAnd final : FGShaderNode {
        FGShaderNodeInput left = FGShaderNodeInput("left");
        FGShaderNodeInput right = FGShaderNodeInput("right");

        FGShaderNodeOutput result = FGShaderNodeOutput("result");

        NodeType getType() override {
            return AND;
        }

        std::vector<std::reference_wrapper<FGShaderNodeInput>> getInputs() override {
            return {left, right};
        }

        std::vector<std::reference_wrapper<FGShaderNodeOutput>> getOutputs() override {
            return {result};
        }

        FGShaderValue getOutputType(const FGShaderSource &source) override {
            return {FGShaderValue::SCALAR, FGShaderValue::BOOLEAN, 1};
        }
    };
}

#endif //XENGINE_FGNODEAND_HPP
