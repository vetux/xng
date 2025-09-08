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

#ifndef XENGINE_FGNODEEQUAL_HPP
#define XENGINE_FGNODEEQUAL_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeEqual final : FGShaderNode {
        FGShaderNodeInput left = FGShaderNodeInput("left");
        FGShaderNodeInput right = FGShaderNodeInput("right");

        FGShaderNodeOutput result = FGShaderNodeOutput("result");

        NodeType getType() override {
            return EQUAL;
        }

        std::vector<std::reference_wrapper<FGShaderNodeInput>> getInputs() override {
            return {left, right};
        }

        std::vector<std::reference_wrapper<FGShaderNodeOutput>> getOutputs() override {
            return {result};
        }

        FGShaderValue getOutputType(const FGShaderSource &source) override {
            return left.source->getOutputType(source);
        }
    };
}

#endif //XENGINE_FGNODEEQUAL_HPP