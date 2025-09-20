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

#ifndef XENGINE_MESHALLOCATOR_HPP
#define XENGINE_MESHALLOCATOR_HPP

#include "xng/rendergraph/rendergraphbuilder.hpp"
#include "xng/graphics/scene/skinnedmesh.hpp"

namespace xng {
    class MeshAllocator {
    public:
        struct MeshAllocation {
            struct Data {
                Primitive primitive = TRIANGLES;
                DrawCall drawCall{};
                size_t baseVertex = 0;
            };

            std::vector<Data> data;
        };

        void allocateMesh(const ResourceHandle<SkinnedMesh> &mesh);

        void deallocateMesh(const ResourceHandle<SkinnedMesh> &mesh);

        bool shouldRebuild();

        void declareReadWrite(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass);

        void onCreate(RenderGraphBuilder &builder);

        void onRecreate(RenderGraphBuilder &builder);

        const std::map<Uri, MeshAllocation> &getMeshAllocations(RenderGraphContext &ctx);

        RenderGraphResource getVertexBuffer() {
            return currentVertexBuffer;
        }

        RenderGraphResource getIndexBuffer() {
            return currentIndexBuffer;
        }

    private:
        void uploadMeshes(RenderGraphContext &ctx, RenderGraphResource vertexBuffer, RenderGraphResource indexBuffer);

        MeshAllocation allocateMesh(const Mesh &mesh);

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

        std::map<Uri, MeshAllocation> meshAllocations;
        std::map<Uri, MeshAllocation> pendingMeshAllocations;
        std::map<Uri, ResourceHandle<SkinnedMesh> > pendingMeshHandles;

        size_t requestedVertexBufferSize{};
        size_t requestedIndexBufferSize{};

        std::map<size_t, size_t> freeVertexBufferRanges;
        // start and size of free ranges of vertices with layout vertexLayout in the vertex buffer
        std::map<size_t, size_t> freeIndexBufferRanges; // start and size of free ranges of bytes in the index buffer

        std::map<size_t, size_t> allocatedVertexRanges;
        std::map<size_t, size_t> allocatedIndexRanges;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        RenderGraphResource currentVertexBuffer{};
        RenderGraphResource currentIndexBuffer{};

        RenderGraphResource staleVertexBuffer{};
        RenderGraphResource staleIndexBuffer{};
    };
}

#endif //XENGINE_MESHALLOCATOR_HPP
