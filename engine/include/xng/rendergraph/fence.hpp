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

#ifndef XENGINE_FENCE_HPP
#define XENGINE_FENCE_HPP

#include <chrono>
#include <cstddef>

#include "xng/rendergraph/timeline.hpp"

namespace xng::rg {
    /**
     * A fence is a cpu awaitable synchronization primitive.
     *
     * It also provides the api for retrieving the timers for a given graph invocation if the runtime has timers enabled.
     *
     * The RenderGraph runtime internally always ensures synchronization correctness where possible.
     *
     * Fences are provided for users to express semantic correctness in the case where the user wants to wait
     * on an operation to finish or check if the operation has finished. (For example, streaming or frames in flight)
     */
    class Fence {
    public:
        virtual ~Fence() = default;

        virtual bool isSignaled() = 0;

        /**
         * @param timeOut Time timeout in nanoseconds.
         * @return Whether the fence was signaled.
         */
        virtual bool wait(size_t timeOut) = 0;

        /**
         * This function will wait on the fence and then return the corresponding timeline.
         *
         * @return The timeline for the execute() invocation that this fence represents.
         */
        virtual const Timeline &getTimeline() = 0;
    };
}

#endif //XENGINE_FENCE_HPP
