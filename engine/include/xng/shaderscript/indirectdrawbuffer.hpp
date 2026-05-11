/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_INDIRECTDRAWBUFFER_HPP
#define XENGINE_INDIRECTDRAWBUFFER_HPP

#include "xng/shaderscript/macro/shaderstruct.hpp"

namespace xng::ShaderScript {
    ShaderStruct(ShaderDrawIndirectArray,
                 unsigned int, vertexCount, // The number of vertices to draw
                 unsigned int, instanceCount, // The number of instances to draw
                 unsigned int, firstVertex, // The index of the first vertex
                 unsigned int, baseInstance) // The index of the first instance to draw (Can be accessed via GetBaseInstance)

    ShaderStruct(ShaderDrawIndirectIndexed,
                 unsigned int, indexCount, // The number of indices to draw
                 unsigned int, instanceCount, // The number of instances to draw
                 unsigned int, firstIndex, // The index of the first index
                 int, baseVertex, // The value added to each index in the buffer
                 unsigned int, baseInstance) // The index of the first instance to draw (Can be accessed via GetBaseInstance)
}

#endif //XENGINE_INDIRECTDRAWBUFFER_HPP
