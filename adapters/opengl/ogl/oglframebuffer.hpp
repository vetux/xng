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

#ifndef XENGINE_OGLFRAMEBUFFER_HPP
#define XENGINE_OGLFRAMEBUFFER_HPP

#include "ogldebug.hpp"
#include "glad/glad.h"

#include "ogltexture.hpp"

#include "xng/rendergraph/rendergraphattachment.hpp"

struct OGLFramebuffer {
    GLuint FBO = 0;

    OGLFramebuffer() {
        glGenFramebuffers(1, &FBO);
        oglCheckError();
    }

    ~OGLFramebuffer() {
        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
            oglCheckError();
        }
    }

    void attach(const RenderGraphAttachment &attachment, const OGLTexture &tex, GLenum attachmentPoint) const {
        switch (tex.textureType) {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_2D_MULTISAMPLE:
                attach(tex, attachmentPoint, static_cast<GLint>(attachment.mipMapLevel));
                break;
            case GL_TEXTURE_CUBE_MAP:
                attach(tex,
                       attachmentPoint,
                       static_cast<GLint>(attachment.mipMapLevel),
                       convert(attachment.face));
                break;
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_TEXTURE_2D_ARRAY:
                attachIndex(tex,
                            attachmentPoint,
                            static_cast<GLint>(attachment.mipMapLevel),
                            static_cast<GLint>(attachment.index));
                break;
            default:
                throw std::runtime_error("unsupported attachment type");
        }
    }

    void attach(const OGLTexture &texture, GLenum attachment, GLint mipMapLevel) const {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               texture.textureType,
                               texture.handle,
                               mipMapLevel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        oglCheckError();
    }

    void attach(const OGLTexture &texture, GLenum attachment, GLint mipLevel, GLenum face) const {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachment,
                               face,
                               texture.handle,
                               mipLevel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        oglCheckError();
    }

    void attachLayered(const OGLTexture &texture, GLenum attachment, GLint mipLevel) const {
        if (texture.textureType != GL_TEXTURE_CUBE_MAP) {
            throw std::runtime_error("Invalid texture type for layered cubemap attachment");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture(GL_FRAMEBUFFER,
                             attachment,
                             texture.handle,
                             mipLevel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        oglCheckError();
    }

    void attachIndex(const OGLTexture &texture, GLenum attachment, GLint mipLevel, GLint index) const {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture3D(GL_FRAMEBUFFER,
                               attachment,
                               convert(texture.texture.textureType),
                               texture.handle,
                               mipLevel,
                               index);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        oglCheckError();
    }
};

#endif //XENGINE_OGLFRAMEBUFFER_HPP
