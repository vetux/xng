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


#include "xng/graphics/3d/meshatlas/meshatlas.hpp"

#include "xng/graphics/vertexstream.hpp"

namespace xng {
    MeshAtlas::MeshAllocation MeshAtlas::allocateMesh(const Mesh &mesh) {
        if (mesh.primitive != TRIANGLES) {
            throw std::runtime_error("Unsupported mesh primitive");
        }
        if (mesh.vertexLayout != vertexLayout) {
            throw std::runtime_error("Unsupported mesh vertex layout");
        }
        if (mesh.indices.empty()) {
            throw std::runtime_error("Arrayed mesh not supported, must be indexed");
        }

        MeshAllocation ret;

        MeshAllocation::Data data;
        data.primitive = mesh.primitive;
        data.drawCall.count = mesh.indices.size();
        auto indexSize = mesh.indices.size() * sizeof(unsigned int);
        data.drawCall.offset = allocateIndexData(indexSize);
        auto vertexSize = mesh.vertices.size() * vertexLayout.getLayoutSize();
        auto baseVertex = allocateVertexData(vertexSize);
        data.baseVertex = baseVertex / vertexLayout.getLayoutSize();

        ret.data.emplace_back(data);

        for (auto &subMesh: mesh.subMeshes) {
            auto v = allocateMesh(subMesh).data;
            ret.data.insert(ret.data.end(), v.begin(), v.end());
        }

        return ret;
    }

    void MeshAtlas::allocateMesh(const ResourceHandle<SkinnedMesh> &mesh) {
        if (meshAllocations.find(mesh.getUri()) == meshAllocations.end()
            && pendingUploads.find(mesh.getUri()) == pendingUploads.end()) {
            const MeshAllocation mdata = allocateMesh(mesh.get());
            meshAllocations[mesh.getUri()] = mdata;
            pendingUploads[mesh.getUri()] = mesh;
        }
    }

    void MeshAtlas::uploadMeshes(RenderGraphContext &ctx,
                                    RenderGraphResource vertexBuffer,
                                    RenderGraphResource indexBuffer) {
        for (auto &pair: pendingUploads) {
            auto &meshAllocation = meshAllocations.at(pair.first);
            for (auto i = 0; i < pair.second.get().subMeshes.size() + 1; i++) {
                auto &data = meshAllocation.data.at(i);
                auto &mesh = i == 0 ? pair.second.get() : pair.second.get().subMeshes.at(i - 1);

                auto vb = VertexStream().addVertices(mesh.vertices).getVertexBuffer();
                ctx.uploadBuffer(vertexBuffer,
                                 vb.data(),
                                 vb.size(),
                                 data.baseVertex * vertexLayout.getLayoutSize());

                auto ib = mesh.indices;
                ctx.uploadBuffer(indexBuffer,
                                 reinterpret_cast<const uint8_t *>(ib.data()),
                                 ib.size() * sizeof(unsigned int),
                                 data.drawCall.offset);
            }
        }
        pendingUploads.clear();
    }

    void MeshAtlas::deallocateMesh(const ResourceHandle<SkinnedMesh> &mesh) {
        auto alloc = meshAllocations.at(mesh.getUri());
        meshAllocations.erase(mesh.getUri());
        pendingUploads.erase(mesh.getUri());
        for (auto &data: alloc.data) {
            deallocateVertexData(data.baseVertex * vertexLayout.getLayoutSize());
            deallocateIndexData(data.drawCall.offset);
        }
        mergeFreeVertexBufferRanges();
        mergeFreeIndexBufferRanges();
    }

    bool MeshAtlas::shouldRebuild() {
        return currentVertexBufferSize < requestedVertexBufferSize || currentIndexBufferSize < requestedIndexBufferSize;
    }

