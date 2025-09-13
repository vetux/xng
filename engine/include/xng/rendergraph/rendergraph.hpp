/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERGRAPH_HPP
#define XENGINE_RENDERGRAPH_HPP

#include <vector>

#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/rendergraph/rendergraphpass.hpp"
#include "xng/rendergraph/rendergraphtexture.hpp"
#include "xng/rendergraph/shader/shaderstage.hpp"

namespace xng {
    class RenderGraphContext;

    /**
     * A graph represents an encapsulated rendering operation.
     *
     * The only resource shareable between graphs is the screen texture.
     */
    struct RenderGraph {
        std::vector<RenderGraphPass> passes;

        std::unordered_map<RenderGraphResource, size_t> vertexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t> indexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t> shaderBufferAllocation;

        std::unordered_map<RenderGraphResource, RenderGraphTexture> textureAllocation;

        std::unordered_map<RenderGraphResource, ShaderStage> shaderAllocation;
        std::unordered_map<RenderGraphResource, std::unordered_set<RenderGraphResource> > pipelineAllocation;

        std::unordered_map<RenderGraphResource, RenderGraphResource> inheritedResources;

        RenderGraphResource screenTexture{};

        RenderGraph() = default;

        RenderGraph(const std::vector<RenderGraphPass> &passes,
                const std::unordered_map<RenderGraphResource, size_t> &vertex_buffer_allocation,
                const std::unordered_map<RenderGraphResource, size_t> &index_buffer_allocation,
                const std::unordered_map<RenderGraphResource, size_t> &shader_buffer_allocation,
                const std::unordered_map<RenderGraphResource, RenderGraphTexture> &texture_allocation,
                const std::unordered_map<RenderGraphResource, ShaderStage> &shader_allocation,
                const std::unordered_map<RenderGraphResource, std::unordered_set<RenderGraphResource> > &pipeline_allocation,
                const std::unordered_map<RenderGraphResource, RenderGraphResource> &inherited_resources,
                const RenderGraphResource screen_texture)
            : passes(passes),
              vertexBufferAllocation(vertex_buffer_allocation),
              indexBufferAllocation(index_buffer_allocation),
              shaderBufferAllocation(shader_buffer_allocation),
              textureAllocation(texture_allocation),
              shaderAllocation(shader_allocation),
              pipelineAllocation(pipeline_allocation),
              inheritedResources(inherited_resources),
              screenTexture(screen_texture) {
        }
    };
}

#endif //XENGINE_RENDERGRAPH_HPP
