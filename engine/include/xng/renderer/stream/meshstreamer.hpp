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

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/buffer.hpp"

#include "xng/renderer/vertexattribute.hpp"
#include "xng/renderer/stream/bufferstreamer.hpp"

namespace xng {
    class MeshStreamer {
    public:
        typedef size_t Handle;

        struct Allocation {
            rg::Primitive primitive;
            rg::DrawCall drawCall;
            bool indexed;
            size_t indexOffset; // The offset applied to each index.
        };

        explicit MeshStreamer(rg::Heap &heap);

        Handle create();

        /**
         * Pads default values for meshes that don't define a given attribute.
         * Positions must be defined.
         *
         * The index
         *
         * @param handle
         * @param mesh
         * @param boneIndices
         */
        void upload(const Handle &handle,
                    const Mesh &mesh,
                    const std::unordered_map<std::string, unsigned int> &boneIndices);

        void destroy(const Handle &handle);

        bool isUploadComplete(const Handle &handle);

        void flush(const Handle &handle);

        const Allocation &getAllocation(const Handle &handle) const;

        std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > commitVertexBuffers(rg::GraphBuilder &ctx);

        rg::HeapResource<rg::Buffer> commitIndexBuffer(rg::GraphBuilder &ctx);

    private:
        std::unordered_map<Handle, Allocation> allocations;
        std::unordered_map<VertexAttribute, StreamBuffer> vertexBuffers;
        StreamBuffer indexBufferStream;
    };
}

#endif //XENGINE_MESHSTREAMER_HPP
