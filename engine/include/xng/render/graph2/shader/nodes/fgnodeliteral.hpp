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

#ifndef XENGINE_FGNODELITERAL_HPP
#define XENGINE_FGNODELITERAL_HPP

#include <utility>

#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeLiteral final : FGShaderNode {
        FGShaderLiteral value;

        FGShaderNodeOutput output = FGShaderNodeOutput("output");

        explicit FGNodeLiteral(FGShaderLiteral value)
            : value(std::move(value)) {
        }

        NodeType getType() override {
            return LITERAL;
        }

        std::vector<std::reference_wrapper<FGShaderNodeInput>> getInputs() override {
            return {};
        }

        std::vector<std::reference_wrapper<FGShaderNodeOutput>> getOutputs() override {
            return {output};
        }

        FGShaderValue getOutputType(const FGShaderSource &source) override {
            return getLiteralType(value);
        }
    };
}

#endif //XENGINE_FGNODELITERAL_HPP