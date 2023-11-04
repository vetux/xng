/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_OGLRENDERTARGET_HPP
#define XENGINE_OGLRENDERTARGET_HPP

#include "xng/gpu/rendertarget.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"
#include "gpu/opengl/oglfence.hpp"
#include "gpu/opengl/ogltexturearraybuffer.hpp"

namespace xng::opengl {
    class OGLRenderTarget : public RenderTarget {
    public:
        std::function<void(RenderObject *)> destructor;
        RenderTargetDesc desc;

        GLuint FBO = 0;

        int attachedColor = 0;
        bool attachedDepthStencil = false;

        OGLRenderTarget() = default;

        explicit OGLRenderTarget(std::function<void(RenderObject *)> destructor, RenderTargetDesc inputDescription)
                : destructor(std::move(destructor)), desc(std::move(inputDescription)) {
            glGenFramebuffers(1, &FBO);

            if (desc.numberOfColorAttachments > 0) {
                std::vector<unsigned int> attachments(desc.numberOfColorAttachments);
                for (int i = 0; i < desc.numberOfColorAttachments; i++) {
                    attachments[i] = GL_COLOR_ATTACHMENT0 + i;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                glDrawBuffers(desc.numberOfColorAttachments, attachments.data());
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            checkGLError();
        }

        ~OGLRenderTarget() override {
            //Check if FBO is 0 which is the default framebuffer managed by the display manager.
            if (FBO != 0) {
                glDeleteFramebuffers(1, &FBO);
                checkGLError();
            }
            if (destructor) {
                destructor(this);
            }
        }

        const RenderTargetDesc &getDescription() override {
            return desc;
        }

        static size_t getLayerIndex(size_t layer, CubeMapFace face) {
            return (layer * 6) + face;
        }

        void attach(const RenderTargetAttachment &att, GLenum attachment){
            switch (att.type) {
                case RenderTargetAttachment::ATTACHMENT_TEXTURE: {
                    auto &tex = dynamic_cast<OGLTextureBuffer &>(*att.textureBuffer);
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           attachment,
                                           convert(tex.getDescription().textureType),
                                           tex.handle,
                                           static_cast<GLint>(att.mipMapLevel));
                    break;
                }
                case RenderTargetAttachment::ATTACHMENT_CUBEMAP: {
                    auto &tex = dynamic_cast<OGLTextureBuffer &>(*att.textureBuffer);
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           attachment,
                                           convert(att.face),
                                           tex.handle,
                                           static_cast<GLint>(att.mipMapLevel));
                    break;
                }
                case RenderTargetAttachment::ATTACHMENT_CUBEMAP_LAYERED: {
                    auto &tex = dynamic_cast<OGLTextureBuffer &>(*att.textureBuffer);
                    if (tex.textureType != TEXTURE_CUBE_MAP){
                        throw std::runtime_error("Invalid texture type for ATTACHMENT_CUBEMAP_LAYERED attachment");
                    }
                    glFramebufferTexture(GL_FRAMEBUFFER,
                                         attachment,
                                         tex.handle,
                                         static_cast<GLint>(att.mipMapLevel));
                    break;
                }
                case RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY: {
                    auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(*att.textureArrayBuffer);
                    glFramebufferTexture3D(GL_FRAMEBUFFER,
                                           attachment,
                                           convert(tex.getDescription().textureDesc.textureType),
                                           tex.handle,
                                           static_cast<GLint>(att.mipMapLevel),
                                           static_cast<GLint>(att.index));
                    break;
                }
                case RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_CUBEMAP: {
                    auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(*att.textureArrayBuffer);
                    glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                              attachment,
                                              tex.handle,
                                              static_cast<GLint>(att.mipMapLevel),
                                              static_cast<GLint>(getLayerIndex(att.index, att.face)));
                    break;
                }
                case RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_LAYERED: {
                    auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(*att.textureArrayBuffer);
                    glFramebufferTexture(GL_FRAMEBUFFER,
                                         attachment,
                                         tex.handle,
                                         static_cast<GLint>(att.mipMapLevel));
                    break;
                }
            }
        }

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments) override {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            if (colorAttachments.size() != desc.numberOfColorAttachments)
                throw std::runtime_error("Invalid number of color attachments");

            for (auto i = 0; i < colorAttachments.size(); i++) {
                attach(colorAttachments.at(i), GL_COLOR_ATTACHMENT0 + i);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            attachedColor = static_cast<int>(colorAttachments.size());

            checkGLError();
        }

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments,
                            RenderTargetAttachment depthStencilAttachment) override {
            if (colorAttachments.size() != desc.numberOfColorAttachments)
                throw std::runtime_error("Invalid number of color attachments");

            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            for (auto i = 0; i < colorAttachments.size(); i++) {
                attach(colorAttachments.at(i), GL_COLOR_ATTACHMENT0 + i);
            }

            attach(depthStencilAttachment, GL_DEPTH_STENCIL_ATTACHMENT);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            attachedColor = static_cast<int>(colorAttachments.size());
            attachedDepthStencil = true;

            checkGLError();
        }

        void clearAttachments() override {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            for (int i = 0; i < attachedColor; i++) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
            }
            if (attachedDepthStencil) {
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_DEPTH_STENCIL_ATTACHMENT,
                                       GL_TEXTURE_2D,
                                       0,
                                       0);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    };
}

#endif //XENGINE_OGLRENDERTARGET_HPP
