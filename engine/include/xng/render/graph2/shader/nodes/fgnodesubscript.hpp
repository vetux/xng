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

#ifndef XENGINE_FGNODESUBSCRIPT_HPP
#define XENGINE_FGNODESUBSCRIPT_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeSubscript final : FGShaderNode {
        FGShaderNodeInput value = FGShaderNodeInput("value");
        FGShaderNodeInput row = FGShaderNodeInput("row");
        FGShaderNodeInput column = FGShaderNodeInput("column");

        FGShaderNodeOutput output = FGShaderNodeOutput("output");

        NodeType getType() override {
            return SUBSCRIPT;
        }

        std::vector<std::reference_wrapper<FGShaderNodeInput>> getInputs() override {
            return {value, row, column};
        }

        std::vector<std::reference_wrapper<FGShaderNodeOutput>> getOutputs() override {
            return {output};
        }

        FGShaderValue getOutputType(const FGShaderSource &source) const override {
            auto valueType = value.source->getOutputType(source);
            if (valueType.count > 1) {
                return {valueType.type, valueType.component, 1};
            } else {
                return {FGShaderValue::SCALAR, value.source->getOutputType(source).component, 1};
            }
        }
    };
}

#endif //XENGINE_FGNODESUBSCRIPT_HPP