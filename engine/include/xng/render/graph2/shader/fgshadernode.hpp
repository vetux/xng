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

#ifndef XENGINE_FGSHADERNODE_HPP
#define XENGINE_FGSHADERNODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "xng/render/graph2/shader/fgshadernodeinput.hpp"
#include "xng/render/graph2/shader/fgshadernodeoutput.hpp"

namespace xng {
    struct FGShaderNode {
        enum NodeType {
            LITERAL,
            VECTOR,

            ATTRIBUTE_READ,
            ATTRIBUTE_WRITE,

            PARAMETER_READ,

            TEXTURE_SAMPLE,
            TEXTURE_SIZE,

            BUFFER_READ,
            BUFFER_WRITE,

            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,

            EQUAL,
            NEQUAL,
            GREATER,
            LESS,
            GREATER_EQUAL,
            LESS_EQUAL,

            AND,
            OR,

            NORMALIZE,
            //TODO: Add builtin function nodes

            ARRAY_LENGTH,

            SUBSCRIPT, //Array, Vector or matrix indexing

            BRANCH,
            LOOP,
        };

        virtual NodeType getType() = 0;

        virtual const std::vector<FGShaderNodeInput> &getInputs() = 0;

        virtual const std::vector<FGShaderNodeOutput> &getOutputs() = 0;

        virtual ~FGShaderNode() = default;
    };
}

#endif //XENGINE_FGSHADERNODE_HPP
