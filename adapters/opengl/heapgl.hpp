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

#ifndef XENGINE_HEAPOGL_HPP
#define XENGINE_HEAPOGL_HPP

#include <cstring>
#include <thread>

#include "xng/rendergraph/heap.hpp"

#include "xng/display/window.hpp"
#include "xng/util/refcounter.hpp"

#include "colorbytesize.hpp"
#include "resourcescope.hpp"
#include "resource/buffergl.hpp"
#include "resource/framebuffer.hpp"

#include "glsl/shadercompilerglsl.hpp"
#include "context/transfercontextgl.hpp"

namespace xng::opengl {
    class HeapTransferContextGL;

    class HeapGL final : public Heap {
    public:
        explicit HeapGL();

        ~HeapGL() override;

        HeapResource<Buffer> allocateBuffer(const Buffer &desc) override;

        HeapResource<Texture> allocateTexture(const Texture &desc) override;

        std::unique_ptr<HeapMapping> map(const HeapResource<Buffer> &target) override;

        void incrementReference(const ResourceId &handle) override {
            refCounter.inc(handle.getHandle());
        }

        void decrementReference(const ResourceId &handle) override;

        const ResourceScope &getResources() const {
            return context->getResources().getHeap();
        }

    private:
        ResourceId::Handle allocateHandle() {
            if (freeHandles.empty()) return nextHandle++;
            const auto ret = freeHandles.back();
            freeHandles.pop_back();
            return ret;
        }

        void freeHandle(const ResourceId::Handle handle) {
            freeHandles.push_back(handle);
        }

        ResourceId::Handle nextHandle = 0;
        std::vector<ResourceId::Handle> freeHandles{};

        RefCounter<ResourceId::Handle, size_t> refCounter{};

        std::unique_ptr<TransferContextGL> context{};
    };
}

#endif //XENGINE_HEAPOGL_HPP
