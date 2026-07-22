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

#ifndef XENGINE_MESHSTREAMER_HPP
#define XENGINE_MESHSTREAMER_HPP

#include <unordered_map>

#include "xng/assets/mesh.hpp"

#include "xng/util/rangeallocator.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/buffer.hpp"

#include "xng/renderer/vertexattribute.hpp"
#include "xng/renderer/vertexbuilder.hpp"
#include "xng/renderer/stream/chunkstreamer.hpp"

namespace xng {
    /**
     * The MeshStreamer uploads mesh data to separate StreamBuffer's per attribute.
     * All meshes share the same attribute buffers / index buffer.
     * Missing attributes in uploaded meshes are padded with default values.
     * For array meshes the streamer generates index data.
     *
     * This wastes memory for the padded default values.
     *
     * An alternative approach is sorting draw calls into buckets based on defined attributes
     * and binding a single default value attribute buffer with stride = 0.
     *
     * For the forward rendering path this will incur O(N) binding overhead in the worst case
     * because the draw calls must execute in submission order for correct transparency.
     *
     * For the best performance the forward rendering path would need to write their own default attributes in the buffers.
     */
    class MeshStreamer {
    public:
        typedef size_t Handle;

        struct Allocation {
            rg::Primitive primitive{};
            rg::DrawCall drawCall{};
            int baseVertex{}; // The offset applied to each index.
            size_t vertexCount{}; // The number of vertices for this mesh
            bool skinned{};
            int skinBaseVertex{}; // The offset applied to each vertex for indexing into the skinned buffers.
        };

