/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_COMMANDQUEUE_HPP
#define XENGINE_COMMANDQUEUE_HPP

#include "xng/gpu/commandbuffer.hpp"

namespace xng {
    /**
     * A queue executes command buffers in submission order,
     * commands inside of command buffers are executed in order as well
     * which means the driver must implement ordering in graphics apis that require
     * manual synchronization of commands.
     *
     * CommandBuffers in different queues can execute in parallel.
     * Users can synchronize submissions between queues using semaphores.
     */
    class CommandQueue : public RenderObject {
    public:
        ~CommandQueue() override = default;

        /**
         *
         * @param buffer
         * @param waitSemaphores
         * @param signalSemaphores
         * @return
         */
        virtual std::unique_ptr<CommandFence> submit(const std::vector<std::reference_wrapper<CommandBuffer>> &buffers,
                                                     const std::vector<std::shared_ptr<CommandSemaphore>> &waitSemaphores,
                                                     const std::vector<std::shared_ptr<CommandSemaphore>> &signalSemaphores) = 0;
    };
}

#endif //XENGINE_COMMANDQUEUE_HPP
