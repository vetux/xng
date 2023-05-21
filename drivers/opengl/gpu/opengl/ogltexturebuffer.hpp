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
        std::function<void(RenderObject *)> destructor;
        TextureBufferDesc desc;
        GLuint handle = 0;

        GLenum textureType;
        GLint texInternalFormat;

        OGLTextureBuffer(std::function<void(RenderObject *)> destructor, TextureBufferDesc descArg)
                : destructor(std::move(destructor)), desc(std::move(descArg)) {
            textureType = convert(desc.textureType);

            glGenTextures(1, &handle);
            glBindTexture(textureType, handle);

            checkGLError();

            if (desc.textureType == TEXTURE_2D) {
                texInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        texInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        texInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        texInternalFormat = GL_R8;
                        break;
                    case RG:
                        texInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        texInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        texInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage2D(textureType,
                               desc.generateMipmap
                               ? static_cast<GLsizei>( std::log2(std::max(desc.size.x, desc.size.y))) + 1
                               : 1,
                               texInternalFormat,
                               desc.size.x,
                               desc.size.y);

                try {
                    checkGLError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage2D(textureType,
                                       1,
                                       texInternalFormat,
                                       desc.size.x,
                                       desc.size.y);
                    } else {
                        throw e;
                    }
                }
            } else if (desc.textureType == TEXTURE_2D_MULTISAMPLE) {
                texInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        texInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        texInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        texInternalFormat = GL_R8;
                        break;
                    case RG:
                        texInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        texInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        texInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage2DMultisample(textureType,
                                          desc.generateMipmap
                                          ? static_cast<GLsizei>( std::log2(std::max(desc.size.x, desc.size.y))) + 1
                                          : 1,
                                          texInternalFormat,
                                          desc.size.x,
                                          desc.size.y,
                                          desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);

                try {
                    checkGLError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point
                    // and create only one layer when this happens.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage2DMultisample(textureType,
                                                  1,
                                                  texInternalFormat,
                                                  desc.size.x,
                                                  desc.size.y,
                                                  desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);
                    } else {
                        throw e;
                    }
                }
            } else {
                for (unsigned int i = 0; i < 6; i++) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                 0,
                                 static_cast<GLint>(convert(desc.format)),
                                 desc.size.x,
                                 desc.size.y,
                                 0,
                                 GL_RGBA,
                                 GL_UNSIGNED_BYTE,
                                 nullptr);
                }
            }

            checkGLError();

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            if (desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
            }

            auto col = desc.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

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

            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            0,
                            0,
                            desc.size.x,
                            desc.size.y,
                            convert(format),
                            GL_UNSIGNED_BYTE,
                            buffer);

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            if (desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
            }
            checkGLError();

            auto col = desc.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

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

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            if (desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
            }

            auto col = desc.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

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

        std::unique_ptr<GpuFence> copy(TextureBuffer &other) override {
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

            glBindTexture(textureType, handle);

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            if (desc.generateMipmap) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipmapFilter));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
                glGenerateMipmap(textureType);
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
                glTexParameteri(textureType,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.filterMag));
            }

            auto col = desc.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindTexture(textureType, 0);

            checkGLError();

            return std::make_unique<OGLFence>();
        }
    };
}

#endif //XENGINE_OGLTEXTUREBUFFER_HPP
