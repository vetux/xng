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

#ifndef XENGINE_HEAPOGL_HPP
#define XENGINE_HEAPOGL_HPP

#include <cstring>
#include <thread>

#include "xng/rendergraph/heap.hpp"

#include "colorbytesize.hpp"
#include "resourcescope.hpp"
#include "context/heaptransfercontextgl.hpp"
#include "glsl/shadercompilerglsl.hpp"
#include "resource/buffergl.hpp"
#include "resource/framebuffer.hpp"
#include "xng/display/window.hpp"
#include "xng/rendergraph/statistics.hpp"
#include "xng/util/refcounter.hpp"

namespace xng::opengl {
    class HeapTransferContextGL;

    class HeapGL final : public Heap {
    public:
        explicit HeapGL(std::unique_ptr<Window> heapWindow);

        ~HeapGL() override;

        HeapResource<Buffer> allocateBuffer(const Buffer &desc) override;

        HeapResource<Texture> allocateTexture(const Texture &desc) override;

        HeapMapping mapBuffer(const HeapResource<Buffer> &target) override;

        bool hasPendingTransfers(const ResourceId &handle) override;

        TransferContext &getTransferContext() override;

        std::vector<uint8_t> downloadStorageBuffer(const HeapResource<StorageBuffer> &buffer) override;

        Image<ColorRGBA> downloadTexture(const HeapResource<Texture> &texture,
                                         size_t index,
                                         size_t mipMapLevel,
                                         CubeMapFace face) override;

        void incrementReference(const ResourceId &handle) override {
            refCounter.inc(handle.getHandle());
        }

        void decrementReference(const ResourceId &handle) override;

        Statistics &getStatistics() {
            return stats;
        }

        ResourceScope getResources() const {
            return transferContext->getResources();
        }

        HeapTransferContextGL& getTransferContextGL() const {
            return *transferContext;
        }

    private:
        Statistics stats;

        RefCounter<ResourceId::Handle, size_t> refCounter{};

        std::unique_ptr<HeapTransferContextGL> transferContext;
    };
}

#endif //XENGINE_HEAPOGL_HPP
