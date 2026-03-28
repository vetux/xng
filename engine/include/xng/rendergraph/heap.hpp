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

#ifndef XENGINE_RENDERGRAPH_HEAP_HPP
#define XENGINE_RENDERGRAPH_HEAP_HPP

#include "xng/rendergraph/image.hpp"
#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/heapmapping.hpp"

#include "xng/rendergraph/context/transfercontext.hpp"

#include "xng/rendergraph/resource/indexbuffer.hpp"
#include "xng/rendergraph/resource/storagebuffer.hpp"
#include "xng/rendergraph/resource/texture.hpp"
#include "xng/rendergraph/resource/vertexbuffer.hpp"

namespace xng::rendergraph {
    template<typename T>
    class HeapResource;

    /**
     * The Heap manages persistent resource allocations.
     *
     * Only heap buffers can have a non-MEMORY_GPU_ONLY memory type.
     *
     * The heap exposes the mapping api for MEMORY_CPU_TO_GPU / MEMORY_GPU_TO_CPU buffers.
     *
     * Heap resources transfers can either be performed asynchronously through getTransferContext()
     * or blocking inside a TransferPass.
     *
     * The implementation controls the physical memory layout.
     *
     * Heap resource allocation is synchronous. create*()
     * Heap resource transfers are asynchronous. getTransferContext().*
     *
     * This enables users to asynchronously stream data to the gpu.
     *
     * On OpenGL this is implemented using glFenceSync / glClientWaitSync.
     * On Vulkan this is implemented using transfer queues + semaphores / fences.
     */
    class Heap {
    public:
        virtual ~Heap() = default;

        /**
         * Allocate a buffer.
         *
         * This is a synchronous operation. The returned resource handle can be used immediately.
         *
         * @param desc The description of the buffer to allocate
         * @return
         */
        virtual HeapResource<Buffer> allocateBuffer(const Buffer &desc) = 0;

        /**
         * Allocate a texture.
         *
         * This is a synchronous operation. The returned resource handle can be used immediately.
         *
         * @param desc The description of the texture to allocate
         * @return
         */
        virtual HeapResource<Texture> allocateTexture(const Texture &desc) = 0;

        /**
         * Map a MEMORY_CPU_TO_GPU or MEMORY_GPU_TO_CPU buffer into process memory.
         *
         * The pointer can be directly accessed to perform readback or writes.
         *
         * @param target
         * @return
         */
        virtual HeapMapping mapBuffer(const HeapResource<Buffer> &target) = 0;

        /**
         * Increment the reference counter for the passed resource.
         *
         * Used by the HeapResource RAII wrapper.
         *
         * @param handle
         */
        virtual void incrementReference(const ResourceId &handle) = 0;

        /**
         * Decrement the reference counter for the passed resource.
         *
         * The resource is deallocated when the counter reaches 0.
         *
         * Used by the HeapResource RAII wrapper.
         *
         * @param handle
         */
        virtual void decrementReference(const ResourceId &handle) = 0;

        /**
         * This method checks whether all pending transfers have finished.
         *
         * Heap transfers are asynchronous.
         *
         * The runtime will wait for pending transfers to complete when accessing a heap-resource in a graph.
         *
         * @param handle The resource to check.
         * @return True if there are pending transfers and the runtime would block on access until the transfers are complete.
         */
        virtual bool hasPendingTransfers(const ResourceId &handle) = 0;

        /**
         * All transfer operations on the returned context are executed asynchronously.
         *
         * The runtime will wait for pending transfers of any resources referenced
         * in a Runtime.execute() call before executing the graph.
         *
         * Users can poll transfer status via hasPendingTransfers().
         *
         * @return The transfer context for heap transfers.
         */
        virtual TransferContext &getTransferContext() = 0;

