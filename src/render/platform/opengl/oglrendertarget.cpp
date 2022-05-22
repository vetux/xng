/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef BUILD_ENGINE_RENDERER_OPENGL

#include "render/platform/opengl/oglrendertarget.hpp"
#include "render/platform/opengl/ogltypeconverter.hpp"
#include "render/platform/opengl/oglcheckerror.hpp"
#include "render/platform/opengl/ogltexturebuffer.hpp"

namespace xengine {
    opengl::OGLRenderTarget::OGLRenderTarget() : FBO(0), size(), samples() {}

    opengl::OGLRenderTarget::OGLRenderTarget(Vec2i size)
            : FBO(0), size(size), samples() {
        glGenFramebuffers(1, &FBO);
        checkGLError("OGLRenderAllocator::allocateFrameBuffer");
    }

    opengl::OGLRenderTarget::OGLRenderTarget(Vec2i size, int samples)
            : FBO(), size(size), samples(samples) {
        glGenFramebuffers(1, &FBO);
        checkGLError("OGLRenderAllocator::allocateFrameBuffer");
    }

    opengl::OGLRenderTarget::~OGLRenderTarget() {
        //Check if FBO is 0 which is the default framebuffer managed by the display manager.
        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
            checkGLError();
        }
    }

    Vec2i opengl::OGLRenderTarget::getSize() {
        return size;
    }

    int opengl::OGLRenderTarget::getSamples() {
        return samples;
    }

    void opengl::OGLRenderTarget::blitColor(RenderTarget &source,
                                            Vec2i sourceOffset,
                                            Vec2i targetOffset,
                                            Vec2i sourceRect,
                                            Vec2i targetRect,
                                            TextureBuffer::TextureFiltering filter,
                                            int sourceIndex,
                                            int targetIndex) {
        if (sourceRect.x < 0 || sourceRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (sourceOffset.x < 0 || sourceOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }
        if (targetRect.x < 0 || targetRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (targetOffset.x < 0 || targetOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }

        auto &fbS = dynamic_cast<OGLRenderTarget &>(source);

        Vec2i sourceSize = fbS.getSize();
        if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
            throw std::runtime_error("Blit rect out of bounds for source framebuffer");

        Vec2i targetSize = getSize();
        if (targetSize.x < targetRect.x + targetOffset.x || targetSize.y < targetRect.y + targetOffset.y)
            throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());

        // The default framebuffer always reads/writes from/to default color buffer.
        if (fbS.getFBO() != 0) {
            glReadBuffer(OGLTypeConverter::getColorAttachment(sourceIndex));
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFBO());

        // The default framebuffer always reads/writes from/to default color buffer.
        if (getFBO() != 0) {
            glDrawBuffer(OGLTypeConverter::getColorAttachment(targetIndex));
        }

        glBlitFramebuffer(sourceOffset.x,
                          sourceOffset.y,
                          sourceRect.x,
                          sourceRect.y,
                          targetOffset.x,
                          targetOffset.y,
                          targetRect.x,
                          targetRect.y,
                          GL_COLOR_BUFFER_BIT,
                          OGLTypeConverter::convert(filter));

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        checkGLError("OGLUserFrameBuffer::blitFramebuffer");
    }

    void opengl::OGLRenderTarget::blitDepth(RenderTarget &source,
                                            Vec2i sourceOffset,
                                            Vec2i targetOffset,
                                            Vec2i sourceRect,
                                            Vec2i targetRect) {
        if (sourceRect.x < 0 || sourceRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (sourceOffset.x < 0 || sourceOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }
        if (targetRect.x < 0 || targetRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (targetOffset.x < 0 || targetOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }

        auto &fbS = dynamic_cast< OGLRenderTarget &>(source);

        Vec2i sourceSize = fbS.getSize();
        if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
            throw std::runtime_error("Blit rect out of bounds for source framebuffer");

        Vec2i targetSize = getSize();
        if (targetSize.x < targetRect.x + targetOffset.x || targetSize.y < targetRect.y + targetOffset.y)
            throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFBO());

        glBlitFramebuffer(sourceOffset.x,
                          sourceOffset.y,
                          sourceRect.x,
                          sourceRect.y,
                          targetOffset.x,
                          targetOffset.y,
                          targetRect.x,
                          targetRect.y,
                          GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        checkGLError("OGLUserFrameBuffer::blitFramebuffer");
    }

    void opengl::OGLRenderTarget::blitStencil(RenderTarget &source,
                                              Vec2i sourceOffset,
                                              Vec2i targetOffset,
                                              Vec2i sourceRect,
                                              Vec2i targetRect) {
        if (sourceRect.x < 0 || sourceRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (sourceOffset.x < 0 || sourceOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }
        if (targetRect.x < 0 || targetRect.y < 0) {
            throw std::runtime_error("Rect cannot be negative");
        }
        if (targetOffset.x < 0 || targetOffset.y < 0) {
            throw std::runtime_error("Offset cannot be negative");
        }

        auto &fbS = dynamic_cast< OGLRenderTarget &>(source);

        Vec2i sourceSize = fbS.getSize();
        if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
            throw std::runtime_error("Blit rect out of bounds for source framebuffer");

        Vec2i targetSize = getSize();
        if (targetSize.x < targetRect.x + targetOffset.x || targetSize.y < targetRect.y + targetOffset.y)
            throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFBO());
        glBlitFramebuffer(sourceOffset.x,
                          sourceOffset.y,
                          sourceRect.x,
                          sourceRect.y,
                          targetOffset.x,
                          targetOffset.y,
                          targetRect.x,
                          targetRect.y,
                          GL_STENCIL_BUFFER_BIT,
                          GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        checkGLError("OGLUserFrameBuffer::blitFramebuffer");
    }

    void opengl::OGLRenderTarget::setNumberOfColorAttachments(int count) {
        for (int i = 0; i < colorAttachments; i++) {
            detachColor(i);
        }
        unsigned int attachments[count];
        for (int i = 0; i < count; i++) {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glDrawBuffers(count, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
        colorAttachments = count;
    }

    void opengl::OGLRenderTarget::attachColor(int index, TextureBuffer &texture) {
        detachColor(index);
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + index,
                               OGLTypeConverter::convert(texture.getAttributes().textureType),
                               tex.handle,
                               0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachDepth(TextureBuffer &texture) {
        detachDepth();
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               OGLTypeConverter::convert(texture.getAttributes().textureType), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachStencil(TextureBuffer &texture) {
        detachStencil();
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                               OGLTypeConverter::convert(texture.getAttributes().textureType), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachDepthStencil(TextureBuffer &texture) {
        detachDepthStencil();
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                               OGLTypeConverter::convert(texture.getAttributes().textureType), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachColor(int index, TextureBuffer::CubeMapFace face, TextureBuffer &texture) {
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, OGLTypeConverter::convert(face), 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, OGLTypeConverter::convert(face), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachDepth(TextureBuffer::CubeMapFace face, TextureBuffer &texture) {
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OGLTypeConverter::convert(face), 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OGLTypeConverter::convert(face), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachStencil(TextureBuffer::CubeMapFace face, TextureBuffer &texture) {
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, OGLTypeConverter::convert(face), 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, OGLTypeConverter::convert(face), tex.handle, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::attachDepthStencil(TextureBuffer::CubeMapFace face,
                                                     TextureBuffer &texture) {
        auto &tex = dynamic_cast< OGLTextureBuffer &>(texture);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, OGLTypeConverter::convert(face), 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, OGLTypeConverter::convert(face), tex.handle,
                               0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    GLuint opengl::OGLRenderTarget::getFBO() {
        return FBO;
    }

    void opengl::OGLRenderTarget::detachColor(int index) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::detachDepth() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::detachStencil() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    void opengl::OGLRenderTarget::detachDepthStencil() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
    }

    bool opengl::OGLRenderTarget::isComplete() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLError();
        return ret == GL_FRAMEBUFFER_COMPLETE;
    }
}

#endif