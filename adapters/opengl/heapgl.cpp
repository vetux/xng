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

#include "heapgl.hpp"

#include "heapmappinggl.hpp"
#include "passresources.hpp"
#include "context/heaptransfercontextgl.hpp"

namespace xng::opengl {
    HeapGL::HeapGL(std::unique_ptr<Window> heapWindow)
        : transferContext(std::make_unique<HeapTransferContextGL>(std::move(heapWindow), *this)) {
    }

    HeapGL::~HeapGL() = default;

    HeapResource<Buffer> HeapGL::allocateBuffer(const Buffer &desc) {
        return HeapResource(transferContext->allocateBuffer(desc), desc, *this);
    }

    HeapResource<Texture> HeapGL::allocateTexture(const Texture &desc) {
        return HeapResource(transferContext->allocateTexture(desc), desc, *this);
    }

    std::unique_ptr<HeapMapping> HeapGL::map(const HeapResource<Buffer> &target) {
        return std::make_unique<HeapMappingGL>(target, transferContext->getBuffer(target.getHandle()));
    }

    std::unique_ptr<Semaphore> HeapGL::transfer(const std::vector<TransferPass> &passes) {
        for (auto &pass: passes) {
            pass.callback(*transferContext);
        }
        return transferContext->finishTransfer();
    }

    void HeapGL::decrementReference(const ResourceId &handle) {
        if (refCounter.dec(handle.getHandle())) {
            transferContext->free(handle.getHandle());
        }
    }
}
