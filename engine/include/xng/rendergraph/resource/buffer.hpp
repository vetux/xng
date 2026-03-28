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

#ifndef XENGINE_BUFFER_HPP
#define XENGINE_BUFFER_HPP

#include <cstdint>
#include <cstddef>

namespace xng::rendergraph {
    /**
     * Buffers are storage blobs that can be used as Vertex / Index or Storage buffers.
     *
     * Users can choose between explicitly managing Buffer allocations or using the type safe wrappers / methods.
     *
     * Buffer resources are downcast-ed in the bind stage of the RasterContext
     * to enforce that the required capabilities are present.
     */
    struct Buffer {
        /**
         * Capability describes what binding points a buffer can be bound to
         * and whether a buffer can be used  in transfer operations.
         *
         * Bitmask.
         */
        enum Capability : uint32_t {
            CAPABILITY_NONE         = 0,
            CAPABILITY_VERTEX       = 1,
            CAPABILITY_INDEX        = 2,
            CAPABILITY_STORAGE      = 4,
            CAPABILITY_TRANSFER_SRC = 8,
            CAPABILITY_TRANSFER_DST = 16,
        };

        /**
         * MEMORY_CPU_TO_GPU and MEMORY_GPU_TO_CPU buffers can be mapped into cpu visible memory.
         */
        enum MemoryType {
            MEMORY_GPU_ONLY = 0, // E.g. Vulkan DEVICE_LOCAL
            MEMORY_CPU_TO_GPU, // E.g. Vulkan HOST_VISIBLE | HOST_COHERENT
            MEMORY_GPU_TO_CPU, // E.g. Vulkan HOST_VISIBLE | HOST_CACHED
        };

        size_t size = 0;
        Capability capabilityFlags = CAPABILITY_NONE;
        MemoryType memoryType = MEMORY_GPU_ONLY;

        Buffer() = default;

        Buffer(const size_t size, const Capability capabilityFlags, const MemoryType memoryType)
            : size(size), capabilityFlags(capabilityFlags), memoryType(memoryType) {
        }
    };

    inline Buffer::Capability operator|(const Buffer::Capability a, const Buffer::Capability b) {
        return static_cast<Buffer::Capability>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    class BufferHash {
    public:
        std::size_t operator()(const Buffer &buffer) const {
            size_t ret = 0;
            hash_combine(ret, buffer.size);
            hash_combine(ret, buffer.capabilityFlags);
            hash_combine(ret, buffer.memoryType);
            return ret;
        }
    };
}

#endif //XENGINE_BUFFER_HPP
