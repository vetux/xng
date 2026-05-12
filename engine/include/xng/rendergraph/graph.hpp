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

#ifndef XENGINE_RENDERGRAPH_GRAPH_HPP
#define XENGINE_RENDERGRAPH_GRAPH_HPP

#include <utility>
#include <vector>

#include "xng/rendergraph/pass.hpp"
#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng::rg {
    /**
     * A graph represents an encapsulated rendering operation.
     *
     * Only heap-resources can be shared between graphs.
     *
     * The runtime handles synchronization internally.
     *
     * Write-After-Write and Write-After-Read hazards to the same resource region inside a single pass are deemed undefined behavior.
     * Write-After-Write and Write-After-Read hazards to the same resource region from two separate passes execute in pass declaration order.
     *
     * A resource "region" in this context represents either a range of bytes in a buffer or a texture subresource.
     *
     * The runtime implementations may throw an exception if a WAW/WAR hazard inside a single pass is encountered.
     */
    struct Graph {
        std::vector<Pass> passes{};

        // Transient / Aliased Resource Allocations - Handles cannot be reused on recompile
        std::unordered_map<ResourceId, Buffer, ResourceIdHash> bufferAllocations{};
        std::unordered_map<ResourceId, Texture, ResourceIdHash> textureAllocations{};

        Graph() = default;

        Graph(std::vector<Pass> passes,
              std::unordered_map<ResourceId, Buffer, ResourceIdHash> vertexBufferAllocations,
              std::unordered_map<ResourceId, Texture, ResourceIdHash> textureAllocations)
            : passes(std::move(passes)),
              bufferAllocations(std::move(vertexBufferAllocations)),
              textureAllocations(std::move(textureAllocations)) {
        }
    };
}

#endif //XENGINE_RENDERGRAPH_GRAPH_HPP
