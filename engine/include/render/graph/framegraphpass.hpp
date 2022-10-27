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

#ifndef XENGINE_FRAMEGRAPHPASS_HPP
#define XENGINE_FRAMEGRAPHPASS_HPP

#include "render/graph/framegraphpassresources.hpp"
#include "types/genericmap.hpp"

#include "gpu/renderdevice.hpp"

namespace xng {
    class FrameGraphBuilder;

    class XENGINE_EXPORT FrameGraphPass {
    public:
        virtual ~FrameGraphPass() = default;

        /**
         * Declare the dependencies of the execute call.
         *
         * @param builder
         */
        virtual void setup(FrameGraphBuilder &builder, const GenericMapString &properties) = 0;

        /**
         * Run the pass.
         * Resources created previously in the setup() call can be accessed in the resources object.
         * Data stored in the sharedData map is passed on to child passes.
         *
         * @param resources
         * @param sharedData
         */
        virtual void execute(FrameGraphPassResources &resources, GenericMapString &sharedData) = 0;

        /**
         * The returned type name is used for defining dependencies between passes.
         *
         * @return The type index of the concrete pass type
         */
        virtual std::type_index getTypeName() = 0;
    };
}

#endif //XENGINE_FRAMEGRAPHPASS_HPP
