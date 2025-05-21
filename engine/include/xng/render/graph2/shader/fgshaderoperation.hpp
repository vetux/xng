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

#ifndef XENGINE_FGSHADEROPERATION_HPP
#define XENGINE_FGSHADEROPERATION_HPP

#include <vector>
#include <cstddef>
#include <variant>
#include <memory>

#include "xng/render/graph2/shader/fgshadervariable.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

namespace xng {
    struct FGShaderOperation {
        enum Op {
            VERTEX_READ,
            VERTEX_WRITE,

            BUFFER_READ,
            BUFFER_WRITE,

            TEXTURE_READ,
            TEXTURE_WRITE,

            ASSIGN,

            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,

            COMPARE_EQUAL,
            COMPARE_LARGER,
            COMPARE_SMALLER,
            COMPARE_LARGER_EQUAL,
            COMPARE_SMALLER_EQUAL,

            LOGICAL_AND,
            LOGICAL_OR,

            NORMALIZE,
            TRANSPOSE,
            INVERSE,
            //....

            SAMPLE,
            SUBSCRIPT, //Array, Vector or matrix indexing

            CONDITIONAL,
            LOOP,
        } operation;

        FGShaderVariable result;
        std::vector<FGShaderVariable> inputs;

        std::vector<FGShaderOperation> predicate;

        std::vector<FGShaderOperation> nextBranch;
        std::vector<FGShaderOperation> alternativeBranch;

        std::vector<FGShaderOperation> loopInitializer;
        std::vector<FGShaderOperation> loopIterator;
        std::vector<FGShaderOperation> loopBody;

        std::string targetName;
        uint32_t targetBinding;

        size_t index;
    };
}

#endif //XENGINE_FGSHADEROPERATION_HPP
