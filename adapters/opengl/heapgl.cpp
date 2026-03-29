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

#include "heapgl.hpp"

#include "passresources.hpp"
#include "context/heaptransfercontextgl.hpp"

namespace xng::opengl {
    HeapGL::HeapGL(std::unique_ptr<Window> heapWindow)
        : transferContext(std::make_unique<HeapTransferContextGL>(std::move(heapWindow))) {
    }

    HeapGL::~HeapGL() = default;

    HeapResource<Buffer> HeapGL::allocateBuffer(const Buffer &desc) {
        return HeapResource(transferContext->allocateBuffer(desc), desc, *this);
    }

    HeapResource<Texture> HeapGL::allocateTexture(const Texture &desc) {
        return HeapResource(transferContext->allocateTexture(desc), desc, *this);
    }

    HeapMapping HeapGL::mapBuffer(const HeapResource<Buffer> &target) {
        auto &b = transferContext->getBuffer(target.getHandle());
        return HeapMapping(b.map(), b.desc.size, [b]() {
            b.unmap();
        });
    }

    bool HeapGL::hasPendingTransfers(const ResourceId &handle) {
        return transferContext->hasPendingTransfers(handle.getHandle());
    }

    TransferContext &HeapGL::getTransferContext() {
        return *transferContext;
    }

    std::vector<uint8_t> HeapGL::downloadStorageBuffer(const HeapResource<StorageBuffer> &buffer) {
        throw std::runtime_error("downloadTextureBuffer not implemented");
    }

    Image<ColorRGBA> HeapGL::downloadTexture(const HeapResource<Texture> &texture,
                                                   size_t index,
                                                   size_t mipMapLevel,
                                                   CubeMapFace face) {
        throw std::runtime_error("downloadTextureBuffer not implemented");
    }

    void HeapGL::decrementReference(const ResourceId &handle) {
        refCounter.dec(handle.getHandle());
        if (refCounter.get(handle.getHandle()) == 0) {
            transferContext->free(handle.getHandle());
        }
    }
}
