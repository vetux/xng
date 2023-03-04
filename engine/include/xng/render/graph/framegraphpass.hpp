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

#ifndef XENGINE_FRAMEGRAPHPASS_HPP
#define XENGINE_FRAMEGRAPHPASS_HPP

#include "framegraphpassresources.hpp"
#include "xng/types/genericmap.hpp"

#include "xng/gpu/renderdevice.hpp"

#define SHARED_PROPERTY(pass, name) constexpr static const char *const name = #pass "::" #name;

namespace xng {
    class FrameGraphBuilder;

    /**
     * A frame graph pass executes rendering logic.
     *
     * A framework for creating resources is provided through the builder interface passed in the setup stage.
     * The resources can then be retrieved in the execution stage in the passed resources object.
     *
     * Passes can depend on other pass data.
     * Resource handles and other data can be shared between passes through the sharedData map passed in the setup stage.
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
         * Run the pass.
         * Resources created or declared as read / write previously in the setup() call can be accessed in the resources object.
         *
         * @param resources
         */
        virtual void execute(FrameGraphPassResources &resources) = 0;

        /**
         * The returned type name is used for defining dependencies between passes.
         *
         * @return The type index of the concrete pass type
         */
        virtual std::type_index getTypeIndex() = 0;
    };
}

#endif //XENGINE_FRAMEGRAPHPASS_HPP
