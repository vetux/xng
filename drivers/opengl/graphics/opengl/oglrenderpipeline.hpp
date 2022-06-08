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

#ifndef XENGINE_OGLRENDERPIPELINE_HPP
#define XENGINE_OGLRENDERPIPELINE_HPP

#include "graphics/renderpipeline.hpp"

#include "graphics/opengl/oglbuildmacro.hpp"

#include "graphics/opengl/oglrendertarget.hpp"
#include "graphics/opengl/oglshaderprogram.hpp"
#include "graphics/opengl/oglshaderbuffer.hpp"
#include "graphics/opengl/oglmeshbuffer.hpp"
#include "graphics/opengl/ogltexturebuffer.hpp"

namespace xengine::opengl {
    class OPENGL_TYPENAME(RenderPipeline) : public RenderPipeline OPENGL_INHERIT {
    public:
        RenderPipelineDesc desc;

        explicit OPENGL_TYPENAME(RenderPipeline)(RenderPipelineDesc desc)
                : desc(desc) {
            initialize();
        }

        const RenderPipelineDesc &getDescription() override {
            return desc;
        }

        void render(RenderTarget &target, const std::vector<RenderCommand> &commands) override {
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
            if (desc.shader == nullptr)
                throw std::runtime_error("Nullptr shader");

            auto &shader = dynamic_cast<OPENGL_TYPENAME(ShaderProgram) &>(*desc.shader);
            shader.activate();

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
            for (auto &c: commands) {
                // Bind textures and uniform buffers
                for (int i = 0; i < c.shaderBindings.size(); i++) {
                    auto &b = c.shaderBindings.at(i);
                    OPENGL_TYPENAME(TextureBufferView) *texture;
                    OPENGL_TYPENAME(ShaderBuffer) *shaderBuffer;
                    switch (b.type) {
                        case RenderCommand::TEXTURE_BUFFER:
                            texture = dynamic_cast<OPENGL_TYPENAME(TextureBufferView) *>(&b.getTextureBuffer());
                            glActiveTexture(getTextureSlot(i));
                            glBindTexture(convert(texture->buffer->getDescription().textureType),
                                          texture->buffer->handle);
                            break;
                        case RenderCommand::SHADER_BUFFER:
                            shaderBuffer = dynamic_cast<OPENGL_TYPENAME(ShaderBuffer) *>(&b.getShaderBuffer());
#warning NOT IMPLEMENTED
                            break;
                    }
                }

                //Bind VAO and draw.
                auto &meshView = dynamic_cast<const OPENGL_TYPENAME(MeshBufferView) &>(*c.mesh);
                auto &mesh = *meshView.buffer;

                glBindVertexArray(mesh.VAO);

                if (mesh.indexed) {
                    if (mesh.instanced)
                        glDrawElementsInstanced(mesh.elementType,
                                                numeric_cast<GLsizei>(mesh.elementCount),
                                                GL_UNSIGNED_INT,
                                                0,
                                                numeric_cast<GLsizei>(mesh.instanceCount));
                    else
                        glDrawElements(mesh.elementType,
                                       numeric_cast<GLsizei>(mesh.elementCount),
                                       GL_UNSIGNED_INT,
                                       0);
                } else {
                    if (mesh.instanced)
                        glDrawArraysInstanced(mesh.elementType,
                                              0,
                                              numeric_cast<GLsizei>(mesh.elementCount),
                                              numeric_cast<GLsizei>(mesh.instanceCount));
                    else
                        glDrawArrays(mesh.elementType, 0, numeric_cast<GLsizei>(mesh.elementCount));
                }

                glBindVertexArray(0);

                //Unbind textures and uniform buffers
                for (int i = 0; i < c.shaderBindings.size(); i++) {
                    auto &b = c.shaderBindings.at(i);
                    switch (b.type) {
                        case RenderCommand::TEXTURE_BUFFER:
                            glActiveTexture(getTextureSlot(i));
                            glBindTexture(GL_TEXTURE_2D, 0);
                            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                            break;
                        case RenderCommand::SHADER_BUFFER:
#warning NOT IMPLEMENTED
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
