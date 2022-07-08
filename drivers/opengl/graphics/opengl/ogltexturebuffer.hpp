/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "graphics/texturebuffer.hpp"

#include "graphics/opengl/oglbuildmacro.hpp"

namespace xng {
    namespace opengl {
        class OPENGL_TYPENAME(TextureBuffer) : public TextureBuffer OPENGL_INHERIT {
        public:
            TextureBufferDesc desc;
            GLuint handle;

            OPENGL_TYPENAME(TextureBuffer)(const TextureBufferDesc &inputDescription)
                    : desc(std::move(inputDescription)) {
                initialize();

                GLenum type = convert(desc.textureType);

                glGenTextures(1, &handle);
                glBindTexture(type, handle);

                if (type != GL_TEXTURE_2D_MULTISAMPLE) {
                    glTexParameteri(type, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
                    glTexParameteri(type, GL_TEXTURE_WRAP_T, convert(desc.wrapping));
                    glTexParameteri(type,
                                    GL_TEXTURE_MIN_FILTER,
                                    convert(desc.filterMin));
                    glTexParameteri(type,
                                    GL_TEXTURE_MAG_FILTER,
                                    convert(desc.filterMag));
                }
                checkGLError("OGLTextureBuffer::OGLTextureBuffer()");

                if (desc.textureType == TEXTURE_2D) {
                    GLuint texInternalFormat = convert(desc.format);
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

                    glTexImage2D(type,
                                 0,
                                 numeric_cast<GLint>(texInternalFormat),
                                 desc.size.x,
                                 desc.size.y,
                                 0,
                                 texFormat,
                                 texType,
                                 NULL);
                } else if (desc.textureType == TEXTURE_2D_MULTISAMPLE) {
                    GLuint texInternalFormat = convert(desc.format);

                    if (desc.format == ColorFormat::DEPTH) {
                        texInternalFormat = GL_DEPTH;
                    } else if (desc.format == ColorFormat::DEPTH_STENCIL) {
                        texInternalFormat = GL_DEPTH24_STENCIL8;
                    }

                    glTexImage2DMultisample(type,
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
                                     NULL);
                    }
                }
                checkGLError("OGLTextureBuffer::OGLTextureBuffer()");

                if (type != GL_TEXTURE_2D_MULTISAMPLE && desc.generateMipmap) {
                    glGenerateMipmap(type);
                    glTexParameteri(type, GL_TEXTURE_MIN_FILTER,
                                    convert(desc.mipmapFilter));
                    glTexParameteri(type, GL_TEXTURE_MAG_FILTER,
                                    convert(desc.filterMag));
                }

                glBindTexture(type, 0);

                checkGLError("OGLTextureBuffer::OGLTextureBuffer()");
            }

            ~OPENGL_TYPENAME(TextureBuffer)() override {
                glDeleteTextures(1, &handle);
            }

            void pinGpuMemory() override {}

            void unpinGpuMemory() override {}

            const TextureBufferDesc &getDescription() override {
                return desc;
            }

            void upload(ColorFormat format, const uint8_t *buffer, size_t bufferSize) override {
                //TODO: Range check the buffer
                glBindTexture(GL_TEXTURE_2D, handle);
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             numeric_cast<GLint>(convert(desc.format)),
                             desc.size.x,
                             desc.size.y,
                             0,
                             convert(format),
                             GL_UNSIGNED_BYTE,
                             buffer);

                if (desc.generateMipmap) {
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

                glBindTexture(GL_TEXTURE_2D, 0);

                checkGLError("OGLTextureBuffer::upload(RGB)");
            }

            void upload(CubeMapFace face, ColorFormat format, const uint8_t *buffer, size_t bufferSize) override {
                //TODO: Range check the buffer
                glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
                glTexImage2D(convert(face),
                             0,
                             numeric_cast<GLint>(convert(desc.format)),
                             desc.size.x,
                             desc.size.y,
                             0,
                             convert(format),
                             GL_UNSIGNED_BYTE,
                             buffer);

                if (desc.generateMipmap) {
                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                }

                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                checkGLError("OGLTextureBuffer::upload(CUBEMAP)");
            }

            Image<ColorRGBA> download() override {
                if (desc.textureType != TEXTURE_2D)
                    throw std::runtime_error("TextureBuffer not texture 2d");

                auto output = ImageRGBA(desc.size);
                glBindTexture(GL_TEXTURE_2D, handle);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) output.getData());
                glBindTexture(GL_TEXTURE_2D, 0);
                checkGLError("OGLTextureBuffer::download");
                return output;
            }

            Image<ColorRGBA> download(CubeMapFace face) override {
                if (desc.textureType != TEXTURE_CUBE_MAP)
                    throw std::runtime_error("TextureBuffer not cubemap");

                throw std::runtime_error("Not Implemented");
            }

            OPENGL_MEMBERS

            OPENGL_CONVERSION_MEMBERS
        };
    }
}

#endif //XENGINE_OGLTEXTUREBUFFER_HPP
