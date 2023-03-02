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

#ifndef XENGINE_GPUFENCE_HPP
#define XENGINE_GPUFENCE_HPP

#include <stdexcept>

namespace xng {
    /**
     * A fence represents a task which is executed on the gpu device and can be awaited from the cpu.
     * The gpu driver implementation can batch together tasks for optimal performance.
     */
    class XENGINE_EXPORT GpuFence {
    public:
        /**
         * The destructor waits for the gpu task to finish before destroying the fence object.
         */
        virtual ~GpuFence() = default;

        /**
         * Wait for the gpu task to finish or return an exception ptr if the task threw an exception.
         * @return
         */
        virtual std::exception_ptr wait() = 0;

        /**
         * @return true if the task has completed
         */
        virtual bool isComplete() = 0;

        /**
         * @return nullptr or the exception object if an exception was thrown in the gpu task
         */
        virtual std::exception_ptr getException() = 0;
    };
}
#endif //XENGINE_GPUFENCE_HPP
