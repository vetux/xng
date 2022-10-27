/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPH_HPP
#define XENGINE_FRAMEGRAPH_HPP

#include "render/graph/framegraphresource.hpp"
#include "render/graph/framegraphpass.hpp"

namespace xng {
    struct FrameGraph {
        struct PassExecution {
            std::type_index pass = typeid(FrameGraphPass); // The pass to call execute() on
            std::set<FrameGraphResource> allocations; // The set of resources created by the pass
            std::set<FrameGraphResource> writes; // The set of resources that are written to by the pass
            std::set<FrameGraphResource> reads; // The set of resources that are read by the pass
            std::vector<PassExecution> childPasses; // The set of child passes

            PassExecution() = default;
        };

        struct PassAllocation {
            RenderObject::Type objectType;
            bool isUri;
            std::variant<Uri,
                    std::pair<FrameGraphResource, RenderPipelineDesc>,
                    TextureBufferDesc,
                    ShaderBufferDesc,
                    std::pair<Vec2i, int>> allocationData;
        };

        std::vector<PassExecution> passExecutions; // The set of passes without specified dependencies
        std::map<FrameGraphResource, PassAllocation> allocatedObjects; // The map of render objects that were allocated for this frame identified by their resource id
    };
}

#endif //XENGINE_FRAMEGRAPH_HPP
