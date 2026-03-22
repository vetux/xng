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

#ifndef XENGINE_RENDERGRAPH_BUILDER_HPP
#define XENGINE_RENDERGRAPH_BUILDER_HPP

#include "xng/rendergraph/graph.hpp"
#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"
#include "xng/rendergraph/resource/indexbuffer.hpp"
#include "xng/rendergraph/resource/vertexbuffer.hpp"
#include "xng/rendergraph/resource/storagebuffer.hpp"

#include "xng/rendergraph/builder/transferpassbuilder.hpp"
#include "xng/rendergraph/builder/rasterpassbuilder.hpp"
#include "xng/rendergraph/builder/computepassbuilder.hpp"

namespace xng::rendergraph {
    /**
     * The graph builder can be used to simplify graph recording.
     *
     * The Pass Builders expose interfaces to define resource usages cleanly.
     */
    class XENGINE_EXPORT GraphBuilder {
    public:
        explicit GraphBuilder(Resource<Texture> backBufferColor, Resource<Texture> backBufferDepthStencil)
            : resourceCounter(0),
              backBufferColor(std::move(backBufferColor)),
              backBufferDepthStencil(std::move(backBufferDepthStencil)) {
        }

        Resource<Buffer> allocateBuffer(const Buffer &desc) {
            if (desc.memoryType != Buffer::MEMORY_GPU_ONLY)
                throw std::runtime_error("Transient buffer allocations must be MEMORY_GPU_ONLY");
            Resource ret(createResource(), desc);
            bufferAllocations.insert({ret, static_cast<Buffer>(desc)});
            return ret;
        }

        Resource<VertexBuffer> allocateVertexBuffer(const VertexBuffer &desc) {
            return static_cast<Resource<VertexBuffer>>(allocateBuffer(desc));
        }

        Resource<IndexBuffer> allocateIndexBuffer(const IndexBuffer &desc) {
            return static_cast<Resource<IndexBuffer>>(allocateBuffer(desc));
        }

        Resource<StorageBuffer> allocateStorageBuffer(const StorageBuffer &desc) {
            return static_cast<Resource<StorageBuffer>>(allocateBuffer(desc));
        }

        Resource<Texture> allocateTexture(const Texture &desc) {
            Resource ret(createResource(), desc);
            textureAllocations.insert({ret, static_cast<Texture>(desc)});
            return ret;
        }

        /**
         * The returned texture has an RGBA color format.
         *
         * @return The resource handle representing the back buffer color texture
         */
        [[nodiscard]] Resource<Texture> getBackBufferColor() const {
            return backBufferColor;
        }

        /**
         * The returned texture has an DEPTH_STENCIL color format.
         *
         * @return The resource handle representing the back buffer depth stencil texture
         */
        [[nodiscard]] Resource<Texture> getBackBufferDepthStencil() const {
            return backBufferDepthStencil;
        }

        TransferPassBuilder addTransferPass(std::string name) {
            return {
                std::move(name),
                [this](TransferPass &&pass) {
                    this->passes.emplace_back(std::move(pass));
                }
            };
        }

        RasterPassBuilder addRasterPass(std::string name) {
            return {
                std::move(name),
                [this](RasterPass &&pass) {
                    this->passes.emplace_back(std::move(pass));
                }
            };
        }

        ComputePassBuilder addComputePass(std::string name) {
            return {
                std::move(name),
                [this](ComputePass &&pass) {
                    this->passes.emplace_back(std::move(pass));
                }
            };
        }

        Graph build() {
            return Graph{std::move(passes), std::move(bufferAllocations), std::move(textureAllocations)};
        }

    private:
        ResourceId::IdType createResource() {
            return resourceCounter++;
        }

        ResourceId::IdType resourceCounter;

        std::vector<Pass> passes;

        std::unordered_map<ResourceId, Buffer, ResourceIdHash> bufferAllocations;
        std::unordered_map<ResourceId, Texture, ResourceIdHash> textureAllocations;

        Resource<Texture> backBufferColor;
        Resource<Texture> backBufferDepthStencil;
    };
}

#endif //XENGINE_RENDERGRAPH_BUILDER_HPP
