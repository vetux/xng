/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREBUFFER_HPP
#define XENGINE_TEXTUREBUFFER_HPP

#include <memory>

#include "renderbuffer.hpp"
#include "textureproperties.hpp"
#include "texturebufferdesc.hpp"
#include "gpufence.hpp"

#include "xng/asset/image.hpp"

namespace xng {
    /**
     * A bindable texture buffer eg sampler2D in glsl
     */
    class XENGINE_EXPORT TextureBuffer : public RenderBuffer {
    public:
        ~TextureBuffer() override = default;

        virtual const TextureBufferDesc &getDescription() = 0;

        RenderBufferType getBufferType() override {
            return getDescription().bufferType;
        }

        Type getType() override {
            return RENDER_OBJECT_TEXTURE_BUFFER;
        }

        /**
         * Upload the image buffer.
         *
         * The size and format of the buffer has to match the texture description.
         *
         * @param format The format of the data pointed at by buffer
         * @param buffer
         * @param bufferSize
         * @return
         */
        virtual std::unique_ptr<GpuFence> upload(ColorFormat format, const uint8_t *buffer, size_t bufferSize) = 0;

        virtual Image<ColorRGBA> download() = 0;

        virtual std::unique_ptr<GpuFence> upload(CubeMapFace face,
                                                 ColorFormat format,
                                                 const uint8_t *buffer,
                                                 size_t bufferSize) = 0;

        virtual Image<ColorRGBA> download(CubeMapFace face) = 0;

        std::unique_ptr<GpuFence> upload(const Image<ColorRGBA> &image) {
            return upload(RGBA,
                          reinterpret_cast<const uint8_t *>(image.getData()),
                          image.getDataSize() * sizeof(ColorRGBA));
        }
    };
}

#endif //XENGINE_TEXTUREBUFFER_HPP