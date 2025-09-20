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


#include "../../include/xng/graphics/3d/meshallocator.hpp"

#include "../../include/xng/graphics/vertexstream.hpp"

namespace xng {
    MeshAllocator::MeshAllocation MeshAllocator::allocateMesh(const Mesh &mesh) {
        MeshAllocation ret;

        MeshAllocation::Data data;
        data.primitive = mesh.primitive;
        data.drawCall.count = mesh.indices.size();
        auto indexSize = mesh.indices.size() * sizeof(unsigned int);
        data.drawCall.offset = allocateIndexData(indexSize);
        auto vertexSize = mesh.vertices.size() * mesh.vertexLayout.getLayoutSize();
        auto baseVertex = allocateVertexData(vertexSize);
        data.baseVertex = baseVertex / mesh.vertexLayout.getLayoutSize();

        ret.data.emplace_back(data);

        for (auto &subMesh: mesh.subMeshes) {
            auto v = allocateMesh(subMesh).data;
            ret.data.insert(ret.data.end(), v.begin(), v.end());
        }

        return ret;
    }

    void MeshAllocator::allocateMesh(const ResourceHandle<SkinnedMesh> &mesh) {
        if (mesh.get().primitive != TRIANGLES) {
            throw std::runtime_error("Unsupported mesh primitive");
        }
        if (mesh.get().vertexLayout != SkinnedMesh::getDefaultVertexLayout()) {
            throw std::runtime_error("Unsupported mesh vertex layout");
        }
        if (mesh.get().indices.empty()) {
            throw std::runtime_error("Arrayed mesh not supported, must be indexed");
        }
        if (meshAllocations.find(mesh.getUri()) == meshAllocations.end()
            && pendingMeshAllocations.find(mesh.getUri()) == pendingMeshAllocations.end()) {
            const MeshAllocation mdata = allocateMesh(mesh.get());
            pendingMeshAllocations[mesh.getUri()] = mdata;
            pendingMeshHandles[mesh.getUri()] = mesh;
        }
    }

    void MeshAllocator::uploadMeshes(RenderGraphContext &ctx,
                                     RenderGraphResource vertexBuffer,
                                     RenderGraphResource indexBuffer) {
        for (auto &pair: pendingMeshAllocations) {
            auto meshHandle = pendingMeshHandles.at(pair.first);
            for (auto i = 0; i < meshHandle.get().subMeshes.size() + 1; i++) {
                auto &data = pair.second.data.at(i);
                auto &curMesh = i == 0 ? meshHandle.get() : meshHandle.get().subMeshes.at(i - 1);

                auto vb = VertexStream().addVertices(curMesh.vertices).getVertexBuffer();
                ctx.uploadBuffer(vertexBuffer,
                                 vb.data(),
                                 vb.size(),
                                 data.baseVertex * curMesh.vertexLayout.getLayoutSize());

                auto ib = curMesh.indices;
                ctx.uploadBuffer(indexBuffer,
                                 reinterpret_cast<const uint8_t *>(ib.data()),
                                 ib.size() * sizeof(unsigned int),
                                 data.drawCall.offset);
            }
            meshAllocations[pair.first] = pair.second;
        }
        pendingMeshAllocations.clear();
        pendingMeshHandles.clear();
    }

    void MeshAllocator::deallocateMesh(const ResourceHandle<SkinnedMesh> &mesh) {
        auto alloc = meshAllocations.at(mesh.getUri());
        meshAllocations.erase(mesh.getUri());
        for (auto &data: alloc.data) {
            deallocateVertexData(data.baseVertex * mesh.get().vertexLayout.getLayoutSize());
            deallocateIndexData(data.drawCall.offset);
        }
        mergeFreeVertexBufferRanges();
        mergeFreeIndexBufferRanges();
    }

    bool MeshAllocator::shouldRebuild() {
        return currentVertexBufferSize < requestedVertexBufferSize
               || currentIndexBufferSize < requestedIndexBufferSize;
    }

    void MeshAllocator::declareReadWrite(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass) {
        builder.readWrite(pass, currentVertexBuffer);
        builder.readWrite(pass, currentIndexBuffer);
        if (staleVertexBuffer) {
            builder.readWrite(pass, staleVertexBuffer);
            builder.readWrite(pass, staleIndexBuffer);
        }
    }

    void MeshAllocator::onCreate(RenderGraphBuilder &builder) {
        currentVertexBuffer = builder.createVertexBuffer(0);
        currentIndexBuffer = builder.createIndexBuffer(0);
        currentVertexBufferSize = 0;
        currentIndexBufferSize = 0;
    }

    void MeshAllocator::onRecreate(RenderGraphBuilder &builder) {
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
    }

    const std::map<Uri, MeshAllocator::MeshAllocation> &MeshAllocator::getMeshAllocations(RenderGraphContext &ctx) {
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

    size_t MeshAllocator::allocateVertexData(size_t size) {
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

    void MeshAllocator::deallocateVertexData(size_t offset) {
        auto size = allocatedVertexRanges.at(offset);
        allocatedVertexRanges.erase(offset);
        freeVertexBufferRanges[offset] = size;
    }

    size_t MeshAllocator::allocateIndexData(size_t size) {
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

    void MeshAllocator::deallocateIndexData(size_t offset) {
        auto size = allocatedIndexRanges.at(offset);
        allocatedIndexRanges.erase(offset);
        freeIndexBufferRanges[offset] = size;
    }

    void MeshAllocator::mergeFreeVertexBufferRanges() {
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

    void MeshAllocator::mergeFreeIndexBufferRanges() {
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
