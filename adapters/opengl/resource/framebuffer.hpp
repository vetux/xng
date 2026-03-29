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

#ifndef XENGINE_OGLFRAMEBUFFER_HPP
#define XENGINE_OGLFRAMEBUFFER_HPP

#include "ogldebug.hpp"

#include "resource/texturegl.hpp"

#include "surfacegl.hpp"

#include "xng/rendergraph/attachment.hpp"

namespace xng::opengl {
    struct Framebuffer {
        GLuint FBO = 0;

        Framebuffer() {
            glGenFramebuffers(1, &FBO);
            oglCheckError();
        }

        ~Framebuffer() {
            if (FBO != 0) {
                glDeleteFramebuffers(1, &FBO);
                oglCheckError();
            }
        }

        static void attach(const GLenum attachmentPoint, const TextureGL &tex, const Texture::SubResource &target) {
            switch (tex.desc.textureType) {
                case TEXTURE_2D:
                case TEXTURE_2D_MULTISAMPLE:
                    attach2D(attachmentPoint, tex, tex.textureType, static_cast<GLint>(target.mipLevel));
                    break;
                case TEXTURE_CUBE_MAP:
                    if (target.face == FACE_UNDEFINED) {
                        attachLayered(attachmentPoint, tex, static_cast<GLint>(target.mipLevel));
                    } else {
                        attach2D(attachmentPoint, tex, convert(target.face), static_cast<GLint>(target.mipLevel));
                    }
                    break;
                case TEXTURE_2D_ARRAY:
                case TEXTURE_2D_MULTISAMPLE_ARRAY:
                    if (target.arrayLayer < 0) {
                        attachLayered(attachmentPoint, tex, static_cast<GLint>(target.mipLevel));
                    } else {
                        attachArrayLayer(attachmentPoint,
                                         tex,
                                         static_cast<GLint>(target.mipLevel),
                                         static_cast<GLint>(target.arrayLayer));
                    }
                    break;
                case TEXTURE_CUBE_MAP_ARRAY:
                    if (target.arrayLayer < 0) {
                        if (target.face != FACE_UNDEFINED) {
                            throw std::runtime_error(
                                "Target face must be undefined when array layer is not specified.");
                        }
                        attachLayered(attachmentPoint, tex, static_cast<GLint>(target.mipLevel));
                    } else {
                        if (target.face == FACE_UNDEFINED) {
                            throw std::runtime_error("Target face must be specified when array layer is specified.");
                        }
                        attachArrayLayer(attachmentPoint,
                                         tex,
                                         static_cast<GLint>(target.mipLevel),
                                         static_cast<GLint>(target.arrayLayer),
                                         convert(target.face));
                    }
                    break;
                default:
                    throw std::runtime_error("Invalid texture type");
            }
        }

        static void attach2D(const GLenum attachment,
                             const TextureGL &texture,
                             const GLenum target,
                             const GLint mipLevel) {
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                                   attachment,
                                   target,
                                   texture.handle,
                                   mipLevel);
            oglCheckError();
        }

        static void attachLayered(const GLenum attachment, const TextureGL &texture, const GLint mipLevel) {
            if (texture.textureType != GL_TEXTURE_CUBE_MAP
                && texture.textureType != GL_TEXTURE_CUBE_MAP_ARRAY
                && texture.textureType != GL_TEXTURE_2D_ARRAY
                && texture.textureType != GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
                throw std::runtime_error("Invalid texture type for layered attachment");
            }
            glFramebufferTexture(GL_DRAW_FRAMEBUFFER,
                                 attachment,
                                 texture.handle,
                                 mipLevel);
            oglCheckError();
        }

        static void attachArrayLayer(const GLenum attachment,
                                     const TextureGL &texture,
                                     const GLint mipLevel,
                                     const GLint index) {
            if (texture.textureType != GL_TEXTURE_2D_ARRAY) {
                throw std::runtime_error("Invalid texture type for index 2D array attachment");
            }
            glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER,
                                      attachment,
                                      texture.handle,
                                      mipLevel,
                                      index);
            oglCheckError();
        }

        static void attachArrayLayer(const GLenum attachment,
                                     const TextureGL &texture,
                                     const GLint mipLevel,
                                     const GLint index,
                                     const GLenum face) {
            if (texture.textureType != GL_TEXTURE_CUBE_MAP_ARRAY) {
                throw std::runtime_error("Invalid texture type for index cubemap array attachment");
            }
            glFramebufferTexture3D(GL_DRAW_FRAMEBUFFER,
                                   attachment,
                                   face,
                                   texture.handle,
                                   mipLevel,
                                   index);
            oglCheckError();
        }
    };
}

#endif //XENGINE_OGLFRAMEBUFFER_HPP
