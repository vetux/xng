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

#ifndef XENGINE_TEXTUREBUFFER_HPP
#define XENGINE_TEXTUREBUFFER_HPP

#include <memory>

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/textureproperties.hpp"
#include "xng/gpu/texturebufferdesc.hpp"
#include "xng/gpu/command.hpp"

#include "xng/asset/image.hpp"

namespace xng {
    /**
     * A bindable texture buffer eg sampler2D in glsl
     */
    class XENGINE_EXPORT TextureBuffer : public RenderObject {
    public:
        ~TextureBuffer() override = default;

        Type getType() override {
            return RENDER_OBJECT_TEXTURE_BUFFER;
        }

        virtual const TextureBufferDesc &getDescription() = 0;

        Command copy(TextureBuffer &source) {
            return {Command::COPY_TEXTURE, TextureBufferCopy(&source, this)};
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
        virtual void upload(ColorFormat format, const uint8_t *buffer, size_t bufferSize) = 0;

        virtual void upload(CubeMapFace face,
                            ColorFormat format,
                            const uint8_t *buffer,
                            size_t bufferSize) = 0;

        void upload(const Image<ColorRGBA> &image) {
            return upload(RGBA,
                          reinterpret_cast<const uint8_t *>(image.getData()),
                          image.getDataSize() * sizeof(ColorRGBA));
        }

        virtual Image<ColorRGBA> download() = 0;

        virtual Image<ColorRGBA> download(CubeMapFace face) = 0;
    };
}

#endif //XENGINE_TEXTUREBUFFER_HPP