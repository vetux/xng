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

#ifndef XENGINE_FGNODEVARIABLEREAD_HPP
#define XENGINE_FGNODEVARIABLEREAD_HPP

#include <optional>

#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/shader/nodes/fgnodevariablecreate.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeloop.hpp"
#include "xng/render/graph2/shader/nodes/fgnodebranch.hpp"

#include "xng/util/downcast.hpp"

namespace xng {
    struct FGNodeVariableRead final : FGShaderNode {
        std::string variableName;

        explicit FGNodeVariableRead(const std::string &variable_name)
            : variableName(variable_name) {
        }

        NodeType getType() const override {
            return VARIABLE_READ;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeVariableRead>(variableName);
        }
    };
}

#endif //XENGINE_FGNODEVARIABLEREAD_HPP