        /**
         * Download the data from the gpu side shader buffer.
         *
         * This is a convenience method.
         *
         * This will internally use staging buffers on platforms that don't have native "download from gpu" api (E.g., Vulkan).
         *
         * @param buffer
         * @return
         */
        virtual std::vector<uint8_t> downloadStorageBuffer(const HeapResource<StorageBuffer> &buffer) = 0;

        /**
         * Download the data from the gpu side texture.
         *
         * This is a convenience method.
         *
         * This will internally use staging buffers on platforms that don't have native "download from gpu" api (E.g., Vulkan).
         *
         * @param texture
         * @param index
         * @param mipMapLevel
         * @param face
         * @return
         */
        virtual Image<ColorRGBA> downloadTextureBuffer(const HeapResource<Texture> &texture,
                                                       size_t index,
                                                       size_t mipMapLevel,
                                                       CubeMapFace face) = 0;

        /**
         * Type safe vertex buffer allocation wrapper.
         *
         * @param desc
         * @return
         */
        HeapResource<VertexBuffer> allocateVertexBuffer(const VertexBuffer &desc);

        /**
         * Type safe index buffer allocation wrapper.
         *
         * @param desc
         * @return
         */
        HeapResource<IndexBuffer> allocateIndexBuffer(const IndexBuffer &desc);

        /**
         * Type safe storage buffer allocation wrapper.
         *
         * @param desc
         * @return
         */
        HeapResource<StorageBuffer> allocateStorageBuffer(const StorageBuffer &desc);
    };

    template<typename T>
    class HeapResource : public Resource<T> {
    public:
        HeapResource()
            : Resource<T>(), heap(nullptr) {
        }

        HeapResource(ResourceId::Handle handle, T value, Heap &heap)
            : Resource<T>(handle, value, ResourceId::HEAP), heap(&heap) {
            heap.incrementReference(*this);
        }

        HeapResource(const HeapResource &other)
            : Resource<T>(other.getHandle(), other.getData(), ResourceId::HEAP), heap(other.heap) {
            if (heap != nullptr) {
                heap->incrementReference(*this);
            }
        }

        HeapResource(HeapResource &&other) noexcept
            : Resource<T>(std::move(other)), heap(other.heap) {
            other.heap = nullptr;
        }

        /**
         * Explicit construction of heap resource handles with compatible types.
         *
         * Used for e.g. Buffer -> VertexBuffer
         *
         * @tparam U
         * @param other
         */
        template<typename U>
        explicit HeapResource(const HeapResource<U> &other)
            : Resource<T>(other.getHandle(), other.getData(), ResourceId::HEAP), heap(other.getHeap()) {
        }

        ~HeapResource() {
            if (heap != nullptr) {
                heap->decrementReference(*this);
            }
        }

        HeapResource &operator=(const HeapResource &other) {
            if (this == &other) return *this;

            if (heap != nullptr) {
                heap->decrementReference(*this);
            }
            Resource<T>::operator=(other);
            heap = other.heap;
            if (heap != nullptr) {
                heap->incrementReference(*this);
            }
            return *this;
        }

        HeapResource &operator=(HeapResource &&other) noexcept {
            if (heap != nullptr) {
                heap->decrementReference(*this);
            }
            heap = other.heap;
            Resource<T>::operator=(std::move(other));
            other.heap = nullptr;
            return *this;
        }

        /**
         * Implicit conversion from a compatible type to Buffer.
         *
         * Used for e.g. VertexBuffer -> Buffer
         */
        operator HeapResource<Buffer>() const {
            if (heap == nullptr) throw std::runtime_error("Nullptr heap in resource cast");
            return HeapResource<Buffer>(Resource<T>::getHandle(),
                                        static_cast<Buffer>(Resource<T>::getData()),
                                        *heap);
        }

    private:
        Heap *heap = nullptr;
    };

    template<typename T>
    class HeapResourceHash {
        std::size_t operator()(const HeapResource<T> &k) const {
            size_t ret = 0;
            hash_combine(ret, k.getHandle());
            hash_combine(ret, k.getScope());
            return ret;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_HEAP_HPP
