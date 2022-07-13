/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_OGLRENDERPIPELINE_HPP
#define XENGINE_OGLRENDERPIPELINE_HPP

#include "gpu/renderpipeline.hpp"

#include "gpu/opengl/oglbuildmacro.hpp"

#include "gpu/opengl/oglrendertarget.hpp"
#include "gpu/opengl/oglshaderprogram.hpp"
#include "gpu/opengl/oglshaderbuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"

namespace xng::opengl {
    class OPENGL_TYPENAME(RenderPipeline) : public RenderPipeline OPENGL_INHERIT {
    public:
        RenderPipelineDesc desc;

        explicit OPENGL_TYPENAME(RenderPipeline)(const RenderPipelineDesc &desc)
                : desc(desc) {
            initialize();
        }

        void pinGpuMemory() override {}

        void unpinGpuMemory() override {}

        const RenderPipelineDesc &getDescription() override {
            return desc;
        }

        void render(RenderTarget &target, const std::vector<RenderPass> &passes) override {
            auto clearColor = desc.clearColorValue.divide();

            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClearDepth(desc.clearDepthValue);

            if (desc.multiSample)
                glEnable(GL_MULTISAMPLE);
            else
                glDisable(GL_MULTISAMPLE);

            if (desc.multiSampleEnableFrequency)
                glEnable(GL_SAMPLE_COVERAGE);
            else
                glDisable(GL_SAMPLE_COVERAGE);

            glSampleCoverage(desc.multiSampleFrequency, GL_TRUE);

            auto &fb = dynamic_cast<OPENGL_TYPENAME(RenderTarget) &>(target);

            GLint vpData[4];
            glGetIntegerv(GL_VIEWPORT, vpData);

            glViewport(desc.viewportOffset.x,
                       desc.viewportOffset.y,
                       desc.viewportSize.x,
                       desc.viewportSize.y);

            glBindFramebuffer(GL_FRAMEBUFFER, fb.getFBO());

            auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (ret != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Render Target framebuffer is not complete: " + std::to_string(ret));
            }

            GLbitfield clearMask = 0;
            if (desc.clearColor) {
                clearMask |= GL_COLOR_BUFFER_BIT;
            }

            if (desc.clearDepth) {
                clearMask |= GL_DEPTH_BUFFER_BIT;
            }

            if (desc.clearStencil) {
                clearMask |= GL_STENCIL_BUFFER_BIT;
            }

            glClear(clearMask);

            // Bind shader program
            auto &oglShader = dynamic_cast<OPENGL_TYPENAME(ShaderProgram) &>(desc.shader);
            oglShader.activate();

            // Setup pipeline state
            glDepthFunc(convert(desc.depthTestMode));
            if (desc.depthTestWrite)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);

