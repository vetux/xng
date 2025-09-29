/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
#include "xng/rendergraph/rendergraphpipeline.hpp"

namespace xng {
    class RenderGraphContext;

    /**
     * A graph represents an encapsulated rendering operation.
     *
     * The only resource shareable between graphs is the screen texture.
     */
    struct RenderGraph {
        std::vector<RenderGraphPass> passes;

        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> vertexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> indexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> shaderBufferAllocation;
        std::unordered_map<RenderGraphResource, RenderGraphTexture, RenderGraphResourceHash> textureAllocation;
        std::unordered_map<RenderGraphResource, RenderGraphPipeline, RenderGraphResourceHash> pipelineAllocation;
        std::unordered_map<RenderGraphResource, RenderGraphResource, RenderGraphResourceHash> inheritedResources;

        RenderGraphResource backBufferColor;
        RenderGraphResource backBufferDepthStencil;

        RenderGraph() = default;

        RenderGraph(const std::vector<RenderGraphPass> &passes,
                    const std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> &
                    vertex_buffer_allocation,
                    const std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> &
                    index_buffer_allocation,
                    const std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> &
                    shader_buffer_allocation,
                    const std::unordered_map<RenderGraphResource, RenderGraphTexture, RenderGraphResourceHash> &
                    texture_allocation,
                    const std::unordered_map<RenderGraphResource, RenderGraphPipeline, RenderGraphResourceHash> &
                    pipeline_allocation,
                    const std::unordered_map<RenderGraphResource, RenderGraphResource, RenderGraphResourceHash> &
                    inherited_resources,
                    const RenderGraphResource backBufferColor,
                    const RenderGraphResource backBufferDepthStencil)
            : passes(passes),
              vertexBufferAllocation(vertex_buffer_allocation),
              indexBufferAllocation(index_buffer_allocation),
              shaderBufferAllocation(shader_buffer_allocation),
              textureAllocation(texture_allocation),
              pipelineAllocation(pipeline_allocation),
              inheritedResources(inherited_resources),
              backBufferColor(backBufferColor),
              backBufferDepthStencil(backBufferDepthStencil) {
        }
    };
}

#endif //XENGINE_RENDERGRAPH_HPP
