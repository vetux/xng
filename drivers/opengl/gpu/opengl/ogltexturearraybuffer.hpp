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

#ifndef XENGINE_OGLTEXTUREARRAYBUFFER_HPP
#define XENGINE_OGLTEXTUREARRAYBUFFER_HPP

#include "xng/gpu/texturearraybuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "opengl_checkerror.hpp"

#include "xng/asset/color.hpp"

namespace xng::opengl {
    class OGLTextureArrayBuffer : public TextureArrayBuffer {
    public:
        std::function<void(RenderObject * )> destructor;
        TextureArrayBufferDesc desc;

        GLuint handle{};

        OGLTextureArrayBuffer(std::function<void(RenderObject * )> destructor,
                              TextureArrayBufferDesc descArg)
                : destructor(std::move(destructor)),
                  desc(std::move(descArg)) {
            if (desc.textureDesc.textureType != TEXTURE_2D) {
                throw std::runtime_error("Invalid texture type for array texture");
            }

            glGenTextures(1, &handle);

            glBindTexture(GL_TEXTURE_2D_ARRAY, handle);

            if (desc.textureCount > 0) {
                GLenum format;
                switch (desc.textureDesc.format) {
                    case R:
                        format = GL_R8;
                        break;
                    case RG:
                        format = GL_RG8;
                        break;
                    case RGB:
                        format = GL_RGB8;
                        break;
                    case RGBA:
                        format = GL_RGBA8;
                        break;
                    default:
                        throw std::runtime_error("Unsupported format");
                }
                glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                               1,
                               format,
                               desc.textureDesc.size.x,
                               desc.textureDesc.size.y,
                               static_cast<GLsizei>(desc.textureCount));
                checkGLError();

                auto texWrap = convert(desc.textureDesc.wrapping);

                glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_WRAP_S,
                                texWrap);
                glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_WRAP_T,
                                texWrap);

                glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_MIN_FILTER,
                                convert(desc.textureDesc.filterMin));
                glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_MAG_FILTER,
                                convert(desc.textureDesc.filterMag));

                if (desc.textureDesc.generateMipmap) {
                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                    GL_TEXTURE_MIN_FILTER,
                                    convert(desc.textureDesc.mipmapFilter));
                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                    GL_TEXTURE_MAG_FILTER,
                                    convert(desc.textureDesc.filterMag));
                    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
                }

                auto col = desc.textureDesc.borderColor.divide();
                float borderColor[] = {col.x, col.y, col.z, col.w};
                glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

                checkGLError();

            }

            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

            checkGLError();
        }

        ~OGLTextureArrayBuffer() override {
            glDeleteTextures(1, &handle);
            checkGLError();
            destructor(this);
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &source) override {
            auto &buf = dynamic_cast<OGLTextureArrayBuffer &>(source);
            if (buf.desc.textureDesc != desc.textureDesc) {
                throw std::runtime_error("Cannot copy texture array buffer");
            }

            auto count = buf.desc.textureCount > desc.textureCount ? desc.textureCount : buf.desc.textureCount;

            if (count > 0)
                glCopyImageSubData(buf.handle,
                                   GL_TEXTURE_2D_ARRAY,
                                   0,
                                   0,
                                   0,
                                   0,
                                   handle,
                                   GL_TEXTURE_2D_ARRAY,
                                   0,
                                   0,
                                   0,
                                   0,
                                   buf.desc.textureDesc.size.x,
                                   buf.desc.textureDesc.size.y,
                                   static_cast<GLsizei>(count));
            checkGLError();

            return std::make_unique<OGLFence>();
        }

        size_t getSize() override {
            return 0;
        }

        const TextureArrayBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(size_t index,
                                         ColorFormat format,
                                         const uint8_t *buffer,
                                         size_t bufferSize) override {
            glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            0,
                            0,
                            0,
                            static_cast<GLint>(index),
                            desc.textureDesc.size.x,
                            desc.textureDesc.size.y,
                            1,
                            convert(format),
                            GL_UNSIGNED_BYTE,
                            buffer);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            checkGLError();
            return std::make_unique<OGLFence>();
        }

        Image <ColorRGBA> download(size_t index) override {
            // Requires https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetTextureSubImage.xhtml
            throw std::runtime_error("Download not supported for texture array buffers");
        }
    };
}

#endif //XENGINE_OGLTEXTUREARRAYBUFFER_HPP
