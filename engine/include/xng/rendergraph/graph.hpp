/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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
     * Only heap resources can be shared between graphs.
     * The runtime synchronizes heap accesses between graphs.
     *
     * Write-After-Write conflicts for both heap and transient resources are deemed undefined behavior.
     * The runtime implementations may throw an exception if WAW is encountered or may perform Last-Write-Wins
     * based on submission order.
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
