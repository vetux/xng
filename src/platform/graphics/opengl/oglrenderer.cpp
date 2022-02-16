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

#include <string>

#include "cast/numeric_cast.hpp"

#include "platform/graphics/opengl/oglrenderer.hpp"

#include "platform/graphics/opengl/oglshaderprogram.hpp"
#include "platform/graphics/opengl/ogltexturebuffer.hpp"
#include "platform/graphics/opengl/oglmeshbuffer.hpp"
#include "platform/graphics/opengl/oglrendertarget.hpp"

#include "platform/graphics/opengl/oglcheckerror.hpp"
#include "platform/graphics/opengl/ogltypeconverter.hpp"

#include "platform/graphics/opengl/openglinclude.hpp"

namespace xengine {
    namespace opengl {
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

        void OGLRenderer::renderBegin(RenderTarget &target, const RenderOptions &options) {
            auto clearColor = options.clearColorValue.divide();

            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClearDepth(options.clearDepthValue);

            if (options.multiSample)
                glEnable(GL_MULTISAMPLE);
            else
                glDisable(GL_MULTISAMPLE);

            if (options.multiSampleEnableFrequency)
                glEnable(GL_SAMPLE_COVERAGE);
            else
                glDisable(GL_SAMPLE_COVERAGE);

            glSampleCoverage(options.multiSampleFrequency, GL_TRUE);

            auto &fb = dynamic_cast<OGLRenderTarget &>(target);

            GLint vpData[4];
            glGetIntegerv(GL_VIEWPORT, vpData);

            glViewport(options.viewportOffset.x,
                       options.viewportOffset.y,
                       options.viewportSize.x,
                       options.viewportSize.y);

            glBindFramebuffer(GL_FRAMEBUFFER, fb.getFBO());

            auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (ret != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Render Target framebuffer is not complete: " + std::to_string(ret));
            }

            GLbitfield clearMask = 0;
            if (options.clearColor) {
                clearMask |= GL_COLOR_BUFFER_BIT;
            }

            if (options.clearDepth) {
                clearMask |= GL_DEPTH_BUFFER_BIT;
            }

            if (options.clearStencil) {
                clearMask |= GL_STENCIL_BUFFER_BIT;
            }

            glClear(clearMask);

            checkGLError("OGLRenderer::renderBegin");
        }

        void OGLRenderer::addCommand(RenderCommand &command) {
            drawCalls++;

            //Bind textures
            for (int i = 0; i < command.textures.size(); i++) {
                auto &texture = dynamic_cast<const OGLTextureBuffer &>(command.textures.at(i).get());
                glActiveTexture(getTextureSlot(i));
                glBindTexture(OGLTypeConverter::convert(texture.getAttributes().textureType), texture.handle);
            }

            //Bind shader program
            auto &shader = dynamic_cast<OGLShaderProgram &>(command.shader.get());
            shader.activate();

            glDepthFunc(OGLTypeConverter::convert(command.properties.depthTestMode));
            if (command.properties.depthTestWrite)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);

            //Setup per model depth, stencil, culling and blend states
            if (command.properties.enableDepthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            glStencilMask(command.properties.stencilTestMask);
            glStencilFunc(OGLTypeConverter::convert(command.properties.stencilMode),
                          command.properties.stencilReference,
                          command.properties.stencilFunctionMask);
            glStencilOp(OGLTypeConverter::convert(command.properties.stencilFail),
                        OGLTypeConverter::convert(command.properties.stencilDepthFail),
                        OGLTypeConverter::convert(command.properties.stencilPass));

            if (command.properties.enableStencilTest) {
                glEnable(GL_STENCIL_TEST);
            } else {
                glDisable(GL_STENCIL_TEST);
            }

            glCullFace(OGLTypeConverter::convert(command.properties.faceCullMode));
            if (command.properties.faceCullClockwiseWinding)
                glFrontFace(GL_CW);
            else
                glFrontFace(GL_CCW);

            if (command.properties.enableFaceCulling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }

            glBlendFunc(OGLTypeConverter::convert(command.properties.blendSourceMode),
                        OGLTypeConverter::convert(command.properties.blendDestinationMode));

            if (command.properties.enableBlending) {
                glEnable(GL_BLEND);
            } else {
                glDisable(GL_BLEND);
            }

            //Bind VAO and draw.
            auto &mesh = dynamic_cast<const OGLMeshBuffer &>(command.mesh.get());
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

            //Unbind textures
            for (int y = 0; y < 10; y++) {
                glActiveTexture(getTextureSlot(y));
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            }

            //Reset Stencil mask
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);

            checkGLError("OGLRenderer::addCommand");
        }

        void OGLRenderer::renderFinish() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            checkGLError("OGLRenderer::renderFinish");
        }

        void OGLRenderer::renderClear(RenderTarget &target, ColorRGBA color, float depth) {
            glEnable(GL_MULTISAMPLE);

            auto clearColor = color.divide();

            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClearDepth(depth);

            auto &fb = dynamic_cast<OGLRenderTarget &>(target);

            GLint vpData[4];
            glGetIntegerv(GL_VIEWPORT, vpData);

            auto size = fb.getSize();
            glViewport(0, 0, size.x, size.y);

            glBindFramebuffer(GL_FRAMEBUFFER, fb.getFBO());

            auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (ret != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Render Target framebuffer is not complete: " + std::to_string(ret));
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            checkGLError("OGLRenderer::renderClear");
        }

        void OGLRenderer::debugDrawCallRecordStart() {
            drawCalls = 0;
        }

        unsigned long OGLRenderer::debugDrawCallRecordStop() {
            return drawCalls;
        }
    }
}

#endif