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

#ifndef XENGINE_OGLTEXTUREBUFFER_HPP
#define XENGINE_OGLTEXTUREBUFFER_HPP

#include "xng/gpu/texturebuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLTextureBuffer : public TextureBuffer {
    public:
        std::function<void(RenderObject*)> destructor;
        TextureBufferDesc desc;
        GLuint handle = 0;

        GLenum textureType;

        OGLTextureBuffer(std::function<void(RenderObject*)> destructor, TextureBufferDesc descArg)
                : destructor(std::move(destructor)), desc(std::move(descArg)) {
            textureType = convert(desc.textureType);

            glGenTextures(1, &handle);
            glBindTexture(textureType, handle);

            if (textureType != GL_TEXTURE_2D_MULTISAMPLE) {
                glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
                glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
            }
            checkGLError();

            if (desc.textureType == TEXTURE_2D) {
                GLint texInternalFormat = convert(desc.format);
                GLuint texFormat = GL_RGBA;

                if (desc.format >= R8I) {
                    texFormat = GL_RGBA_INTEGER; //Integer formats require _INTEGER format
                }

                GLuint texType = GL_UNSIGNED_BYTE;

                if (desc.format == ColorFormat::DEPTH) {
                    texInternalFormat = GL_DEPTH;
                    texFormat = GL_DEPTH_COMPONENT;
                    texType = GL_FLOAT;
                } else if (desc.format == ColorFormat::DEPTH_STENCIL) {
                    texInternalFormat = GL_DEPTH24_STENCIL8;
                    texFormat = GL_DEPTH_STENCIL;
                    texType = GL_UNSIGNED_INT_24_8;
                }

                glTexImage2D(textureType,
                             0,
                             texInternalFormat,
                             desc.size.x,
                             desc.size.y,
                             0,
                             texFormat,
                             texType,
                             nullptr);
            } else if (desc.textureType == TEXTURE_2D_MULTISAMPLE) {
                GLuint texInternalFormat = convert(desc.format);

                if (desc.format == ColorFormat::DEPTH) {
                    texInternalFormat = GL_DEPTH;
                } else if (desc.format == ColorFormat::DEPTH_STENCIL) {
                    texInternalFormat = GL_DEPTH24_STENCIL8;
                }

                glTexImage2DMultisample(textureType,
                                        desc.samples,
                                        texInternalFormat,
                                        desc.size.x,
                                        desc.size.y,
                                        desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);
            } else {
                for (unsigned int i = 0; i < 6; i++) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0,
                                 numeric_cast<GLint>(convert(desc.format)),
                                 desc.size.x,
                                 desc.size.y,
                                 0,
                                 GL_RGBA,
                                 GL_UNSIGNED_BYTE,
                                 nullptr);
                }
            }
            checkGLError();

            if (textureType != GL_TEXTURE_2D_MULTISAMPLE && desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            }

            glBindTexture(textureType, 0);

            checkGLError();
        }

        ~OGLTextureBuffer() override {
            glDeleteTextures(1, &handle);
            destructor(this);
        }

        const TextureBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(ColorFormat format, const uint8_t *buffer, size_t bufferSize) override {
            if (desc.textureType == TEXTURE_CUBE_MAP) {
                throw std::runtime_error(
                        "Attempted to upload texture on cube map texture without specifying a target face.");
            }

            glBindTexture(GL_TEXTURE_2D, handle);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         convert(desc.format),
                         desc.size.x,
                         desc.size.y,
                         0,
                         convert(format),
                         GL_UNSIGNED_BYTE,
                         buffer);

            if (textureType != GL_TEXTURE_2D_MULTISAMPLE && desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            }

            glBindTexture(GL_TEXTURE_2D, 0);

            checkGLError();

            return std::make_unique<OGLFence>();
        }

        std::unique_ptr<GpuFence> upload(CubeMapFace face,
                                         ColorFormat format,
                                         const uint8_t *buffer,
                                         size_t bufferSize) override {
            if (desc.bufferType != HOST_VISIBLE) {
                throw std::runtime_error("Upload called on non host visible buffer.");
            }

            if (desc.textureType != TEXTURE_CUBE_MAP) {
                throw std::runtime_error("Attempted to upload a cube map face on a non cube map texture");
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
            glTexImage2D(convert(face),
                         0,
                         convert(desc.format),
                         desc.size.x,
                         desc.size.y,
                         0,
                         convert(format),
                         GL_UNSIGNED_BYTE,
                         buffer);

            if (desc.textureType != TEXTURE_2D_MULTISAMPLE && desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            checkGLError();
            return std::make_unique<OGLFence>();
        }

        Image<ColorRGBA> download() override {
            if (desc.textureType == TEXTURE_CUBE_MAP)
                throw std::runtime_error(
                        "Attempted to download a cube map texture without specifying the target face.");

            auto ret = ImageRGBA(desc.size);
            glBindTexture(GL_TEXTURE_2D, handle);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) ret.getData());
            glBindTexture(GL_TEXTURE_2D, 0);
            checkGLError();
            ret = ret.swapColumns();
            return ret;
        }

        Image<ColorRGBA> download(CubeMapFace face) override {
            if (desc.textureType != TEXTURE_CUBE_MAP)
                throw std::runtime_error("Attempted to download face of a non cube map texture.");

            auto ret = ImageRGBA(desc.size);
            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
            glGetTexImage(convert(face), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) ret.getData());
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            checkGLError();
            ret = ret.swapColumns();
            return ret;
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &other) override {
            auto &src = dynamic_cast<OGLTextureBuffer &>(other);
            glCopyImageSubData(src.handle,
                               GL_TEXTURE_2D,
                               0,
                               0,
                               0,
                               0,
                               handle,
                               GL_TEXTURE_2D,
                               0,
                               0,
                               0,
                               0,
                               src.desc.size.x,
                               src.desc.size.y,
                               1);
            checkGLError();
            return std::make_unique<OGLFence>();
        }

        size_t getSize() override {
            return desc.size.x * desc.size.y * getTextureFormatUnitSize();
        }

    protected:
        size_t getTextureFormatUnitSize() const {
            // Estimate
            switch (desc.format) {
                default:
                    throw std::runtime_error("Invalid texture format");
                case DEPTH:
                case DEPTH_STENCIL:
                    return 2;
                case R:
                    return 4;
                case RG:
                    return 8;
                case RGB:
                    return 12;
                case RGBA:
                    return 16;
                case R_COMPRESSED:
                    return 4;
                case RG_COMPRESSED:
                    return 8;
                case RGB_COMPRESSED:
                    return 12;
                case RGBA_COMPRESSED:
                    return 16;
                case R8:
                    return 1;
                case RG8:
                    return 2;
                case RGB8:
                    return 3;
                case RGBA8:
                    return 4;
                case R16:
                    return 2;
                case RG16:
                    return 4;
                case RGB16:
                    return 8;
                case RGBA16:
                    return 10;
                case RGB12:
                    return 5;
                case RGBA12:
                    return 6;
                case RGB10:
                    return 4;
                case R16F:
                    return 2;
                case RG16F:
                    return 4;
                case RGB16F:
                    return 6;
                case RGBA16F:
                    return 8;
                case R32F:
                    return 4;
                case RG32F:
                    return 8;
                case RGB32F:
                    return 12;
                case RGBA32F:
                    return 16;
                case R8I:
                    return 1;
                case RG8I:
                    return 2;
                case RGB8I:
                    return 3;
                case RGBA8I:
                    return 4;
                case R16I:
                    return 2;
                case RG16I:
                    return 4;
                case RGB16I:
                    return 6;
                case RGBA16I:
                    return 8;
                case R32I:
                    return 4;
                case RG32I:
                    return 8;
                case RGB32I:
                    return 12;
                case RGBA32I:
                    return 16;
                case R8UI:
                    return 1;
                case RG8UI:
                    return 2;
                case RGB8UI:
                    return 3;
                case RGBA8UI:
                    return 4;
                case R16UI:
                    return 2;
                case RG16UI:
                    return 4;
                case RGB16UI:
                    return 6;
                case RGBA16UI:
                    return 8;
                case R32UI:
                    return 4;
                case RG32UI:
                    return 8;
                case RGB32UI:
                    return 12;
                case RGBA32UI:
                    return 16;
            }
        }
    };
}

#endif //XENGINE_OGLTEXTUREBUFFER_HPP
