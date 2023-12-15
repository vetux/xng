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

#ifndef XENGINE_FRAMEGRAPHPASS_HPP
#define XENGINE_FRAMEGRAPHPASS_HPP

#include "xng/util/genericmap.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    class FrameGraphBuilder;

    /**
     * A frame graph pass defines the rendering logic to be executed later by the runtime.
     *
     * Passes can depend on other pass data through FrameGraphSlots.
     */
    class XENGINE_EXPORT FrameGraphPass {
    public:
        virtual ~FrameGraphPass() = default;

        /**
         * Define the resource allocations and usages for the next execute invocation.
         *
         *
         * @param builder
         */
        virtual void setup(FrameGraphBuilder &builder) = 0;

        /**
         * The returned type name is used for defining dependencies between passes.
         *
         * @return The type index of the concrete pass type
         */
        virtual std::type_index getTypeIndex() const = 0;
    };
}

#endif //XENGINE_FRAMEGRAPHPASS_HPP
