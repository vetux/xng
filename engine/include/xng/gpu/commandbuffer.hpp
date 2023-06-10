/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_COMMANDBUFFER_HPP
#define XENGINE_COMMANDBUFFER_HPP

#include "xng/gpu/command.hpp"

#include "xng/gpu/commandsemaphore.hpp"

namespace xng {
    class CommandBuffer : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_COMMAND_BUFFER;
        }

        /**
         * Begin recording a new set of commands into the command buffer
         */
        virtual void begin() = 0;

        /**
         * @param commands
         * @return
         */
        virtual CommandBuffer &add(const std::vector<Command> &commands) = 0;

        CommandBuffer &add(const Command &command) {
            return add(std::vector<Command>{command});
        }

        virtual void end() = 0;
    };
}

#endif //XENGINE_COMMANDBUFFER_HPP
