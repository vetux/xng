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

#ifndef XENGINE_FGNODEMULTIPLY_HPP
#define XENGINE_FGNODEMULTIPLY_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeMultiply final : FGShaderNode {
        std::unique_ptr<FGShaderNode> left;
        std::unique_ptr<FGShaderNode> right;

        FGNodeMultiply(std::unique_ptr<FGShaderNode> left, std::unique_ptr<FGShaderNode> right)
            : left(std::move(left)),
              right(std::move(right)) {
        }

        NodeType getType() const override {
            return MULTIPLY;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeMultiply>(left->copy(), right->copy());
        }

        FGShaderValue getOutputType(const FGShaderSource &source, const std::string &functionName) const override {
            auto leftType = left->getOutputType(source, functionName);
            auto rightType = right->getOutputType(source, functionName);
            if (leftType.type == rightType.type) {
                return leftType;
            } else {
                // Mixed Arithmetic

                // Matrix * Vector or Vector * Matrix
                if (leftType.type == FGShaderValue::MAT4 || rightType.type == FGShaderValue::MAT4) {
                    return {FGShaderValue::VECTOR4, leftType.component, 1};
                } else if (leftType.type == FGShaderValue::MAT3 || rightType.type == FGShaderValue::MAT3) {
                    return {FGShaderValue::VECTOR3, leftType.component, 1};
                } else if (leftType.type == FGShaderValue::MAT2 || rightType.type == FGShaderValue::MAT2) {
                    return {FGShaderValue::VECTOR2, leftType.component, 1};
                }

                // Scalar * Vector
                if (leftType.type == FGShaderValue::SCALAR) {
                    return rightType;
                } else {
                    return leftType;
                }
            }
        }
    };
}

#endif //XENGINE_FGNODEMULTIPLY_HPP