            if (desc.enableDepthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            glStencilMask(desc.stencilTestMask);
            glStencilFunc(convert(desc.stencilMode),
                          desc.stencilReference,
                          desc.stencilFunctionMask);
            glStencilOp(convert(desc.stencilFail),
                        convert(desc.stencilDepthFail),
                        convert(desc.stencilPass));

            if (desc.enableStencilTest) {
                glEnable(GL_STENCIL_TEST);
            } else {
                glDisable(GL_STENCIL_TEST);
            }

            glCullFace(convert(desc.faceCullMode));
            if (desc.faceCullClockwiseWinding)
                glFrontFace(GL_CW);
            else
                glFrontFace(GL_CCW);

            if (desc.enableFaceCulling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }

            glBlendFunc(convert(desc.blendSourceMode),
                        convert(desc.blendDestinationMode));

            if (desc.enableBlending) {
                glEnable(GL_BLEND);
            } else {
                glDisable(GL_BLEND);
            }

            checkGLError("Render Pipeline Setup");

            // Draw commands
            for (auto &c: passes) {
                // Bind textures and uniform buffers
                for (int bindingPoint = 0; bindingPoint < c.getBindings().size(); bindingPoint++) {
                    auto &b = c.getBindings().at(bindingPoint);
                    OPENGL_TYPENAME(TextureBuffer) *texture;
                    OPENGL_TYPENAME(ShaderBuffer) *shaderBuffer;
                    switch (b.getType()) {
                        case RenderPass::TEXTURE_BUFFER:
                            texture = dynamic_cast<OPENGL_TYPENAME(TextureBuffer) *>(&b.getTextureBuffer());
                            glActiveTexture(getTextureSlot(bindingPoint));
                            glBindTexture(convert(texture->getDescription().textureType),
                                          texture->handle);
                            break;
                        case RenderPass::SHADER_BUFFER:
                            shaderBuffer = dynamic_cast<OPENGL_TYPENAME(ShaderBuffer) *>(&b.getShaderBuffer());
                            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, shaderBuffer->ubo);
                            break;
                    }
                }

                //Bind VAO and draw.
                auto &mesh = dynamic_cast<const OPENGL_TYPENAME(VertexBuffer) &>(c.getVertexBuffer());

                glBindVertexArray(mesh.VAO);

                if (mesh.indexed) {
                    if (mesh.instanced) {
                        glDrawElementsInstanced(mesh.elementType,
                                                numeric_cast<GLsizei>(mesh.elementCount),
                                                GL_UNSIGNED_INT,
                                                0,
                                                numeric_cast<GLsizei>(mesh.desc.numberOfInstances));
                    } else {
                        glDrawElements(mesh.elementType,
                                       numeric_cast<GLsizei>(mesh.elementCount),
                                       GL_UNSIGNED_INT,
                                       0);
                    }
                } else {
                    if (mesh.instanced) {
                        glDrawArraysInstanced(mesh.elementType,
                                              0,
                                              numeric_cast<GLsizei>(mesh.elementCount),
                                              numeric_cast<GLsizei>(mesh.desc.numberOfInstances));
                    } else {
                        glDrawArrays(mesh.elementType, 0, numeric_cast<GLsizei>(mesh.elementCount));
                    }
                }

                glBindVertexArray(0);

                //Unbind textures and uniform buffers
                for (int bindingPoint = 0; bindingPoint < c.getBindings().size(); bindingPoint++) {
                    auto &b = c.getBindings().at(bindingPoint);
                    switch (b.getType()) {
                        case RenderPass::TEXTURE_BUFFER:
                            glActiveTexture(getTextureSlot(bindingPoint));
                            glBindTexture(GL_TEXTURE_2D, 0);
                            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                            break;
                        case RenderPass::SHADER_BUFFER:
                            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
                            break;
                    }
                }

                checkGLError("OGLRenderer::addCommand");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            checkGLError("OGLRenderer::renderFinish");
        }

        std::vector<uint8_t> cache() override {
            throw std::runtime_error("Caching not implemented");
        }

        void setViewportSize(Vec2i viewportSize) override {
            desc.viewportSize = viewportSize;
        }

        OPENGL_MEMBERS

        OPENGL_CONVERSION_MEMBERS

    private:
        static GLuint getTextureSlot(int slot) {
            switch (slot) {
                case 0:
                    return GL_TEXTURE0;
                case 1:
                    return GL_TEXTURE1;
                case 2:
                    return GL_TEXTURE2;
                case 3:
                    return GL_TEXTURE3;
                case 4:
                    return GL_TEXTURE4;
                case 5:
                    return GL_TEXTURE5;
                case 6:
                    return GL_TEXTURE6;
                case 7:
                    return GL_TEXTURE7;
                case 8:
                    return GL_TEXTURE8;
                case 9:
                    return GL_TEXTURE9;
                default:
                    throw std::runtime_error("Maximum 10 texture slots");
            }
        }
    };
}

#endif //XENGINE_OGLRENDERPIPELINE_HPP
