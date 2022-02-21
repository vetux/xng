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

#ifndef XENGINE_OGLTEXTUREBUFFER_HPP
#define XENGINE_OGLTEXTUREBUFFER_HPP

#include "graphics/texturebuffer.hpp"

#include "openglinclude.hpp"

namespace xengine {
    namespace opengl {
        class OGLTextureBuffer : public TextureBuffer {
        public:
            GLuint handle;

            explicit OGLTextureBuffer(Attributes attributes);

            OGLTextureBuffer(const OGLTextureBuffer &copy) = delete;

            OGLTextureBuffer &operator=(const OGLTextureBuffer &copy) = delete;

            ~OGLTextureBuffer() override;

            void upload(const Image<ColorRGB> &buffer) override;

            void upload(const Image<ColorRGBA> &buffer) override;

            void upload(const Image<float> &buffer) override;

            void upload(const Image<int> &buffer) override;

            void upload(const Image<char> &buffer) override;

            void upload(const Image<unsigned char> &buffer) override;

            Image<ColorRGBA> download() override;

            void upload(CubeMapFace face, const Image<ColorRGBA> &buffer) override;

            Image<ColorRGBA> download(CubeMapFace face) override;

            void uploadCubeMap(const Image<ColorRGBA> &buffer) override;

            Image<ColorRGBA> downloadCubeMap() override;

        private:
            void setTextureType(TextureType type);
        };
    }
}

#endif //XENGINE_OGLTEXTUREBUFFER_HPP
