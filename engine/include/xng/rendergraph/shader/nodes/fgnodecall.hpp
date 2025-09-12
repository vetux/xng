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

#ifndef XENGINE_FGNODECALL_HPP
#define XENGINE_FGNODECALL_HPP

#include <vector>

#include "xng/rendergraph/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeCall final : FGShaderNode {
        std::string functionName;
        std::vector<std::unique_ptr<FGShaderNode> > arguments;

        FGNodeCall(std::string function_name, std::vector<std::unique_ptr<FGShaderNode> > arguments)
            : functionName(std::move(function_name)),
              arguments(std::move(arguments)) {
        }

        NodeType getType() const override {
            return CALL;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            std::vector<std::unique_ptr<FGShaderNode> > argumentsCopy;
            argumentsCopy.reserve(arguments.size());
            for (auto &node: arguments) {
                argumentsCopy.push_back(node->copy());
            }
            return std::make_unique<FGNodeCall>(functionName, std::move(argumentsCopy));
        }
    };
}

#endif //XENGINE_FGNODECALL_HPP
