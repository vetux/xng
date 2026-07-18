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

namespace xng::opengl {
    HeapGL::HeapGL()
        : context(std::make_unique<TransferContextGL>()) {
    }

    HeapGL::~HeapGL() = default;

    HeapResource<Buffer> HeapGL::allocateBuffer(const Buffer &desc) {
        auto ret = HeapResource(allocateHandle(), desc, *this);
        context->getResources().getHeap().buffers[ret.getHandle()] = std::make_shared<BufferGL>(desc);
        return ret;
    }

    HeapResource<Texture> HeapGL::allocateTexture(const Texture &desc) {
        auto ret = HeapResource(allocateHandle(), desc, *this);
        context->getResources().getHeap().textures[ret.getHandle()] = std::make_shared<TextureGL>(desc);
        return ret;
    }

    std::unique_ptr<HeapMapping> HeapGL::map(const HeapResource<Buffer> &target) {
        return std::make_unique<HeapMappingGL>(target,
                                               *context->getResources().getHeap().buffers.at(target.getHandle()));
    }

    void HeapGL::decrementReference(const ResourceId &handle) {
        if (refCounter.dec(handle.getHandle())) {
            context->getResources().getHeap().buffers.erase(handle.getHandle());
        }
    }
}
