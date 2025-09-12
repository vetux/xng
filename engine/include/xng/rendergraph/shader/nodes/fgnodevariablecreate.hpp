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

#ifndef XENGINE_FGNODEVARIABLECREATE_HPP
#define XENGINE_FGNODEVARIABLECREATE_HPP

#include "xng/rendergraph/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeVariableCreate final : FGShaderNode {
        std::string variableName;
        FGShaderValue type;
        size_t count;

        std::unique_ptr<FGShaderNode> value;

        FGNodeVariableCreate(const std::string &variable_name,
                             const FGShaderValue &type,
                             const size_t count,
                             std::unique_ptr<FGShaderNode> value)
            : variableName(variable_name),
              type(type),
              count(count),
              value(std::move(value)) {
        }

        NodeType getType() const override {
            return VARIABLE_CREATE;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeVariableCreate>(variableName, type, count, value ? value->copy() : nullptr);
        }
    };
}

#endif //XENGINE_FGNODEVARIABLECREATE_HPP
