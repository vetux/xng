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
#include "xng/renderer/stream/bufferstreamer.hpp"

namespace xng {
    /**
     * The MeshStreamer uploads mesh data to separate StreamBuffer's per attribute.
     * All meshes share the same attribute buffers / index buffer.
     * Missing attributes in uploaded meshes are padded with default values.
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
            bool indexed{};
            int baseVertex{}; // The offset applied to each index.
            size_t vertexCount{}; // The number of vertices for this mesh
        };

        explicit MeshStreamer(rg::Heap &heap)
            : indexBuffer(StreamBuffer(heap, rg::Buffer::CAPABILITY_INDEX)) {
            for (auto attr = VertexAttribute::ATTRIBUTE_BEGIN;
                 attr <= VertexAttribute::ATTRIBUTE_END;
                 attr = static_cast<VertexAttribute>(attr + 1)) {
                vertexBuffers.emplace(attr, StreamBuffer(heap, rg::Buffer::CAPABILITY_VERTEX));
            }
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
        Handle create(const Mesh &mesh, const std::unordered_map<std::string, unsigned int> &boneIndices) {
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
            alloc.indexed = false;

            const auto vertexCount = mesh.positions.size();

            std::map<size_t, std::vector<std::pair<float, std::string> > > boneWeightsMap;
            for (auto &pair: boneIndices) {
                auto wIt = mesh.boneWeights.find(pair.first);
                if (wIt != mesh.boneWeights.end()) {
                    for (auto &weight: wIt->second) {
                        boneWeightsMap[weight.vertex].emplace_back(std::make_pair(weight.weight, pair.first));
                    }
                }
            }

            std::vector<Vec4i> vertexBoneIndices;
            std::vector<Vec4f> vertexBoneWeights;
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

                    vertexBoneIndices.emplace_back(vertexBoneIndex);
                    vertexBoneWeights.emplace_back(vertexBoneWeight);
                } else {
                    vertexBoneIndices.emplace_back(-1);
                    vertexBoneWeights.emplace_back(0.0f);
                }
            }

            auto vertexBufferIndex = vertexBufferAlloc.allocate(vertexCount);

            std::unordered_map<VertexAttribute, StreamBuffer::Handle> attributeBufferHandles;
            for (auto attr = VertexAttribute::ATTRIBUTE_BEGIN;
                 attr <= VertexAttribute::ATTRIBUTE_END;
                 attr = static_cast<VertexAttribute>(attr + 1)) {
                auto data = getBytes(attr, mesh, vertexBoneIndices, vertexBoneWeights);
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

                alloc.indexed = true;
                alloc.baseVertex = static_cast<int>(vertexBufferIndex);
            } else {
                alloc.drawCall.offset = vertexBufferIndex;
                alloc.drawCall.count = vertexCount;
                alloc.indexed = false;
            }

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
            if (alloc.indexed) {
                vertexBufferAlloc.free(alloc.baseVertex, alloc.vertexCount);
                indexBufferAlloc.free(alloc.drawCall.offset, alloc.drawCall.count * sizeof(unsigned int));
            } else {
                vertexBufferAlloc.free(alloc.drawCall.offset, alloc.vertexCount);
            }

            vertexBufferHandles.erase(handle);
            indexBufferHandles.erase(handle);
            allocations.erase(handle);
            freeHandles.push_back(handle);
        }

        bool isUploadComplete(const Handle &handle) const {
            for (auto &pair: vertexBufferHandles.at(handle)) {
                if (!vertexBuffers.at(pair.first).isUploadComplete(pair.second)) {
                    return false;
                }
            }
            if (allocations.at(handle).indexed
                && !indexBuffer.isUploadComplete(indexBufferHandles.at(handle))) {
                return false;
            }
            return true;
        }

        void flush(const Handle &handle) {
            for (auto &pair: vertexBufferHandles.at(handle)) {
                vertexBuffers.at(pair.first).flush(pair.second);
            }
            if (allocations.at(handle).indexed) {
                indexBuffer.flush(indexBufferHandles.at(handle));
            }
        }

        const Allocation &getAllocation(const Handle &handle) const {
            return allocations.at(handle);
        }

        std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > commitVertexBuffers(rg::GraphBuilder &ctx) {
            std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > ret;
            for (auto &pair: vertexBuffers) {
                ret.emplace(pair.first, pair.second.commit(ctx));
            }
            return ret;
        }

        rg::HeapResource<rg::Buffer> commitIndexBuffer(rg::GraphBuilder &ctx) {
            return indexBuffer.commit(ctx);
        }

    private:
        static std::vector<uint8_t> getBytes(const VertexAttribute attr,
                                             const Mesh &mesh,
                                             const std::vector<Vec4i> &boneIndices,
                                             const std::vector<Vec4f> &boneWeights) {
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
                case BONE_INDEX:
                    if (boneIndices.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec4(Vec4i(-1));
                        }
                    } else {
                        for (auto &indices: boneIndices) {
                            builder.addVec4(indices);
                        }
                    }
                    break;
                case BONE_WEIGHT:
                    if (boneWeights.empty()) {
                        for (auto i = 0; i < mesh.positions.size(); i++) {
                            builder.addVec4(Vec4f(0));
                        }
                    } else {
                        for (auto &weights: boneWeights) {
                            builder.addVec4(weights);
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

        Handle nextHandle = 0;
        std::vector<Handle> freeHandles;

        std::unordered_map<Handle, Allocation> allocations;
    };
}

#endif //XENGINE_MESHSTREAMER_HPP
