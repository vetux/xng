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

#ifndef XENGINE_FGNODEDIVIDE_HPP
#define XENGINE_FGNODEDIVIDE_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeDivide final : FGShaderNode {
        std::unique_ptr<FGShaderNode> left;
        std::unique_ptr<FGShaderNode> right;

        FGNodeDivide(std::unique_ptr<FGShaderNode> left, std::unique_ptr<FGShaderNode> right)
            : left(std::move(left)),
              right(std::move(right)) {
        }

        NodeType getType() const override {
            return DIVIDE;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeDivide>(left->copy(), right->copy());
        }

        FGShaderValue getOutputType(const FGShaderSource &source, const std::string &functionName) const override {
            auto leftType = left->getOutputType(source, functionName);
            auto rightType = right->getOutputType(source, functionName);
            if (leftType.type == rightType.type) {
                return leftType;
            } else {
                // Mixed Arithmetic eg vec3 / float
                if (leftType.type == FGShaderValue::SCALAR) {
                    return rightType;
                } else {
                    return leftType;
                }
            }
        }
    };
}

#endif //XENGINE_FGNODEDIVIDE_HPP
