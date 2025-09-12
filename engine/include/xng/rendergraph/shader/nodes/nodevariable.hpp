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

#ifndef XENGINE_NODEVARIABLE_HPP
#define XENGINE_NODEVARIABLE_HPP

#include <optional>

#include "xng/rendergraph/shader/shadernode.hpp"
#include "xng/rendergraph/shader/nodes/nodevariablecreate.hpp"
#include "xng/rendergraph/shader/nodes/nodeloop.hpp"
#include "xng/rendergraph/shader/nodes/nodebranch.hpp"

#include "xng/util/downcast.hpp"

namespace xng {
    struct NodeVariable final : ShaderNode {
        std::string variableName;

        explicit NodeVariable(const std::string &variable_name)
            : variableName(variable_name) {
        }

        NodeType getType() const override {
            return VARIABLE;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeVariable>(variableName);
        }
    };
}

#endif //XENGINE_NODEVARIABLE_HPP
