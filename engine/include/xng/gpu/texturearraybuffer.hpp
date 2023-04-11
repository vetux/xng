/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TEXTUREARRAYBUFFER_HPP
#define XENGINE_TEXTUREARRAYBUFFER_HPP

#include <memory>

#include "xng/asset/image.hpp"

#include "xng/gpu/renderbuffer.hpp"
#include "xng/gpu/texturearraybufferdesc.hpp"

namespace xng {
    /**
     * Represents a bindable texture array containing textures of the same type eg sampler2DArray in glsl.
     */
    class TextureArrayBuffer : public RenderObject {
    public:
        ~TextureArrayBuffer() override = default;

        virtual const TextureArrayBufferDesc &getDescription() = 0;

        Type getType() override {
            return RENDER_OBJECT_TEXTURE_ARRAY_BUFFER;
        }

        virtual std::unique_ptr<GpuFence> copy(TextureArrayBuffer &source) = 0;

        /**
         * Upload the image buffer.
         *
         * The size and format of the buffer has to match the texture description.
         *
         * @param index
         * @param format The format of the data pointed at by buffer
         * @param buffer
         * @param bufferSize
         * @return
         */
        virtual std::unique_ptr<GpuFence> upload(size_t index,
                                                 ColorFormat format,
                                                 const uint8_t *buffer,
                                                 size_t bufferSize) = 0;

        virtual Image<ColorRGBA> download(size_t index) = 0;

        std::unique_ptr<GpuFence> upload(size_t index,
                                         const Image<ColorRGBA> &image) {
            return upload(index,
                          RGBA,
                          reinterpret_cast<const uint8_t *>(image.getData()),
                          image.getDataSize() * sizeof(ColorRGBA));
        }
    };
}
#endif //XENGINE_TEXTUREARRAYBUFFER_HPP
