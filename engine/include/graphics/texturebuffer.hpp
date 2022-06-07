/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TEXTUREBUFFER_HPP
#define XENGINE_TEXTUREBUFFER_HPP

#include "graphics/renderobject.hpp"
#include "graphics/textureproperties.hpp"
#include "graphics/texturebufferdesc.hpp"
#include "graphics/texturebufferview.hpp"

#include "asset/image.hpp"

namespace xengine {
    /**
     * A texture buffer.
     */
    class XENGINE_EXPORT TextureBuffer : public RenderObject {
    public:
        ~TextureBuffer() override = default;

        virtual const TextureBufferDesc &getDescription() = 0;

        virtual std::unique_ptr<TextureBufferView> createView() = 0;

        /**
         * Create a view which references the given face of the cube map texture as a TEXTURE_2D.
         *
         * @param face
         * @return
         */
        virtual std::unique_ptr<TextureBufferView> createView(CubeMapFace face) = 0;

        /**
         * Upload the image buffer.
         *
         * The size of the buffer has to match the size in the texture description.
         *
         * @param buffer
         */
        virtual void upload(const Image<ColorRGB> &buffer) = 0;

        virtual void upload(const Image<ColorRGBA> &buffer) = 0;

        virtual void upload(const Image<float> &buffer) = 0;

        virtual void upload(const Image<int> &buffer) = 0;

        virtual void upload(const Image<char> &buffer) = 0;

        virtual void upload(const Image<unsigned char> &buffer) = 0;

        virtual Image<ColorRGBA> download() = 0;

        virtual void upload(CubeMapFace face, const Image<ColorRGBA> &buffer) = 0;

        virtual Image<ColorRGBA> download(CubeMapFace face) = 0;

        /**
         * Upload a cube map in the following format:
         * ....
         *
         * @param buffer
         */
        virtual void uploadCubeMap(const Image<ColorRGBA> &buffer) = 0;

        virtual Image<ColorRGBA> downloadCubeMap() = 0;
    };
}

#endif //XENGINE_TEXTUREBUFFER_HPP