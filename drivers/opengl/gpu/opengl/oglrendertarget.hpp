/**
 *  This file is part of xEngine, a C++ game engine library.
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

            if (desc.numberOfColorAttachments < 1)
                throw std::runtime_error("Invalid color attachment count");

            std::vector<unsigned int> attachments(desc.numberOfColorAttachments);
            for (int i = 0; i < desc.numberOfColorAttachments; i++) {
                attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glDrawBuffers(desc.numberOfColorAttachments, attachments.data());
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments) override {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            if (colorAttachments.empty()) {
                for (int i = 0; i < attachedColor; i++) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
                }
            } else {
                if (colorAttachments.size() != desc.numberOfColorAttachments)
                    throw std::runtime_error("Invalid number of color attachments");

                for (auto i = 0; i < colorAttachments.size(); i++) {
                    auto &att = colorAttachments.at(i);
                    switch (att.index()) {
                        case ATTACHMENT_TEXTURE: {
                            auto &tex = dynamic_cast<OGLTextureBuffer &>(
                                    std::get<std::reference_wrapper<TextureBuffer>>(att).get());
                            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(tex.getDescription().textureType),
                                                   tex.handle,
                                                   0);
                            break;
                        }
                        case ATTACHMENT_CUBEMAP: {
                            auto &pair = std::get<std::pair<CubeMapFace, std::reference_wrapper<TextureBuffer>>>(
                                    att);
                            auto &tex = dynamic_cast<OGLTextureBuffer &>(pair.second.get());
                            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(pair.first),
                                                   tex.handle,
                                                   0);
                            break;
                        }
                        case ATTACHMENT_TEXTUREARRAY: {
                            auto &pair = std::get<std::pair<size_t, std::reference_wrapper<TextureArrayBuffer>>>(att);
                            auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(pair.second.get());
                            glFramebufferTexture3D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(tex.getDescription().textureDesc.textureType),
                                                   tex.handle,
                                                   0,
                                                   static_cast<GLint>(pair.first));
                            break;
                        }
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            attachedColor = static_cast<int>(colorAttachments.size());

            checkGLError();
        }

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments,
                            RenderTargetAttachment depthStencilAttachment) override {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            if (colorAttachments.empty()) {
                for (int i = 0; i < attachedColor; i++) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
                }
            } else {
                if (colorAttachments.size() != desc.numberOfColorAttachments)
                    throw std::runtime_error("Invalid number of color attachments");

                for (auto i = 0; i < colorAttachments.size(); i++) {
                    auto &att = colorAttachments.at(i);
                    switch (att.index()) {
                        case ATTACHMENT_TEXTURE: {
                            auto &tex = dynamic_cast<OGLTextureBuffer &>(
                                    std::get<std::reference_wrapper<TextureBuffer>>(att).get());
                            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(tex.getDescription().textureType),
                                                   tex.handle,
                                                   0);
                            break;
                        }
                        case ATTACHMENT_CUBEMAP: {
                            auto &pair = std::get<std::pair<CubeMapFace, std::reference_wrapper<TextureBuffer>>>(
                                    att);
                            auto &tex = dynamic_cast<OGLTextureBuffer &>(pair.second.get());
                            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(pair.first),
                                                   tex.handle,
                                                   0);
                            break;
                        }
                        case ATTACHMENT_TEXTUREARRAY: {
                            auto &pair = std::get<std::pair<size_t, std::reference_wrapper<TextureArrayBuffer>>>(att);
                            auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(pair.second.get());
                            glFramebufferTexture3D(GL_FRAMEBUFFER,
                                                   GL_COLOR_ATTACHMENT0 + i,
                                                   convert(tex.getDescription().textureDesc.textureType),
                                                   tex.handle,
                                                   0,
                                                   static_cast<GLint>(pair.first));
                            break;
                        }
                    }
                }
            }

            switch (depthStencilAttachment.index()) {
                case ATTACHMENT_TEXTURE: {
                    auto &tex = dynamic_cast< OGLTextureBuffer &>(std::get<std::reference_wrapper<TextureBuffer>>(
                            depthStencilAttachment).get());
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(tex.getDescription().textureType),
                                           tex.handle,
                                           0);
                    break;
                }
                case ATTACHMENT_CUBEMAP: {
                    auto &pair = std::get<std::pair<CubeMapFace, std::reference_wrapper<TextureBuffer>>>(depthStencilAttachment);
                    auto &tex = dynamic_cast<OGLTextureBuffer &>(pair.second.get());
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(pair.first),
                                           tex.handle,
                                           0);
                    break;
                }
                case ATTACHMENT_TEXTUREARRAY: {
                    auto &pair = std::get<std::pair<size_t, std::reference_wrapper<TextureArrayBuffer>>>(depthStencilAttachment);
                    auto &tex = dynamic_cast<OGLTextureArrayBuffer &>(pair.second.get());
                    glFramebufferTexture3D(GL_FRAMEBUFFER,
                                           GL_DEPTH_STENCIL_ATTACHMENT,
                                           convert(tex.getDescription().textureDesc.textureType),
                                           tex.handle,
                                           0,
                                           static_cast<GLint>(pair.first));
                    break;
                }
            }

            attachedDepthStencil = true;

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            attachedColor = static_cast<int>(colorAttachments.size());

            checkGLError();
        }

        void clearDepthStencilAttachment() override {
            attachedDepthStencil = false;
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
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
