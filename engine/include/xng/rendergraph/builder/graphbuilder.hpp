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

#ifndef XENGINE_RENDERGRAPH_BUILDER_HPP
#define XENGINE_RENDERGRAPH_BUILDER_HPP

#include "xng/rendergraph/graph.hpp"
#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"

#include "xng/rendergraph/builder/transferpassbuilder.hpp"
#include "xng/rendergraph/builder/graphicspassbuilder.hpp"
#include "xng/rendergraph/builder/computepassbuilder.hpp"

namespace xng::rg {
    /**
     * The graph builder can be used to simplify graph recording.
     *
     * The Pass Builders expose interfaces to define resource usages cleanly.
     */
    class XENGINE_EXPORT GraphBuilder {
    public:
        explicit GraphBuilder()
            : resourceCounter(0) {
        }

        Resource<Buffer> allocateBuffer(const Buffer &desc) {
            if (desc.memoryType != Buffer::MEMORY_GPU_ONLY)
                throw std::runtime_error("Transient buffer allocations must be MEMORY_GPU_ONLY");
            Resource ret(createResource(), desc);
            bufferAllocations.insert({ret, static_cast<Buffer>(desc)});
            return ret;
        }

        Resource<Texture> allocateTexture(const Texture &desc) {
            Resource ret(createResource(), desc);
            textureAllocations.insert({ret, static_cast<Texture>(desc)});
            return ret;
        }

        GraphBuilder &addPass(TransferPass pass) {
            passes.emplace_back(std::move(pass));
            return *this;
        }

        GraphBuilder &addPass(GraphicsPass pass) {
            passes.emplace_back(std::move(pass));
            return *this;
        }

        GraphBuilder &addPass(ComputePass pass) {
            passes.emplace_back(std::move(pass));
            return *this;
        }

        Graph build() {
            return Graph{std::move(passes), std::move(bufferAllocations), std::move(textureAllocations)};
        }

    private:
        ResourceId::Handle createResource() {
            return resourceCounter++;
        }

        ResourceId::Handle resourceCounter;

        std::vector<Pass> passes;

        std::unordered_map<ResourceId, Buffer, ResourceIdHash> bufferAllocations;
        std::unordered_map<ResourceId, Texture, ResourceIdHash> textureAllocations;
    };
}

#endif //XENGINE_RENDERGRAPH_BUILDER_HPP
