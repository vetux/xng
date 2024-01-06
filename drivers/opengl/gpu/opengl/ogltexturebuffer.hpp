/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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
#include "xng/gpu/renderstatistics.hpp"

#include <utility>

#include "oglinclude.hpp"
#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLTextureBuffer : public TextureBuffer {
    public:
        TextureBufferDesc desc;
        GLuint handle = 0;

        GLenum textureType;
        GLint texInternalFormat;

        RenderStatistics &stats;

        OGLTextureBuffer(TextureBufferDesc descArg, RenderStatistics &stats)
                : desc(std::move(descArg)), stats(stats) {
            textureType = convert(desc.textureType);

            oglDebugStartGroup("Texture Buffer Constructor");

            glGenTextures(1, &handle);
            glBindTexture(textureType, handle);

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
                               desc.mipMapLevels,
                               texInternalFormat,
                               desc.size.x,
                               desc.size.y);

                try {
                    oglCheckError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage2D(textureType,
                                       desc.mipMapLevels,
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
                                          desc.mipMapLevels,
                                          texInternalFormat,
                                          desc.size.x,
                                          desc.size.y,
                                          desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);

                try {
                    oglCheckError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point
                    // and create only one layer when this happens.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage2DMultisample(textureType,
                                                  desc.mipMapLevels,
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

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            if (desc.mipMapLevels > 1) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.mipMapFilter));
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.filterMin));
            }

            glTexParameteri(textureType,
                            GL_TEXTURE_MAG_FILTER,
                            convert(desc.filterMag));

            auto col = desc.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindTexture(textureType, 0);

            oglDebugEndGroup();

            oglCheckError();
        }

        ~OGLTextureBuffer() override {
            glDeleteTextures(1, &handle);

        }

        const TextureBufferDesc &getDescription() override {
            return desc;
        }

        void upload(ColorFormat format, const uint8_t *buffer, size_t bufferSize, int mipMapLevel) override {
            if (desc.textureType == TEXTURE_CUBE_MAP) {
                throw std::runtime_error(
                        "Attempted to upload texture on cube map texture without specifying a target face.");
            }

            oglDebugStartGroup("Texture Buffer Upload");

            glBindTexture(GL_TEXTURE_2D, handle);

            glTexSubImage2D(GL_TEXTURE_2D,
                            mipMapLevel,
                            0,
                            0,
                            desc.size.x,
                            desc.size.y,
                            convert(format),
                            GL_UNSIGNED_BYTE,
                            buffer);

            glBindTexture(GL_TEXTURE_2D, 0);

            oglDebugEndGroup();

            oglCheckError();

            stats.uploadTexture += bufferSize;
        }

        void upload(CubeMapFace face,
                    ColorFormat format,
                    const uint8_t *buffer,
                    size_t bufferSize,
                    int mipMapLevel) override {
            if (desc.bufferType != HOST_VISIBLE) {
                throw std::runtime_error("Upload called on non host visible buffer.");
            }

            if (desc.textureType != TEXTURE_CUBE_MAP) {
                throw std::runtime_error("Attempted to upload a cube map face on a non cube map texture");
            }

            oglDebugStartGroup("Texture Buffer Upload");

            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);

            glTexSubImage2D(convert(face),
                            mipMapLevel,
                            0,
                            0,
                            desc.size.x,
                            desc.size.y,
                            convert(format),
                            GL_UNSIGNED_BYTE,
                            buffer);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            oglDebugEndGroup();

            oglCheckError();

            stats.uploadTexture += bufferSize;
        }

        Image<ColorRGBA> download() override {
            if (desc.textureType == TEXTURE_CUBE_MAP)
                throw std::runtime_error(
                        "Attempted to download a cube map texture without specifying the target face.");

            auto ret = ImageRGBA(desc.size);

            oglDebugStartGroup("Texture Buffer Download");

            glBindTexture(GL_TEXTURE_2D, handle);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) ret.getBuffer().data());
            glBindTexture(GL_TEXTURE_2D, 0);

            oglDebugEndGroup();

            oglCheckError();

            stats.downloadTexture += desc.size.x * desc.size.y * sizeof(ColorRGBA);

            return std::move(ret.swapColumns());
        }

        Image<ColorRGBA> download(CubeMapFace face) override {
            if (desc.textureType != TEXTURE_CUBE_MAP)
                throw std::runtime_error("Attempted to download face of a non cube map texture.");

            auto ret = ImageRGBA(desc.size);

            oglDebugStartGroup("Texture Buffer Download");

            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
            glGetTexImage(convert(face), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) ret.getBuffer().data());
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            oglDebugEndGroup();

            oglCheckError();

            stats.downloadTexture += desc.size.x * desc.size.y * sizeof(ColorRGBA);
            return std::move(ret.swapColumns());
        }

        void generateMipMaps() override {
            oglDebugStartGroup("Texture Buffer Generate Mip Maps");

            glBindTexture(textureType, handle);

            glGenerateMipmap(textureType);

            glBindTexture(textureType, 0);

            oglDebugEndGroup();

            oglCheckError();
        }
    };
}

#endif //XENGINE_OGLTEXTUREBUFFER_HPP
