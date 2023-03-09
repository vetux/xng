/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_FRAMEGRAPHALLOCATOR_HPP
#define XENGINE_FRAMEGRAPHALLOCATOR_HPP

#include "framegraph.hpp"
#include "framegraphallocation.hpp"

namespace xng {
    /**
     * The FrameGraphAllocator controls how render objects created and used by passes are stored in memory.
     *
     * Currently the gpu abstraction does not allow low level control over where the buffers are allocated themselves.
     *
     * Once this is implemented advanced allocator implementations can use this api to allocate the buffers in the most
     * efficient way possible.
     *
     * The passes implement the memory management for data inside the buffers.
     */
    class XENGINE_EXPORT FrameGraphAllocator {
    public:
        virtual ~FrameGraphAllocator() = default;

        /**
         * Must be called before calling allocateNextPass.
         *
         * Allocators can use the graph data to allocate the objects in the most efficient way possible.
         *
         * @param frame The frame graph that contains the resource allocation information
         */
        virtual void beginFrame(const FrameGraph &frame) = 0;

        /**
         * Allocate the set of resources for the next pass and deallocate any unused resources.
         *
         * @throws std::runtime_error when there are no more pass resources to allocate.
         * @return The set of resources to pass to the current pass execute method.
         */
        virtual FrameGraphPassResources allocateNextPass() = 0;
    };
}

#endif //XENGINE_FRAMEGRAPHALLOCATOR_HPP
