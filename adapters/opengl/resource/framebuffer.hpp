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

#ifndef XENGINE_OGLFRAMEBUFFER_HPP
#define XENGINE_OGLFRAMEBUFFER_HPP

#include "ogldebug.hpp"

#include "resource/texturegl.hpp"

#include "surfacegl.hpp"

#include "xng/rendergraph/attachment.hpp"

namespace xng::opengl {
    struct Framebuffer {
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

        void bind(const GLenum target) {
            bindingPoint = target;
            glBindFramebuffer(bindingPoint, FBO);
            oglCheckError();
        }

        void unbind() {
            glBindFramebuffer(bindingPoint, 0);
            oglCheckError();
            bindingPoint = 0;
        }

        void attach(const GLenum attachmentPoint, const TextureGL &tex, const Texture::SubResource &target) const {
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

        void attach2D(const GLenum attachment,
                      const TextureGL &texture,
                      const GLenum target,
                      const GLint mipLevel) const {
            if (bindingPoint == 0) {
                throw std::runtime_error("Framebuffer must be bound before attaching a texture");
            }
            glFramebufferTexture2D(bindingPoint,
                                   attachment,
                                   target,
                                   texture.handle,
                                   mipLevel);
            oglCheckError();
        }

        void attachLayered(const GLenum attachment, const TextureGL &texture, const GLint mipLevel) const {
            if (texture.textureType != GL_TEXTURE_CUBE_MAP
                && texture.textureType != GL_TEXTURE_CUBE_MAP_ARRAY
                && texture.textureType != GL_TEXTURE_2D_ARRAY
                && texture.textureType != GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
                throw std::runtime_error("Invalid texture type for layered attachment");
            }
            if (bindingPoint == 0) {
                throw std::runtime_error("Framebuffer must be bound before attaching a texture");
            }
            glFramebufferTexture(bindingPoint,
                                 attachment,
                                 texture.handle,
                                 mipLevel);
            oglCheckError();
        }

        void attachArrayLayer(const GLenum attachment,
                              const TextureGL &texture,
                              const GLint mipLevel,
                              const GLint index) const {
            if (texture.textureType != GL_TEXTURE_2D_ARRAY) {
                throw std::runtime_error("Invalid texture type for index 2D array attachment");
            }
            if (bindingPoint == 0) {
                throw std::runtime_error("Framebuffer must be bound before attaching a texture");
            }
            glFramebufferTextureLayer(bindingPoint,
                                      attachment,
                                      texture.handle,
                                      mipLevel,
                                      index);
            oglCheckError();
        }

        void attachArrayLayer(const GLenum attachment,
                              const TextureGL &texture,
                              const GLint mipLevel,
                              const GLint index,
                              const GLenum face) const {
            if (texture.textureType != GL_TEXTURE_CUBE_MAP_ARRAY) {
                throw std::runtime_error("Invalid texture type for index cubemap array attachment");
            }
            if (bindingPoint == 0) {
                throw std::runtime_error("Framebuffer must be bound before attaching a texture");
            }
            glFramebufferTexture3D(bindingPoint,
                                   attachment,
                                   face,
                                   texture.handle,
                                   mipLevel,
                                   index);
            oglCheckError();
        }

    private:
        GLuint FBO = 0;
        GLenum bindingPoint = 0;
    };
}

#endif //XENGINE_OGLFRAMEBUFFER_HPP
