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

#ifndef XENGINE_OGLRENDERTARGET_HPP
#define XENGINE_OGLRENDERTARGET_HPP

#include "graphics/rendertarget.hpp"

#include "graphics/opengl/oglbuildmacro.hpp"

#include "graphics/opengl/ogltexturebuffer.hpp"

namespace xengine {
    namespace opengl {
        class OPENGL_TYPENAME(RenderTarget) : public RenderTarget OPENGL_INHERIT {
        public:
            RenderTargetDesc desc;

            GLuint FBO = 0;

            explicit OPENGL_TYPENAME(RenderTarget)(RenderTargetDesc desc)
                    : desc(desc) {
                initialize();

                glGenFramebuffers(1, &FBO);

                if (desc.numberOfColorAttachments < 1)
                    throw std::runtime_error("Invalid color attachment count");

                unsigned int attachments[desc.numberOfColorAttachments];
                for (int i = 0; i < desc.numberOfColorAttachments; i++) {
                    attachments[i] = GL_COLOR_ATTACHMENT0 + i;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                glDrawBuffers(desc.numberOfColorAttachments, attachments);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                checkGLError("OGLRenderAllocator::allocateFrameBuffer");
            }

            ~OPENGL_TYPENAME(RenderTarget)() override {
                //Check if FBO is 0 which is the default framebuffer managed by the display manager.
                if (FBO != 0) {
                    glDeleteFramebuffers(1, &FBO);
                    checkGLError();
                }
            }

            const RenderTargetDesc &getDescription() override {
                return desc;
            }

            void blitColor(RenderTarget &source,
                           Vec2i sourceOffset,
                           Vec2i targetOffset,
                           Vec2i sourceRect,
                           Vec2i targetRect,
                           TextureFiltering filter,
                           int sourceIndex,
                           int targetIndex) override {
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

                auto &fbS = dynamic_cast<OPENGL_TYPENAME(RenderTarget) &>(source);

                Vec2i sourceSize = fbS.getDescription().size;
                if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
                    throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                Vec2i targetSize = desc.size;
                if (targetSize.x < targetRect.x + targetOffset.x || targetSize.y < targetRect.y + targetOffset.y)
                    throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());

                // The default framebuffer always reads/writes from/to default color buffer.
                if (fbS.getFBO() != 0) {
                    glReadBuffer(getColorAttachment(sourceIndex));
                }

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFBO());

                // The default framebuffer always reads/writes from/to default color buffer.
                if (getFBO() != 0) {
                    glDrawBuffer(getColorAttachment(targetIndex));
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
                                  convert(filter));

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                checkGLError("OGLUserFrameBuffer::blitFramebuffer");
            }

            void blitDepth(RenderTarget &source,
                           Vec2i sourceOffset,
                           Vec2i targetOffset,
                           Vec2i sourceRect,
                           Vec2i targetRect) override {
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

                auto &fbS = dynamic_cast< OPENGL_TYPENAME(RenderTarget) &>(source);

                Vec2i sourceSize = fbS.getDescription().size;
                if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
                    throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                Vec2i targetSize = desc.size;
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

            void blitStencil(RenderTarget &source,
                             Vec2i sourceOffset,
                             Vec2i targetOffset,
                             Vec2i sourceRect,
                             Vec2i targetRect) override {
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

                auto &fbS = dynamic_cast< OPENGL_TYPENAME(RenderTarget) &>(source);

                Vec2i sourceSize = fbS.getDescription().size;
                if (sourceSize.x < sourceRect.x + sourceOffset.x || sourceSize.y < sourceRect.y + sourceOffset.y)
                    throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                Vec2i targetSize = desc.size;
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

            void setColorAttachments(const std::vector<TextureBuffer *> &textures) override {
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);

                if (textures.size() != desc.numberOfColorAttachments)
                    throw std::runtime_error("Invalid number of color attachments");

                int index = 0;
                for (auto &texture: textures) {
                    if (texture == nullptr) {
                        if (desc.multisample)
                            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + index,
                                                   GL_TEXTURE_2D_MULTISAMPLE,
                                                   0,
                                                   0);
                        else
                            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, 0, 0);
                    } else {
                        auto &tex = dynamic_cast<OPENGL_TYPENAME(TextureBuffer) &>(*texture);
                        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                               GL_COLOR_ATTACHMENT0 + index,
                                               convert(tex.getDescription().textureType),
                                               tex.handle,
                                               0);
                    }
                    index++;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                checkGLError();
            }

            void setDepthStencilAttachment(TextureBuffer *texture) override {
                if (texture == nullptr) {
                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    if (desc.multisample)
                        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                               GL_DEPTH_STENCIL_ATTACHMENT,
                                               GL_TEXTURE_2D_MULTISAMPLE,
                                               0,
                                               0);
                    else
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                } else {
                    auto &tex = dynamic_cast< OPENGL_TYPENAME(TextureBuffer) &>(*texture);
                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(texture->getDescription().textureType),
                                           tex.handle,
                                           0);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
                checkGLError();
            }

            void setColorAttachments(const std::vector<std::pair<CubeMapFace, TextureBuffer *>> &textures) override {
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);

                if (textures.size() != desc.numberOfColorAttachments)
                    throw std::runtime_error("Invalid number of color attachments");

                int index = 0;
                for (auto &pair: textures) {
                    if (pair.second == nullptr) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                               GL_COLOR_ATTACHMENT0 + index,
                                               convert(pair.first),
                                               0,
                                               0);
                    } else {
                        auto &tex = dynamic_cast<OPENGL_TYPENAME(TextureBuffer) &>(*pair.second);
                        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                               GL_COLOR_ATTACHMENT0 + index,
                                               convert(pair.first),
                                               tex.handle,
                                               0);
                    }
                    index++;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                checkGLError();
            }

            void setDepthStencilAttachment(CubeMapFace face, TextureBuffer *texture) override {
                if (texture == nullptr) {
                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0,
                                           0);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                } else {
                    auto &tex = dynamic_cast< OPENGL_TYPENAME(TextureBuffer) &>(*texture);
                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(face),
                                           0, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(face),
                                           tex.handle,
                                           0);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
                checkGLError();
            }

            bool isComplete() override {
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                checkGLError();
                return ret == GL_FRAMEBUFFER_COMPLETE;
            }

            virtual GLuint getFBO() {
                return FBO;
            }

            OPENGL_MEMBERS

            OPENGL_CONVERSION_MEMBERS
        };
    }
}

#endif //XENGINE_OGLRENDERTARGET_HPP
