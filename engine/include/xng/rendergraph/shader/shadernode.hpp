/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SHADERNODE_HPP
#define XENGINE_SHADERNODE_HPP

#include <memory>

#include <vector>

namespace xng {
    struct Shader;

    struct ShaderNode {
        enum NodeType {
            VARIABLE_CREATE = 0,

            ASSIGN,

            VARIABLE,
            LITERAL,
            ARGUMENT,
            ATTRIBUTE_IN,
            ATTRIBUTE_OUT,
            PARAMETER,

            VECTOR,
            MATRIX,
            ARRAY,

            TEXTURE,
            TEXTURE_SAMPLE,
            TEXTURE_FETCH,
            TEXTURE_SIZE,

            BUFFER,
            BUFFER_SIZE,

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

            CALL,
            RETURN,

            BUILTIN,

            VECTOR_SWIZZLE,

            SUBSCRIPT_ARRAY,
            SUBSCRIPT_MATRIX,

            BRANCH,
            LOOP,

            VERTEX_POSITION,
            FRAGMENT_DEPTH,
        };

        virtual NodeType getType() const = 0;

        virtual std::unique_ptr<ShaderNode> copy() const = 0;

        virtual ~ShaderNode() = default;
    };
}

#endif //XENGINE_SHADERNODE_HPP