        MeshStreamer(rg::Heap &heap, ChunkStreamer &chunkStreamer)
            : indexBuffer(StreamBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_INDEX)),
              skinnedBindPosBuffer(StreamBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE)),
              skinnedBoneIndicesBuffer(StreamBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE)),
              skinnedBoneWeightsBuffer(StreamBuffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE)) {
            for (auto attr = ATTRIBUTE_BEGIN;
                 attr <= ATTRIBUTE_END;
                 attr = static_cast<VertexAttribute>(attr + 1)) {
                auto caps = rg::Buffer::CAPABILITY_VERTEX;
                if (attr == POSITION) {
                    caps = caps | rg::Buffer::CAPABILITY_STORAGE;
                }
                vertexBuffers.emplace(attr, std::move(StreamBuffer(heap, chunkStreamer, caps)));
            }
            skinnedBufferAlloc = RangeAllocator();
        }

        /**
         * Upload the given mesh data.
         * Positions must be present, all other attributes are padded with default values if missing.
         * Both indexed and non-indexed meshes are supported.
         *
         * @param mesh The mesh to upload
         * @param boneIndices The indices of the bones
         * @return The allocation handle
         */
        Handle create(const Mesh &mesh, const std::unordered_map<std::string, unsigned int> &boneIndices = {}) {
            Handle ret;
            if (!freeHandles.empty()) {
                ret = freeHandles.back();
                freeHandles.pop_back();
            } else {
                ret = nextHandle++;
            }

            Allocation alloc;
            switch (mesh.primitive) {
                case Mesh::POINTS:
                    alloc.primitive = rg::Primitive::POINTS;
                    break;
                case Mesh::LINES:
                    alloc.primitive = rg::Primitive::LINES;
                    break;
                case Mesh::TRIANGLES:
                    alloc.primitive = rg::Primitive::TRIANGLES;
                    break;
                case Mesh::QUAD:
                    alloc.primitive = rg::Primitive::QUAD;
                    break;
            }

            alloc.skinned = mesh.boneWeights.size() > 0;

            const auto vertexCount = mesh.positions.size();

            if (alloc.skinned) {
                std::map<size_t, std::vector<std::pair<float, std::string> > > boneWeightsMap;
                for (auto &pair: boneIndices) {
                    auto wIt = mesh.boneWeights.find(pair.first);
                    if (wIt != mesh.boneWeights.end()) {
                        for (auto &weight: wIt->second) {
                            boneWeightsMap[weight.vertex].emplace_back(std::make_pair(weight.weight, pair.first));
                        }
                    }
                }

                VertexBuilder vertexBoneIndicesBuilder;
                VertexBuilder vertexBoneWeightsBuilder;
                for (auto i = 0; i < vertexCount; i++) {
                    const auto it = boneWeightsMap.find(i);
                    if (it != boneWeightsMap.end()) {
                        Vec4i vertexBoneIndex(-1);
                        Vec4f vertexBoneWeight(0);

                        auto weights = it->second;

                        std::sort(weights.begin(), weights.end(), [](const auto &a, const auto &b) {
                            return a.first > b.first;
                        });

                        int weightIndex = 0;
                        for (auto weight = weights.begin();
                             weight != weights.end()
                             && weightIndex < 4;
                             ++weight, weightIndex++) {
                            switch (weightIndex) {
                                case 0:
                                    vertexBoneIndex.x = static_cast<int>(boneIndices.at(weight->second));
                                    vertexBoneWeight.x = weight->first;
                                    break;
                                case 1:
                                    vertexBoneIndex.y = static_cast<int>(boneIndices.at(weight->second));
                                    vertexBoneWeight.y = weight->first;
                                    break;
                                case 2:
                                    vertexBoneIndex.z = static_cast<int>(boneIndices.at(weight->second));
                                    vertexBoneWeight.z = weight->first;
                                    break;
                                case 3:
                                    vertexBoneIndex.w = static_cast<int>(boneIndices.at(weight->second));
                                    vertexBoneWeight.w = weight->first;
                                    break;
                                default:
                                    assert(false);
                                    break;
                            }
                        }

                        //Renormalize weights
                        float sum = vertexBoneWeight.x + vertexBoneWeight.y + vertexBoneWeight.z + vertexBoneWeight.w;
                        if (sum > 0) {
                            vertexBoneWeight.x /= sum;
                            vertexBoneWeight.y /= sum;
                            vertexBoneWeight.z /= sum;
                            vertexBoneWeight.w /= sum;
                        }

                        vertexBoneIndicesBuilder.addVec4(vertexBoneIndex);
                        vertexBoneWeightsBuilder.addVec4(vertexBoneWeight);
                    } else {
                        vertexBoneIndicesBuilder.addVec4(Vec4i(-1));
                        vertexBoneWeightsBuilder.addVec4(Vec4f(0.0f));
                    }
                }

                auto vertexBoneIndices = vertexBoneIndicesBuilder.build();
                auto vertexBoneWeights = vertexBoneWeightsBuilder.build();

                auto vertexPositions = getBytes(POSITION, mesh);
                alloc.skinBaseVertex = static_cast<int>(skinnedBufferAlloc.allocate(vertexCount));

                skinnedBindPosBufferHandles.emplace(ret,
                                                    skinnedBindPosBuffer.upload(vertexPositions.data(),
                                                        vertexPositions.size(),
                                                        alloc.skinBaseVertex * sizeof(float) * 3));
                skinnedBoneIndicesBufferHandles.emplace(ret,
                                                        skinnedBoneIndicesBuffer.upload(vertexBoneIndices.data(),
                                                            vertexBoneIndices.size(),
                                                            alloc.skinBaseVertex * sizeof(int) * 4));
                skinnedBoneWeightsBufferHandles.emplace(ret,
                                                        skinnedBoneWeightsBuffer.upload(vertexBoneWeights.data(),
                                                            vertexBoneWeights.size(),
                                                            alloc.skinBaseVertex * sizeof(float) * 4));
            }

            auto vertexBufferIndex = vertexBufferAlloc.allocate(vertexCount);

            std::unordered_map<VertexAttribute, StreamBuffer::Handle> attributeBufferHandles;
            for (auto attr = ATTRIBUTE_BEGIN;
                 attr <= ATTRIBUTE_END;
                 attr = static_cast<VertexAttribute>(attr + 1)) {
                auto data = getBytes(attr, mesh);
                attributeBufferHandles.emplace(attr, uploadAttribute(attr, data, vertexBufferIndex));
            }

            vertexBufferHandles.emplace(ret, attributeBufferHandles);

            alloc.vertexCount = vertexCount;

            if (mesh.indices.size() > 0) {
                auto indexBufferOffset = indexBufferAlloc.allocate(mesh.indices.size() * sizeof(unsigned int));
                auto uploadHandle = indexBuffer.upload(reinterpret_cast<const uint8_t *>(mesh.indices.data()),
                                                       mesh.indices.size() * sizeof(unsigned int),
                                                       indexBufferOffset);
                indexBufferHandles.emplace(ret, uploadHandle);

                alloc.drawCall.offset = indexBufferOffset;
                alloc.drawCall.count = mesh.indices.size();
            } else {
                std::vector<unsigned int> indices(vertexCount);
                for (auto i = 0; i < vertexCount; i++) {
                    indices.at(i) = i;
                }
                auto indexBufferOffset = indexBufferAlloc.allocate(indices.size() * sizeof(unsigned int));
                auto uploadHandle = indexBuffer.upload(reinterpret_cast<const uint8_t *>(indices.data()),
                                                       indices.size() * sizeof(unsigned int),
                                                       indexBufferOffset);
                indexBufferHandles.emplace(ret, uploadHandle);

                alloc.drawCall.offset = indexBufferOffset;
                alloc.drawCall.count = indices.size();
            }

            alloc.baseVertex = static_cast<int>(vertexBufferIndex);

            allocations.emplace(ret, alloc);

            return ret;
        }

        void destroy(const Handle &handle) {
            auto vIt = vertexBufferHandles.find(handle);
            if (vIt != vertexBufferHandles.end()) {
                for (const auto &pair: vIt->second) {
                    vertexBuffers.at(pair.first).release(pair.second);
                }
            }

            auto it = indexBufferHandles.find(handle);
            if (it != indexBufferHandles.end()) {
                indexBuffer.release(indexBufferHandles.at(handle));
            }

            const auto &alloc = allocations.at(handle);

            vertexBufferAlloc.free(alloc.baseVertex, alloc.vertexCount);
            indexBufferAlloc.free(alloc.drawCall.offset, alloc.drawCall.count * sizeof(unsigned int));

            if (alloc.skinned) {
                skinnedBindPosBuffer.release(skinnedBindPosBufferHandles.at(handle));
                skinnedBoneIndicesBuffer.release(skinnedBoneIndicesBufferHandles.at(handle));
                skinnedBoneWeightsBuffer.release(skinnedBoneWeightsBufferHandles.at(handle));
                skinnedBufferAlloc.free(alloc.skinBaseVertex, alloc.vertexCount);
            }

            vertexBufferHandles.erase(handle);
            indexBufferHandles.erase(handle);
            skinnedBindPosBufferHandles.erase(handle);
            skinnedBoneIndicesBufferHandles.erase(handle);
            skinnedBoneWeightsBufferHandles.erase(handle);
            allocations.erase(handle);
            freeHandles.push_back(handle);
        }

        bool isUploadComplete(const Handle &handle) const {
            for (auto &pair: vertexBufferHandles.at(handle)) {
                if (!vertexBuffers.at(pair.first).isUploadComplete(pair.second)) {
                    return false;
                }
            }
            if (allocations.at(handle).skinned) {
                if (!skinnedBindPosBuffer.isUploadComplete(skinnedBindPosBufferHandles.at(handle))
                    || !skinnedBoneIndicesBuffer.isUploadComplete(skinnedBoneIndicesBufferHandles.at(handle))
                    || !skinnedBoneWeightsBuffer.isUploadComplete(skinnedBoneWeightsBufferHandles.at(handle))) {
                    return false;
                }
            }
            return indexBuffer.isUploadComplete(indexBufferHandles.at(handle));
        }

        void flush(const Handle &handle) {
            for (auto &pair: vertexBufferHandles.at(handle)) {
                vertexBuffers.at(pair.first).flush(pair.second);
            }
            if (allocations.at(handle).skinned) {
                skinnedBindPosBuffer.flush(skinnedBindPosBufferHandles.at(handle));
                skinnedBoneIndicesBuffer.flush(skinnedBoneIndicesBufferHandles.at(handle));
                skinnedBoneWeightsBuffer.flush(skinnedBoneWeightsBufferHandles.at(handle));
            }
            indexBuffer.flush(indexBufferHandles.at(handle));
        }

        const Allocation &getAllocation(const Handle &handle) const {
            return allocations.at(handle);
        }

        void commit(StreamerQueue &queue) {
            for (auto &pair: vertexBuffers) {
                pair.second.commit(queue);
            }

            indexBuffer.commit(queue);
            skinnedBindPosBuffer.commit(queue);
            skinnedBoneIndicesBuffer.commit(queue);
            skinnedBoneWeightsBuffer.commit(queue);
        }

        std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > getVertexBuffers() const {
            std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > ret;
            for (auto &pair: vertexBuffers) {
                ret.emplace(pair.first, pair.second.getBuffer());
            }
            return ret;
        }

        rg::HeapResource<rg::Buffer> getIndexBuffer() const {
            return indexBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getSkinnedBindPosBuffer() const {
            return skinnedBindPosBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getSkinnedBoneIndicesBuffer() const {
            return skinnedBoneIndicesBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getSkinnedBoneWeightsBuffer() const {
            return skinnedBoneWeightsBuffer.getBuffer();
        }

    private:
        static std::vector<uint8_t> getBytes(const VertexAttribute attr, const Mesh &mesh) {
            VertexBuilder builder;
            switch (attr) {
                case POSITION:
                    for (auto &pos: mesh.positions) {
                        builder.addVec3(pos);
                    }
                    break;
                case NORMAL:
                    if (mesh.normals.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec3(Vec3f(0, 0, 1));
                        }
                    } else {
                        for (auto &norm: mesh.normals) {
                            builder.addVec3(norm);
                        }
                    }
                    break;
                case TANGENT:
                    if (mesh.tangents.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec3(Vec3f(1, 0, 0));
                        }
                    } else {
                        for (auto &tangent: mesh.tangents) {
                            builder.addVec3(tangent);
                        }
                    }
                    break;
                case BITANGENT:
                    if (mesh.bitangents.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec3(Vec3f(0, 1, 0));
                        }
                    } else {
                        for (auto &bitangent: mesh.bitangents) {
                            builder.addVec3(bitangent);
                        }
                    }
                    break;
                case UV:
                    if (mesh.uvs.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec2(Vec2f(0, 0));
                        }
                    } else {
                        for (auto &uv: mesh.uvs) {
                            builder.addVec2(uv);
                        }
                    }
                    break;
            }
            return builder.build();
        }

        StreamBuffer::Handle uploadAttribute(const VertexAttribute attribute,
                                             const std::vector<uint8_t> &data,
                                             const size_t vertexIndex) {
            return vertexBuffers.at(attribute).upload(data, vertexIndex * getVertexAttributeSize(attribute));
        }

        std::unordered_map<VertexAttribute, StreamBuffer> vertexBuffers;
        StreamBuffer indexBuffer;

        RangeAllocator vertexBufferAlloc;
        RangeAllocator indexBufferAlloc;

        std::unordered_map<Handle, std::unordered_map<VertexAttribute, StreamBuffer::Handle> > vertexBufferHandles;
        std::unordered_map<Handle, StreamBuffer::Handle> indexBufferHandles;

        StreamBuffer skinnedBindPosBuffer;
        StreamBuffer skinnedBoneIndicesBuffer;
        StreamBuffer skinnedBoneWeightsBuffer;

        RangeAllocator skinnedBufferAlloc;

        std::unordered_map<Handle, StreamBuffer::Handle> skinnedBindPosBufferHandles;
        std::unordered_map<Handle, StreamBuffer::Handle> skinnedBoneIndicesBufferHandles;
        std::unordered_map<Handle, StreamBuffer::Handle> skinnedBoneWeightsBufferHandles;

        Handle nextHandle = 0;
        std::vector<Handle> freeHandles;

        std::unordered_map<Handle, Allocation> allocations;
    };
}

#endif //XENGINE_MESHSTREAMER_HPP
