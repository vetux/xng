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

#ifndef XENGINE_COMMANDFENCE_HPP
#define XENGINE_COMMANDFENCE_HPP

#include <stdexcept>

#include "xng/gpu/renderobject.hpp"

namespace xng {
    class XENGINE_EXPORT CommandFence : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_FENCE;
        }

        /**
         * The destructor waits for the commands to finish before destroying the fence object.
         */
        ~CommandFence() override = default;

        /**
         * Wait for the commands to finish or return an exception ptr if the task threw an exception.
         * @return
         */
        virtual std::exception_ptr wait() = 0;

        /**
         * @return true if the commands have completed
         */
        virtual bool isComplete() = 0;

        /**
         * @return nullptr or the exception object if an exception was thrown while executing the commands
         */
        virtual std::exception_ptr getException() = 0;
    };
}
#endif //XENGINE_COMMANDFENCE_HPP