    void MeshAtlas::update(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass) {
        if (requestedVertexBufferSize != currentVertexBufferSize
            || requestedIndexBufferSize != currentIndexBufferSize
            || !currentVertexBuffer) {
            if (currentVertexBufferSize > 0) {
                staleVertexBuffer = builder.inheritResource(currentVertexBuffer);
            }
            if (currentIndexBufferSize > 0) {
                staleIndexBuffer = builder.inheritResource(currentIndexBuffer);
            }

            currentVertexBuffer = builder.createVertexBuffer(requestedVertexBufferSize);
            currentVertexBufferSize = requestedVertexBufferSize;

            currentIndexBuffer = builder.createIndexBuffer(requestedIndexBufferSize);
            currentIndexBufferSize = requestedIndexBufferSize;
        } else {
            currentVertexBuffer = builder.inheritResource(currentVertexBuffer);
            currentIndexBuffer = builder.inheritResource(currentIndexBuffer);
        }

        builder.readWrite(pass, currentVertexBuffer);
        builder.readWrite(pass, currentIndexBuffer);
        if (staleVertexBuffer) {
            builder.readWrite(pass, staleVertexBuffer);
            builder.readWrite(pass, staleIndexBuffer);
        }
    }

    const std::map<Uri, MeshAtlas::MeshAllocation> &MeshAtlas::getMeshAllocations(RenderGraphContext &ctx) {
        if (staleVertexBuffer) {
            ctx.copyBuffer(currentVertexBuffer, staleVertexBuffer, 0, 0, currentVertexBufferSize);
            ctx.copyBuffer(currentIndexBuffer, staleIndexBuffer, 0, 0, currentIndexBufferSize);
            currentVertexBufferSize = requestedVertexBufferSize;
            currentIndexBufferSize = requestedIndexBufferSize;
            staleVertexBuffer = {};
            staleIndexBuffer = {};
        }
        uploadMeshes(ctx, currentVertexBuffer, currentIndexBuffer);
        return meshAllocations;
    }

    size_t MeshAtlas::allocateVertexData(size_t size) {
        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeVertexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }

        if (foundFreeRange) {
            auto rangeSize = freeVertexBufferRanges.at(ret) -= size;
            freeVertexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeVertexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedVertexBufferSize;
            requestedVertexBufferSize += size;
        }

        allocatedVertexRanges[ret] = size;

        return ret;
    }

    void MeshAtlas::deallocateVertexData(size_t offset) {
        auto size = allocatedVertexRanges.at(offset);
        allocatedVertexRanges.erase(offset);
        freeVertexBufferRanges[offset] = size;
    }

    size_t MeshAtlas::allocateIndexData(size_t size) {
        assert(size % sizeof(unsigned int) == 0);

        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeIndexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }
        if (foundFreeRange) {
            auto rangeSize = freeIndexBufferRanges.at(ret) -= size;
            freeIndexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeIndexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedIndexBufferSize;
            requestedIndexBufferSize += size;
        }

        allocatedIndexRanges[ret] = size;

        return ret;
    }

    void MeshAtlas::deallocateIndexData(size_t offset) {
        auto size = allocatedIndexRanges.at(offset);
        allocatedIndexRanges.erase(offset);
        freeIndexBufferRanges[offset] = size;
    }

    void MeshAtlas::mergeFreeVertexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto vertexRanges = freeVertexBufferRanges;
            for (auto range = freeVertexBufferRanges.begin(); range != freeVertexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeVertexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && vertexRanges.find(range->first) != vertexRanges.end()
                        && vertexRanges.find(next->first) != vertexRanges.end()) {
                        merged = true;
                        vertexRanges.at(range->first) += next->second;
                        vertexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeVertexBufferRanges = vertexRanges;
        }
    }

    void MeshAtlas::mergeFreeIndexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto indexRanges = freeIndexBufferRanges;
            for (auto range = freeIndexBufferRanges.begin(); range != freeIndexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeIndexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && indexRanges.find(range->first) != indexRanges.end()
                        && indexRanges.find(next->first) != indexRanges.end()) {
                        merged = true;
                        indexRanges.at(range->first) += next->second;
                        indexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeIndexBufferRanges = indexRanges;
        }
    }
}
