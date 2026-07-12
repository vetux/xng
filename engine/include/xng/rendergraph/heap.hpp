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

#ifndef XENGINE_RENDERGRAPH_HEAP_HPP
#define XENGINE_RENDERGRAPH_HEAP_HPP

#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/heapmapping.hpp"
#include "xng/rendergraph/semaphore.hpp"
#include "xng/rendergraph/pass.hpp"

#include "xng/rendergraph/resource/buffer.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng::rg {
    template<typename T>
    class HeapResource;

    /**
     * The Heap manages persistent resource allocations.
     *
     * Only heap buffers can have a non-MEMORY_GPU_ONLY memory type.
     *
     * The heap exposes the mapping api for MEMORY_CPU_TO_GPU / MEMORY_GPU_TO_CPU buffers.
     *
     * Heap resources transfers can either be performed asynchronously through transfer() or inside a TransferPass submitted to Runtime::execute.
     *
     * The implementation controls the physical memory layout.
     *
     * On OpenGL this is implemented using a separate thread with a subcontext + glFenceSync / glClientWaitSync.
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
         * The returned mapping must only be accessed by the thread which mapped it.
         *
         * Ranges of the returned mapping which are currently in use by a transfer or Runtime::execute invocation
         * must not be accessed until the execution has finished.
         *
         * There is no way for the runtime to guarantee synchronization safety in this case,
         * so users must exercise caution when using mapped resources.
         *
         * The returned mapping internally holds a reference to the passed resource.
         *
         * @param target
         * @return
         */
        virtual std::unique_ptr<HeapMapping> map(const HeapResource<Buffer> &target) = 0;

        //TODO: Drop Heap::transfer() in favour of Runtime::execute()
        /**
         * Perform transfer operations on heap resources in the heap transfer context.
         *
         * Heap transfer operations run asynchronously and can be checked for completion via the returned transfer handle.
         *
         * The runtime internally pins resources referenced in the pass until the transfer has finished.
         *
         * For the passes the ordering and callback rules from the graph apply.
         * The runtime will treat a heap transfer submission like any other Runtime::execute() submission
         * and order / synchronize / overlap separate invocations accordingly.
         *
         * On Vulkan all resources are EXCLUSIVE ownership.
         *
         * This means if a resource is being accessed in a transfer pass on the heap, the runtime
         * may perform an ownership transfer of the resource when it is referenced in a graph.
         *
         * Therefore, for streaming resources to the heap for consumption by graphs the best approach is:
         *
         * Staging Buffer on Heap (MEMORY_CPU_TO_GPU)
         *  -> Upload data to the staging buffer from cpu
         * Intermediate Buffer on Heap (MEMORY_GPU_ONLY, Contains data currently being streamed (Double Buffered))
         *  -> Copy Staging buffer to intermediate buffer in heap transfer pass
         * Big Stable Buffer on Heap (MEMORY_GPU_ONLY, Contains all the streamed data)
         *  -> Copy Intermediate Buffer to stable buffer in runtime transfer pass. (Only when the copy from staging to intermediate has finished)
         *
         * The double buffering of the intermediate buffer ensures that the runtime can overlap streaming with graph execution.
         *
         * @param passes The passes containing the transfer operations
         * @return The semaphore representing the pending transfer operations.
         */
        virtual std::unique_ptr<Semaphore> transfer(const std::vector<TransferPass> &passes) = 0;

    private:
        virtual void incrementReference(const ResourceId &handle) = 0;

        virtual void decrementReference(const ResourceId &handle) = 0;

        template<typename T>
        friend class HeapResource;
    };

    template<typename T>
    class HeapResource : public Resource<T> {
    public:
        HeapResource()
            : Resource<T>() {
        }

        HeapResource(ResourceId::Handle handle, T value, Heap &heap)
            : Resource<T>(handle, value, ResourceId::HEAP), heap(&heap) {
            heap.incrementReference(*this);
        }

        ~HeapResource() {
            if (heap != nullptr) {
                heap->decrementReference(*this);
            }
        }

        // Copy / Move Semantics
        HeapResource(const HeapResource &other)
            : Resource<T>(other.getHandle(), other.getDescription(), ResourceId::HEAP), heap(other.heap) {
            if (heap != nullptr) {
                heap->incrementReference(*this);
            }
        }

        HeapResource(HeapResource &&other) noexcept
            : Resource<T>(std::move(other)), heap(other.heap) {
            other.heap = nullptr;
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
