/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "render/graph/framegraph.hpp"
#include "render/graph/framegraphallocation.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphAllocator {
    public:
        /**
         * Must be called before calling allocateNext.
         *
         * Allocators can use the graph data to allocate the objects in the most efficient way possible.
         * For example by creating the gpu memory buffers adjacent in memory (Not possible yet with the current gpu abstraction)
         *
         * @param frame The frame graph that contains the resource allocations
         */
        virtual void setFrame(const FrameGraph &frame) = 0;

        /**
         * Allocate the set of resources for the next pass and deallocate any unused resources.
         *
         * @throws std::runtime_error when there are no more pass resources to allocate.
         * @return The set of resources to pass to the current pass execute method.
         */
        virtual FrameGraphPassResources allocateNext() = 0;
    };
}

#endif //XENGINE_FRAMEGRAPHALLOCATOR_HPP
