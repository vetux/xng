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

#ifndef XENGINE_RENDERGRAPH_RESOURCE_HPP
#define XENGINE_RENDERGRAPH_RESOURCE_HPP

#include "xng/util/hashcombine.hpp"

#include "xng/rendergraph/resource/buffer.hpp"

namespace xng::rendergraph {
    /**
     * There are 2 resource ID namespaces with separate minting.
     *
     *  TRANSIENT: Resources local to the graph (ID minting handled by user / Builder class)
     *  HEAP: Resources inside the heap (ID minting handled by heap)
     */
    class ResourceId {
    public:
        typedef int Handle;

        enum NameSpace {
            TRANSIENT = 0,
            HEAP
        };

        ResourceId() = default;

        explicit ResourceId(const Handle handle, const NameSpace ns)
            : handle(handle), space(ns) {
        }

        [[nodiscard]] bool isAssigned() const {
            return handle >= 0;
        }

        [[nodiscard]] Handle getHandle() const {
            return handle;
        }

        [[nodiscard]] NameSpace getNameSpace() const {
            return space;
        }

        bool operator==(const ResourceId &other) const {
            return handle == other.handle && space == other.space;
        }

        bool operator!=(const ResourceId &other) const {
            return !(*this == other);
        }

    protected:
        Handle handle = -1;
        NameSpace space = TRANSIENT;
    };

    class ResourceIdHash {
    public:
        std::size_t operator()(const ResourceId &k) const {
            size_t ret = 0;
            hash_combine(ret, k.getHandle());
            hash_combine(ret, k.getNameSpace());
            return ret;
        }
    };

    template<typename T>
    class Resource : public ResourceId {
    public:
        Resource() = default;

        Resource(const Handle handle, T data)
            : ResourceId(handle, TRANSIENT), data(std::move(data)) {
        }

        Resource(const Handle handle, T data, const NameSpace scope)
            : ResourceId(handle, scope), data(std::move(data)) {
        }

        /**
         * Explicit construction of resource handles with compatible types.
         *
         * Used for e.g. Buffer -> VertexBuffer
         *
         * @tparam U
         * @param other
         */
        template<typename U>
        explicit Resource(const Resource<U> &other)
            : ResourceId(other.getHandle(), other.getNameSpace()), data(other.getData()) {
        }

        const T &getData() const {
            return data;
        }

        /**
         * Implicit conversion from a compatible type to Buffer.
         *
         * Used for e.g. VertexBuffer -> Buffer
         */
        operator Resource<Buffer> () const {
            return Resource<Buffer>(getHandle(), static_cast<Buffer>(data), getNameSpace());
        }

    protected:
        T data;
    };
}

#endif //XENGINE_RENDERGRAPH_RESOURCE_HPP
