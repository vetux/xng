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

#ifndef XENGINE_MESHALLOCATOR_HPP
#define XENGINE_MESHALLOCATOR_HPP

#include "xng/rendergraph/rendergraphbuilder.hpp"
#include "xng/graphics/scene/skinnedmodel.hpp"

namespace xng {
    /**
     * Allocates Meshes from resource handles into a single vertex / index buffer.
     */
    class XENGINE_EXPORT MeshAtlas {
    public:
        struct MeshAllocation {
            struct Data {
                RenderPrimitive primitive = TRIANGLES;
                DrawCall drawCall{};
                size_t baseVertex = 0;
            };

            std::vector<Data> data; // Allocation data for each submesh
        };

        /**
         * Allocate the mesh and all sub meshes.
         *
         * @param mesh
         */
        void allocateMesh(const ResourceHandle<SkinnedModel> &mesh);

        /**
         * Deallocate the mesh and all sub meshes.
         *
         * @param mesh
         */
        void deallocateMesh(const ResourceHandle<SkinnedModel> &mesh);

        bool shouldRebuild();

        void update(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass);

        const std::map<Uri, MeshAllocation> &getMeshAllocations(RenderGraphContext &ctx);

        RenderGraphResource getVertexBuffer() {
            return currentVertexBuffer;
        }

        RenderGraphResource getIndexBuffer() {
            return currentIndexBuffer;
        }

    private:
        void uploadMeshes(RenderGraphContext &ctx, RenderGraphResource vertexBuffer, RenderGraphResource indexBuffer);

        MeshAllocation::Data allocateMesh(const Mesh &mesh);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateVertexData(size_t size);

        void deallocateVertexData(size_t offset);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateIndexData(size_t size);

        void deallocateIndexData(size_t offset);

        void mergeFreeVertexBufferRanges();

        void mergeFreeIndexBufferRanges();

        ShaderAttributeLayout vertexLayout = SkinnedModel::getVertexLayout();

        std::map<Uri, MeshAllocation> meshAllocations;
        std::map<Uri, ResourceHandle<SkinnedModel> > pendingUploads;

        size_t requestedVertexBufferSize{};
        size_t requestedIndexBufferSize{};

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        std::map<size_t, size_t> freeVertexBufferRanges;
        std::map<size_t, size_t> freeIndexBufferRanges;

        std::map<size_t, size_t> allocatedVertexRanges;
        std::map<size_t, size_t> allocatedIndexRanges;

        RenderGraphResource currentVertexBuffer{};
        RenderGraphResource currentIndexBuffer{};

        RenderGraphResource staleVertexBuffer{};
        RenderGraphResource staleIndexBuffer{};
    };
}

#endif //XENGINE_MESHALLOCATOR_HPP
