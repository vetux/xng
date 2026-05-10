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

#ifndef XENGINE_HEAPMAPPINGGL_HPP
#define XENGINE_HEAPMAPPINGGL_HPP

#include "xng/rendergraph/heapmapping.hpp"
#include "xng/rendergraph/heap.hpp"

#include "resource/buffergl.hpp"

namespace xng::opengl {
    class HeapMappingGL final : public HeapMapping {
    public:
        HeapMappingGL(const rg::HeapResource<rg::Buffer> &resourceHandle,
                      BufferGL &buffer)
            : resourceHandle(resourceHandle), buffer(buffer) {
            ptr = buffer.map();
        }

        ~HeapMappingGL() override {
            buffer.unmap();
        }

        uint8_t *data() override {
            return ptr;
        }

        size_t size() override {
            return buffer.desc.size;
        }

    private:
        rg::HeapResource<rg::Buffer> resourceHandle;
        BufferGL &buffer;
        uint8_t *ptr = nullptr;
    };
}

#endif //XENGINE_HEAPMAPPINGGL_HPP
