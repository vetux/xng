/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/rendergraph/rendergraphtexture.hpp"

struct OGLTexture {
    GLuint handle{};
    GLenum textureType{};
    GLint textureInternalFormat{};

    RenderGraphTexture texture;

    OGLTexture() = default;

    explicit OGLTexture(const RenderGraphTexture &texture)
        : texture(texture) {
        if (texture.isArrayTexture) {
            initializeArrayTexture(texture);
        } else {
            initializeTexture(texture);
        }
    }

    ~OGLTexture() {
        glDeleteTextures(1, &handle);
    }

private:
    void initializeTexture(const RenderGraphTexture &texture) {
        textureType = convert(texture.textureType);

        oglDebugStartGroup("Texture Buffer Constructor");

        glGenTextures(1, &handle);
        glBindTexture(textureType, handle);

        if (texture.textureType == TEXTURE_2D) {
            textureInternalFormat = convert(texture.format);

            switch (texture.format) {
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
                           texture.mipMapLevels,
                           textureInternalFormat,
                           texture.size.x,
                           texture.size.y);

            try {
                oglCheckError();
            } catch (const std::runtime_error &e) {
                // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                    glTexStorage2D(textureType,
                                   texture.mipMapLevels,
                                   textureInternalFormat,
                                   texture.size.x,
                                   texture.size.y);
                } else {
                    throw e;
                }
            }
        } else if (texture.textureType == TEXTURE_2D_MULTISAMPLE) {
            textureInternalFormat = convert(texture.format);

            switch (texture.format) {
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
                                      texture.mipMapLevels,
                                      textureInternalFormat,
                                      texture.size.x,
                                      texture.size.y,
                                      texture.fixedSampleLocations ? GL_TRUE : GL_FALSE);

            try {
                oglCheckError();
            } catch (const std::runtime_error &e) {
                // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point
                // and create only one layer when this happens.
                if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                    glTexStorage2DMultisample(textureType,
                                              texture.mipMapLevels,
                                              textureInternalFormat,
                                              texture.size.x,
                                              texture.size.y,
                                              texture.fixedSampleLocations ? GL_TRUE : GL_FALSE);
                } else {
                    throw e;
                }
            }
        } else {
            for (unsigned int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             static_cast<GLint>(convert(texture.format)),
                             texture.size.x,
                             texture.size.y,
                             0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             nullptr);
            }
        }

        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(texture.wrapping));
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(texture.wrapping));

        if (texture.mipMapLevels > 1) {
            glTexParameteri(textureType,
                            GL_TEXTURE_MIN_FILTER,
                            convert(texture.mipMapFilter));
        } else {
            glTexParameteri(textureType,
                            GL_TEXTURE_MIN_FILTER,
                            convert(texture.filterMin));
        }

        glTexParameteri(textureType,
                        GL_TEXTURE_MAG_FILTER,
                        convert(texture.filterMag));

        auto col = texture.borderColor.divide();
        float borderColor[] = {col.x, col.y, col.z, col.w};
        glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindTexture(textureType, 0);

        oglDebugEndGroup();

        oglCheckError();
    }

    void initializeArrayTexture(const RenderGraphTexture &texture) {
        if (texture.textureType != TEXTURE_2D && texture.textureType != TEXTURE_CUBE_MAP) {
            throw std::runtime_error("Invalid texture type for array texture");
        }

        oglDebugStartGroup("Texture Array Buffer Constructor");

        glGenTextures(1, &handle);

        GLsizei layers;
        if (texture.textureType == TEXTURE_CUBE_MAP) {
            textureType = GL_TEXTURE_CUBE_MAP_ARRAY;
            layers = static_cast<GLsizei>(texture.arrayLayers) * 6;
        } else {
            textureType = GL_TEXTURE_2D_ARRAY;
            layers = static_cast<GLsizei>(texture.arrayLayers);
        }

        glBindTexture(textureType, handle);

        if (texture.arrayLayers > 0) {
            textureInternalFormat = convert(texture.format);

            switch (texture.format) {
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
                           texture.mipMapLevels,
                           textureInternalFormat,
                           texture.size.x,
                           texture.size.y,
                           layers);

            try {
                oglCheckError();
            } catch (const std::runtime_error &e) {
                // Catch GL_INVALID_OPERATION because the mipmap layers count maximum depends on log2 which uses floating point.
                if (e.what() == getGLErrorString(GL_INVALID_OPERATION)) {
                    glTexStorage3D(textureType,
                                   1,
                                   textureInternalFormat,
                                   texture.size.x,
                                   texture.size.y,
                                   layers);
                } else {
                    throw e;
                }
            }

            auto texWrap = convert(texture.wrapping);

            glTexParameteri(textureType,
                            GL_TEXTURE_WRAP_S,
                            texWrap);
            glTexParameteri(textureType,
                            GL_TEXTURE_WRAP_T,
                            texWrap);

            if (texture.mipMapLevels > 1) {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(texture.mipMapFilter));
            } else {
                glTexParameteri(textureType,
                                GL_TEXTURE_MIN_FILTER,
                                convert(texture.filterMin));
            }

            glTexParameteri(textureType,
                            GL_TEXTURE_MAG_FILTER,
                            convert(texture.filterMag));

            auto col = texture.borderColor.divide();
            float borderColor[] = {col.x, col.y, col.z, col.w};
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glBindTexture(textureType, 0);

        oglDebugEndGroup();

        oglCheckError();
    }
};

#endif //XENGINE_TEXTUREOGL_HPP
