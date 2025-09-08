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

#ifndef XENGINE_OGLTEXTUREARRAYBUFFER_HPP
#define XENGINE_OGLTEXTUREARRAYBUFFER_HPP

#include "xng/gpu/texturearraybuffer.hpp"

#include <utility>

#include "oglinclude.hpp"
#include "ogldebug.hpp"

#include "xng/render/scene/color.hpp"

namespace xng::opengl {
    class OGLTextureArrayBuffer : public TextureArrayBuffer {
    public:
        TextureArrayBufferDesc desc;

        GLuint handle{};
        GLint internalFormat{};

        RenderStatistics &stats;

        OGLTextureArrayBuffer(TextureArrayBufferDesc descArg,
                              RenderStatistics &stats)
                : desc(std::move(descArg)),
                  stats(stats) {
            if (desc.textureDesc.textureType != TEXTURE_2D && desc.textureDesc.textureType != TEXTURE_CUBE_MAP) {
                throw std::runtime_error("Invalid texture type for array texture");
            }

            oglDebugStartGroup("Texture Array Buffer Constructor");

            glGenTextures(1, &handle);

            GLenum target;
            GLsizei layers;
            if (desc.textureDesc.textureType == TEXTURE_CUBE_MAP) {
                target = GL_TEXTURE_CUBE_MAP_ARRAY;
                layers = static_cast<GLsizei>(desc.textureCount) * 6;
            } else {
                target = GL_TEXTURE_2D_ARRAY;
                layers = static_cast<GLsizei>(desc.textureCount);
            }

            glBindTexture(target, handle);

            if (desc.textureCount > 0) {
                internalFormat = convert(desc.textureDesc.format);

                switch (desc.textureDesc.format) {
                    case DEPTH:
                        internalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        internalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        internalFormat = GL_R8;
                        break;
                    case RG:
                        internalFormat = GL_RG8;
                        break;
                    case RGB:
                        internalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        internalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage3D(target,
                               desc.textureDesc.mipMapLevels,
                               internalFormat,
                               desc.textureDesc.size.x,
                               desc.textureDesc.size.y,
                               layers);

                try {
                    oglCheckError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage3D(target,
                                       1,
                                       internalFormat,
                                       desc.textureDesc.size.x,
                                       desc.textureDesc.size.y,
                                       layers);
                    } else {
                        throw e;
                    }
                }

                auto texWrap = convert(desc.textureDesc.wrapping);

                glTexParameteri(target,
                                GL_TEXTURE_WRAP_S,
                                texWrap);
                glTexParameteri(target,
                                GL_TEXTURE_WRAP_T,
                                texWrap);

                if (desc.textureDesc.mipMapLevels > 1) {
                    glTexParameteri(target,
                                    GL_TEXTURE_MIN_FILTER,
                                    convert(desc.textureDesc.mipMapFilter));
                } else {
                    glTexParameteri(target,
                                    GL_TEXTURE_MIN_FILTER,
                                    convert(desc.textureDesc.filterMin));
                }

                glTexParameteri(target,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.textureDesc.filterMag));

                auto col = desc.textureDesc.borderColor.divide();
                float borderColor[] = {col.x, col.y, col.z, col.w};
                glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
            }

            glBindTexture(target, 0);

            oglDebugEndGroup();

            oglCheckError();
        }

        ~OGLTextureArrayBuffer() override {
            glDeleteTextures(1, &handle);
            oglCheckError();
        }

        const TextureArrayBufferDesc &getDescription() override {
            return desc;
        }

        void upload(size_t index,
                    ColorFormat format,
                    const uint8_t *buffer,
                    size_t bufferSize,
                    int mipMapLevel) override {
            auto target = desc.textureDesc.textureType == TEXTURE_CUBE_MAP
                          ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_2D_ARRAY;

            oglDebugStartGroup("Texture Array Buffer Upload");

            glBindTexture(target, handle);

            glTexSubImage3D(target,
                            mipMapLevel,
                            0,
                            0,
                            static_cast<GLint>(index),
                            desc.textureDesc.size.x,
                            desc.textureDesc.size.y,
                            1,
                            convert(format),
                            GL_UNSIGNED_BYTE,
                            buffer);

            glBindTexture(target, 0);

            oglDebugEndGroup();

            oglCheckError();

            stats.uploadTexture += bufferSize;
        }

        Image<ColorRGBA> download(size_t index) override {
            // Requires https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetTextureSubImage.xhtml
            throw std::runtime_error("Download not supported for texture array buffers");
        }

        void generateMipMaps() override {
            auto target = desc.textureDesc.textureType == TEXTURE_CUBE_MAP
                          ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_2D_ARRAY;

            oglDebugStartGroup("Texture Array Buffer Generate Mip Maps");

            glBindTexture(target, handle);

            glGenerateMipmap(target);

            glBindTexture(target, 0);

            oglDebugEndGroup();

            oglCheckError();
        }
    };
}

#endif //XENGINE_OGLTEXTUREARRAYBUFFER_HPP
