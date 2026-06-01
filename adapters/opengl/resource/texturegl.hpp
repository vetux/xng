/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

        rg::Texture desc;

        TextureGL() = default;

        explicit TextureGL(const rg::Texture &texture)
            : desc(texture) {
            if (desc.format == RGB8
                || desc.format == RGB16
                || desc.format == SRGB8
                || desc.format == RGB16F
                || desc.format == RGB32F
                || desc.format == RGB8I
                || desc.format == RGB16I
                || desc.format == RGB32I
                || desc.format == RGB8UI
                || desc.format == RGB16UI
                || desc.format == RGB32UI) {
                throw std::runtime_error("Unsupported texture color format (Texture cannot be 3 component format)");
            }
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
        static GLsizei safeMipLevels(const unsigned int requested, const Vec2i &size) {
            const GLsizei maxLevels = static_cast<GLsizei>(std::floor(std::log2(std::max(size.x, size.y)))) + 1;
            return std::min(static_cast<GLsizei>(requested), maxLevels);
        }

        void initializeTexture() {
            textureType = convert(desc.textureType);

            oglDebugStartGroup("Texture Buffer Constructor");

            glGenTextures(1, &handle);
            glBindTexture(textureType, handle);

            if (desc.textureType == TEXTURE_2D) {
                textureInternalFormat = convert(desc.format);
                glTexStorage2D(textureType,
                               safeMipLevels(desc.mipLevels, desc.size),
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y);
                oglCheckError();
            } else if (desc.textureType == TEXTURE_2D_MULTISAMPLE) {
                textureInternalFormat = convert(desc.format);
                glTexStorage2DMultisample(textureType,
                                          safeMipLevels(desc.mipLevels, desc.size),
                                          textureInternalFormat,
                                          desc.size.x,
                                          desc.size.y,
                                          desc.fixedSampleLocations ? GL_TRUE : GL_FALSE);
                oglCheckError();
            } else {
                // Cube map textures: allocate immutable storage for all faces and mip levels
                textureInternalFormat = convert(desc.format);
                glTexStorage2D(textureType,
                               safeMipLevels(desc.mipLevels, desc.size),
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y);
            }

            glTexParameteri(textureType, GL_TEXTURE_WRAP_S, convert(desc.wrapping));
            glTexParameteri(textureType, GL_TEXTURE_WRAP_T, convert(desc.wrapping));

            // For cube maps also set R wrap to avoid sampling artifacts across faces
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, convert(desc.wrapping));

            GLint minFilter;
            if (desc.filterMin == NEAREST && desc.mipMode == NEAREST) {
                minFilter = GL_NEAREST_MIPMAP_NEAREST;
            } else if (desc.filterMin == LINEAR && desc.mipMode == NEAREST) {
                minFilter = GL_LINEAR_MIPMAP_NEAREST;
            } else if (desc.filterMin == NEAREST && desc.mipMode == LINEAR) {
                minFilter = GL_NEAREST_MIPMAP_LINEAR;
            } else {
                minFilter = GL_LINEAR_MIPMAP_LINEAR;
            }

            glTexParameteri(textureType,GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(textureType,GL_TEXTURE_MAG_FILTER, convert(desc.filterMag));

            switch (desc.borderColor.index()) {
                case 0: {
                    const auto &color = std::get<Vec4f>(desc.borderColor);
                    const GLfloat borderColor[] = {
                        color.x, color.y, color.z, color.w
                    };
                    glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                    break;
                }
                case 1: {
                    const auto &color = std::get<Vec4i>(desc.borderColor);
                    const GLint borderColor[] = {
                        color.x, color.y, color.z, color.w
                    };
                    glTexParameteriv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                    break;
                }
                case 2: {
                    const auto &color = std::get<Vec4u>(desc.borderColor);
                    const GLuint borderColor[] = {
                        color.x, color.y, color.z, color.w
                    };
                    glTexParameterIuiv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                    break;
                }
                default:
                    break;
            }

            glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY, desc.maxAnisotropy);

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
                glTexStorage3D(textureType,
                               safeMipLevels(desc.mipLevels, desc.size),
                               textureInternalFormat,
                               desc.size.x,
                               desc.size.y,
                               layers);
                oglCheckError();

                const auto texWrap = convert(desc.wrapping);

                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_S,
                                texWrap);
                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_T,
                                texWrap);
                glTexParameteri(textureType,
                                GL_TEXTURE_WRAP_R,
                                texWrap);

                GLint minFilter;
                if (desc.filterMin == NEAREST && desc.mipMode == NEAREST) {
                    minFilter = GL_NEAREST_MIPMAP_NEAREST;
                } else if (desc.filterMin == LINEAR && desc.mipMode == NEAREST) {
                    minFilter = GL_LINEAR_MIPMAP_NEAREST;
                } else if (desc.filterMin == NEAREST && desc.mipMode == LINEAR) {
                    minFilter = GL_NEAREST_MIPMAP_LINEAR;
                } else {
                    minFilter = GL_LINEAR_MIPMAP_LINEAR;
                }

                glTexParameteri(textureType,GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(textureType,GL_TEXTURE_MAG_FILTER, convert(desc.filterMag));

                switch (desc.borderColor.index()) {
                    case 0: {
                        const auto &color = std::get<Vec4f>(desc.borderColor);
                        const GLfloat borderColor[] = {
                            color.x, color.y, color.z, color.w
                        };
                        glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                        break;
                    }
                    case 1: {
                        const auto &color = std::get<Vec4i>(desc.borderColor);
                        const GLint borderColor[] = {
                            color.x, color.y, color.z, color.w
                        };
                        glTexParameteriv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                        break;
                    }
                    case 2: {
                        const auto &color = std::get<Vec4u>(desc.borderColor);
                        const GLuint borderColor[] = {
                            color.x, color.y, color.z, color.w
                        };
                        glTexParameterIuiv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);
                        break;
                    }
                    default:
                        break;
                }
            }

            glBindTexture(textureType, 0);

            oglDebugEndGroup();

            oglCheckError();
        }
    };
}

#endif //XENGINE_TEXTUREOGL_HPP
