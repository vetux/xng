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

#ifndef XENGINE_STREAMTEXTURE_HPP
#define XENGINE_STREAMTEXTURE_HPP

#include <cstddef>

#include "xng/assets/image.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng {
    /**
     * Fixed resolution slab-based allocation using Array Texture.
     *
     * Cpu Buffer -> StreamBuffer -> Texture
     *
     * Texture slots can also be allocated and bound to shaders for writing instead of streaming from cpu. (Shadow Mapping, IBL)
     */
    class StreamTexture {
    public:
        /**
         * The index into the array texture.
         */
        typedef unsigned int Handle;

        explicit StreamTexture(const rg::Heap &heap, const Vec2i &resolution);

        ~StreamTexture() = default;

        Handle create();

        void destroy(const Handle &handle);

        void upload(const Handle &handle, const ImageRGBA &image);

        bool isUploadComplete(const Handle &handle);

        void flush(const Handle &handle);

        const rg::HeapResource<rg::Texture> &commit(rg::GraphBuilder &ctx);
    };
}

#endif //XENGINE_STREAMTEXTURE_HPP
