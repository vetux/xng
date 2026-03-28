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

#ifndef XENGINE_RENDERGRAPH_VERTEXBUFFER_HPP
#define XENGINE_RENDERGRAPH_VERTEXBUFFER_HPP

#include <stdexcept>

#include "xng/rendergraph/resource/buffer.hpp"

namespace xng::rendergraph {
    struct VertexBuffer : Buffer {
        explicit VertexBuffer(const size_t size,
                              const Capability capabilities = CAPABILITY_TRANSFER_SRC | CAPABILITY_TRANSFER_DST,
                              const MemoryType memoryType = MEMORY_GPU_ONLY)
            : Buffer(size, CAPABILITY_VERTEX | capabilities, memoryType) {
        }

        explicit VertexBuffer(const Buffer &other)
            : Buffer(other.size, other.capabilityFlags, other.memoryType) {
            if (!(other.capabilityFlags & CAPABILITY_VERTEX)) {
                throw std::runtime_error("VertexBuffer constructor invoked with incompatible Buffer");
            }
        }
    };
}

#endif //XENGINE_RENDERGRAPH_VERTEXBUFFER_HPP
