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

#ifndef XENGINE_MESHSTREAMER_HPP
#define XENGINE_MESHSTREAMER_HPP

#include <unordered_map>

#include "xng/assets/mesh.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/vertexbuffer.hpp"

#include "xng/renderer/stream/bufferstreamer.hpp"

namespace xng {
    class MeshStreamer {
    public:
        typedef size_t Handle;

        enum VertexAttribute {
            POSITION,
            NORMAL,
            TANGENT,
            BITANGENT,
            UV,
            BONE_INDEX,
            BONE_WEIGHT
        };

        struct Allocation {
            rg::Primitive primitive;

            size_t vertexCount;
            size_t indexCount;

            BufferStreamer<float[3]>::Handle positionsStreamHandle;
            BufferStreamer<float[3]>::Handle normalsStreamHandle;
            BufferStreamer<float[3]>::Handle tangentsStreamHandle;
            BufferStreamer<float[3]>::Handle bitangentsStreamHandle;
            BufferStreamer<float[2]>::Handle uvsStreamHandle;
            BufferStreamer<int[4]>::Handle boneIndicesStreamHandle;
            BufferStreamer<float[4]>::Handle boneWeightsStreamHandle;

            std::unordered_map<VertexAttribute, size_t> vertexBufferStreamOffsets;

            BufferStreamer<int>::Handle indexBufferStreamHandle;
            size_t indexBufferStreamOffset;
        };

        static size_t getVertexAttributeSize(const VertexAttribute attribute) {
            switch (attribute) {
                default:
                    return 4 * 3;
                case UV:
                    return 4 * 2;
                case BONE_INDEX:
                case BONE_WEIGHT:
                    return 4 * 4;
            }
        }

        Handle create();

        void upload(const Handle &handle, const Mesh &mesh, const std::unordered_map<std::string, size_t> &boneIndices);

        void destroy(const Handle &handle);

        bool isUploadComplete(const Handle &handle);

        void flush(const Handle &handle);

        const Allocation &getAllocation(const Handle &handle) const;

        std::unordered_map<VertexAttribute, rg::HeapResource<rg::VertexBuffer> > commit(rg::TransferContext &ctx);

    private:
        std::unordered_map<Handle, Allocation> allocations;

        BufferStreamer<float[3]> positionsStream;
        BufferStreamer<float[3]> normalsStream;
        BufferStreamer<float[3]> tangentsStream;
        BufferStreamer<float[3]> bitangentsStream;
        BufferStreamer<float[2]> uvsStream;
        BufferStreamer<int[4]> boneIndicesStream;
        BufferStreamer<float[4]> boneWeightsStream;

        BufferStreamer<int> indexBufferStream;
    };
}

#endif //XENGINE_MESHSTREAMER_HPP
