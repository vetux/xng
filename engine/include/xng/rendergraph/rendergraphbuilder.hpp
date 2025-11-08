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

#ifndef XENGINE_RENDERGRAPHBUILDER_HPP
#define XENGINE_RENDERGRAPHBUILDER_HPP

#include <string>
#include <functional>

#include "xng/rendergraph/rendergraphpipeline.hpp"
#include "xng/rendergraph/rendergraph.hpp"
#include "xng/rendergraph/rendergraphcontext.hpp"
#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/rendergraph/rendergraphtexture.hpp"
#include "xng/rendergraph/shader/shader.hpp"

namespace xng {
    class XENGINE_EXPORT RenderGraphBuilder {
    public:
        typedef size_t PassHandle;

        explicit RenderGraphBuilder(const Vec2i &backBufferSize);

        ~RenderGraphBuilder() = default;

        /**
         * Declare a previously allocated resource to be inherited by this graph.
         *
         * The specified resource must have been allocated or inherited in the
         * preceding RenderGraphRuntime.compile() or RenderGraphRuntime.recompile() call.
         *
         * @param resource The resource to inherit
         * @return The new resource handle representing the inherited resource
         */
        RenderGraphResource inheritResource(RenderGraphResource resource);

        RenderGraphResource createVertexBuffer(size_t size);

        RenderGraphResource createIndexBuffer(size_t size);

        RenderGraphResource createShaderBuffer(size_t size);

        RenderGraphResource createShaderBuffer(const ShaderStruct &type);

        RenderGraphResource createTexture(const RenderGraphTexture &texture);

        /**
         * Create a render pipeline from the given shader resources.
         *
         * All shaders in a pipeline are required to have identical buffer / texture / parameter layouts and compatible
         * input / output attribute layouts.
         *
         * @param pipeline
         * @return
         */
        RenderGraphResource createPipeline(const RenderGraphPipeline &pipeline);

        const Vec2i &getBackBufferSize() const;

        /**
         * The returned texture has an RGBA color format.
         *
         * @return The resource handle representing the back buffer color texture
         */
        RenderGraphResource getBackBufferColor() const;

        /**
         * The returned texture has an DEPTH_STENCIL color format.
         *
         * @return The resource handle representing the back buffer depth stencil texture
         */
        RenderGraphResource getBackBufferDepthStencil() const;

        PassHandle addPass(const std::string &name, std::function<void(RenderGraphContext &)> pass);

        void read(PassHandle pass, RenderGraphResource resource);

        void write(PassHandle pass, RenderGraphResource resource);

        void readWrite(PassHandle pass, RenderGraphResource resource);

        RenderGraph build();

    private:
        RenderGraphResource createResource();

        int resourceCounter;

        std::vector<RenderGraphPass> passes;

        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> vertexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> indexBufferAllocation;
        std::unordered_map<RenderGraphResource, size_t, RenderGraphResourceHash> shaderBufferAllocation;
        std::unordered_map<RenderGraphResource, RenderGraphTexture, RenderGraphResourceHash> textureAllocation;
        std::unordered_map<RenderGraphResource, RenderGraphPipeline, RenderGraphResourceHash> pipelineAllocation;

        std::unordered_map<RenderGraphResource, RenderGraphResource, RenderGraphResourceHash> inheritedResources;

        RenderGraphResource backBufferColor;
        RenderGraphResource backBufferDepthStencil;

        Vec2i backBufferSize;
    };
}

#endif //XENGINE_RENDERGRAPHBUILDER_HPP
