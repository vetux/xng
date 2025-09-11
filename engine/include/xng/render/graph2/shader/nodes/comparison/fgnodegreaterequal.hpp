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

#ifndef XENGINE_FGNODEGREATEREQUAL_HPP
#define XENGINE_FGNODEGREATEREQUAL_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeGreaterEqual final : FGShaderNode {
        std::unique_ptr<FGShaderNode> left;
        std::unique_ptr<FGShaderNode> right;

        FGNodeGreaterEqual(std::unique_ptr<FGShaderNode> left, std::unique_ptr<FGShaderNode> right)
            : left(std::move(left)),
              right(std::move(right)) {
        }

        NodeType getType() const override {
            return GREATER_EQUAL;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeGreaterEqual>(left->copy(), right->copy());
        }

        FGShaderValue getOutputType(const FGShaderSource &source, const std::string &functionName) const override {
            return {FGShaderValue::SCALAR, FGShaderValue::BOOLEAN, 1};
        }
    };
}

#endif //XENGINE_FGNODEGREATEREQUAL_HPP
