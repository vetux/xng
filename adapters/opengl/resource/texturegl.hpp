/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_TEXTUREOGL_HPP
#define XENGINE_TEXTUREOGL_HPP

#include "glad/glad.h"

#include "typeconversion.hpp"
#include "ogldebug.hpp"

#include "xng/rendergraph/resource/texture.hpp"

namespace xng::opengl {
    struct TextureGL {
        GLuint handle{};
        GLenum textureType{};
        GLint textureInternalFormat{};

        rendergraph::Texture desc;

        TextureGL() = default;

        explicit TextureGL(const rendergraph::Texture &texture)
            : desc(texture) {
            if (texture.textureType >= TEXTURE_2D_ARRAY) {
                initializeArrayTexture();
            } else {
                initializeTexture();
            }
        }

        ~TextureGL() {
            glDeleteTextures(1, &handle);
            oglCheckError();
        }

    private:
        void initializeTexture() {
            textureType = convert(desc.textureType);

            oglDebugStartGroup("Texture Buffer Constructor");

            glGenTextures(1, &handle);
            glBindTexture(textureType, handle);

            if (desc.textureType == TEXTURE_2D) {
                textureInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        textureInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        textureInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        textureInternalFormat = GL_R8;
                        break;
                    case RG:
                        textureInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        textureInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        textureInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage2D(textureType,
                               desc.mipLevels,
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y);

                try {
                    oglCheckError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage2D(textureType,
                                       desc.mipLevels,
                                       textureInternalFormat,
                                       desc.size.x,
                                       desc.size.y);
                    } else {
                        throw e;
                    }
                }
            } else if (desc.textureType == TEXTURE_2D_MULTISAMPLE) {
                textureInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        textureInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        textureInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        textureInternalFormat = GL_R8;
                        break;
                    case RG:
                        textureInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        textureInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        textureInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage2DMultisample(textureType,
                                          desc.mipLevels,
                                          textureInternalFormat,
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
                                                  desc.mipLevels,
                                                  textureInternalFormat,
                                                  desc.size.x,
                                                  desc.size.y,
                                                  desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);
                    } else {
                        throw e;
                    }
                }
            } else {
                // Cube map textures: allocate immutable storage for all faces and mip levels
                textureInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        textureInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        textureInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        textureInternalFormat = GL_R8;
                        break;
                    case RG:
                        textureInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        textureInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        textureInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage2D(textureType,
                               desc.mipLevels,
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y);
            }

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));
            // For cube maps also set R wrap to avoid sampling artifacts across faces
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, convert(desc.wrapping));

            if (desc.mipLevels > 1) {
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

        void initializeArrayTexture() {
            oglDebugStartGroup("Texture Buffer Constructor");

            glGenTextures(1, &handle);

            textureType = convert(desc.textureType);

            GLsizei layers;
            if (desc.textureType == TEXTURE_CUBE_MAP_ARRAY) {
                layers = static_cast<GLsizei>(desc.arrayLayers) * 6;
            } else {
                layers = static_cast<GLsizei>(desc.arrayLayers);
            }

            glBindTexture(textureType, handle);

            if (desc.arrayLayers > 0) {
                textureInternalFormat = convert(desc.format);

                switch (desc.format) {
                    case DEPTH:
                        textureInternalFormat = GL_DEPTH_COMPONENT32F;
                        break;
                    case DEPTH_STENCIL:
                        textureInternalFormat = GL_DEPTH24_STENCIL8;
                        break;
                    case R:
                        textureInternalFormat = GL_R8;
                        break;
                    case RG:
                        textureInternalFormat = GL_RG8;
                        break;
                    case RGB:
                        textureInternalFormat = GL_RGB8;
                        break;
                    case RGBA:
                        textureInternalFormat = GL_RGBA8;
                        break;
                    default:
                        break;
                }

                glTexStorage3D(textureType,
                               desc.mipLevels,
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y,
                               layers);

                try {
                    oglCheckError();
                } catch (const std::runtime_error &e) {
                    // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                    if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                        glTexStorage3D(textureType,
                                       1,
                                       textureInternalFormat,
                                       desc.size.x,
                                       desc.size.y,
                                       layers);
                    } else {
                        throw e;
                    }
                }

                auto texWrap = convert(desc.wrapping);

                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_S,
                                texWrap);
                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_T,
                                texWrap);
                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_R,
                                texWrap);

                if (desc.mipLevels > 1) {
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
            }

            glBindTexture(textureType, 0);

            oglDebugEndGroup();

            oglCheckError();
        }
    };
}

#endif //XENGINE_TEXTUREOGL_HPP
