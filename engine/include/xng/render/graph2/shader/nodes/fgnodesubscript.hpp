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
    /**
     * When subscripting vectors, the row must contain a literal specifying the index of the element eg (.x = 0)
     */
    struct FGNodeSubscript final : FGShaderNode {
        std::unique_ptr<FGShaderNode> value;
        std::unique_ptr<FGShaderNode> row;
        std::unique_ptr<FGShaderNode> column;

        FGNodeSubscript(std::unique_ptr<FGShaderNode> value,
                        std::unique_ptr<FGShaderNode> row,
                        std::unique_ptr<FGShaderNode> column)
            : value(std::move(value)),
              row(std::move(row)),
              column(std::move(column)) {
        }

        NodeType getType() const override {
            return SUBSCRIPT;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeSubscript>(value->copy(), row->copy(), column ? column->copy() : nullptr);
        }

        FGShaderValue getOutputType(const FGShaderSource &source, const std::string &functionName) const override {
            auto valueType = value->getOutputType(source, functionName);
            if (valueType.count > 1) {
                return {valueType.type, valueType.component, 1};
            } else {
                return {FGShaderValue::SCALAR, value->getOutputType(source, functionName).component, 1};
            }
        }
    };
}

#endif //XENGINE_FGNODESUBSCRIPT_HPP
