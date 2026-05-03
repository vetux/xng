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

#ifndef XENGINE_RENDERGRAPH_INDEXBUFFER_HPP
#define XENGINE_RENDERGRAPH_INDEXBUFFER_HPP

#include "xng/rendergraph/resource/buffer.hpp"

namespace xng::rg {
    struct IndexBuffer : Buffer {
        explicit IndexBuffer(const size_t size,
                             const Capability capabilities = CAPABILITY_TRANSFER_SRC | CAPABILITY_TRANSFER_DST,
                             const MemoryType memoryType = MEMORY_GPU_ONLY)
            : Buffer(size, CAPABILITY_INDEX | capabilities, memoryType) {
        }

        explicit IndexBuffer(const Buffer &other)
            : Buffer(other.size, other.capabilityFlags, other.memoryType) {
            if (!(other.capabilityFlags & CAPABILITY_INDEX)) {
                throw std::runtime_error("IndexBuffer constructor invoked with incompatible Buffer");
            }
        }
    };
}

#endif //XENGINE_RENDERGRAPH_INDEXBUFFER_HPP
