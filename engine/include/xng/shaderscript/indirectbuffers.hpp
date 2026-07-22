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

#ifndef XENGINE_INDIRECTBUFFERS_HPP
#define XENGINE_INDIRECTBUFFERS_HPP

#include "xng/shaderscript/macro/shaderstruct.hpp"

namespace xng::ShaderScript {
    ShaderStruct(ShaderDrawIndirectArray,
                 unsigned int, vertexCount,
                 unsigned int, instanceCount,
                 unsigned int, firstVertex,
                 unsigned int, baseInstance)

        ShaderStruct(ShaderDrawIndirectIndexed,
                     unsigned int, indexCount,
                     unsigned int, instanceCount,
                     unsigned int, firstIndex,
                     int, baseVertex,
                     unsigned int, baseInstance,
                     unsigned int, padding0,
                     unsigned int, padding1,
                     unsigned int, padding2)

    ShaderStruct(ShaderComputeIndirect,
                 unsigned int, x,
                 unsigned int, y,
                 unsigned int, z,
                 unsigned int, padding0)
}

#endif //XENGINE_INDIRECTBUFFERS_HPP
