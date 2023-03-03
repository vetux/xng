/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "xng/gpu/renderpipeline.hpp"

#include <utility>

#include "opengl_include.hpp"

#include "gpu/opengl/oglrendertarget.hpp"
#include "gpu/opengl/oglshaderprogram.hpp"
#include "gpu/opengl/oglshaderbuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"
#include "gpu/opengl/oglfence.hpp"
#include "gpu/opengl/oglvertexarrayobject.hpp"
#include "gpu/opengl/ogltexturearraybuffer.hpp"

#define TEXTURE_ID(index) GL_TEXTURE#index

namespace xng::opengl {
    class OGLRenderPipeline : public RenderPipeline {
    public:
        std::function<void(RenderObject *)> destructor;
        RenderPipelineDesc desc;

        GLuint programHandle = 0;

        explicit OGLRenderPipeline(std::function<void(RenderObject *)> destructor,
                                   RenderPipelineDesc descArg,
                                   SPIRVDecompiler &decompiler)
                : destructor(std::move(destructor)),
                  desc(std::move(descArg)) {
            if (!desc.shaders.empty()) {
                char *vertexSource, *fragmentSource, *geometrySource = nullptr;

                std::string vert, frag, geo;
                auto it = desc.shaders.find(VERTEX);
                if (it == desc.shaders.end())
                    throw std::runtime_error("No vertex shader");

                vert = decompiler.decompile(desc.shaders.at(VERTEX).getBlob(),
                                            it->second.getEntryPoint(),
                                            VERTEX,
                                            GLSL_420);
                vertexSource = vert.data();

                it = desc.shaders.find(FRAGMENT);
                if (it == desc.shaders.end())
                    throw std::runtime_error("No fragment shader");

                frag = decompiler.decompile(desc.shaders.at(FRAGMENT).getBlob(),
                                            it->second.getEntryPoint(),
                                            FRAGMENT,
                                            GLSL_420);
                fragmentSource = frag.data();

                it = desc.shaders.find(GEOMETRY);
                if (it != desc.shaders.end()) {
                    geo = decompiler.decompile(desc.shaders.at(GEOMETRY).getBlob(),
                                               it->second.getEntryPoint(),
                                               GEOMETRY,
                                               GLSL_420);
                    geometrySource = geo.data();
                }

                programHandle = glCreateProgram();

                GLuint vsH = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vsH, 1, &vertexSource, nullptr);
                glCompileShader(vsH);
                GLint success;
                glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(vsH, 512, nullptr, infoLog);
                    glDeleteShader(vsH);
                    std::string error = "Failed to compile vertex shader: ";
                    error.append(infoLog);
                    throw std::runtime_error(error);
                }

                glAttachShader(programHandle, vsH);

                GLuint gsH;

                if (geometrySource != nullptr) {
                    gsH = glCreateShader(GL_GEOMETRY_SHADER);
                    glShaderSource(gsH, 1, &geometrySource, nullptr);
                    glCompileShader(gsH);
                    glGetShaderiv(gsH, GL_COMPILE_STATUS, &success);
                    if (!success) {
                        char infoLog[512];
                        glGetShaderInfoLog(gsH, 512, nullptr, infoLog);
                        glDeleteShader(vsH);
                        glDeleteShader(gsH);
                        std::string error = "Failed to compile geometry shader: ";
                        error.append(infoLog);
                        throw std::runtime_error(error);
                    }
                    glAttachShader(programHandle, gsH);
                }

                GLuint fsH = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fsH, 1, &fragmentSource, nullptr);
                glCompileShader(fsH);
                glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(fsH, 512, nullptr, infoLog);
                    glDeleteShader(vsH);
                    if (geometrySource != nullptr)
                        glDeleteShader(gsH);
                    glDeleteShader(fsH);
                    std::string error = "Failed to compile fragment shader: ";
                    error.append(infoLog);
                    throw std::runtime_error(error);
                }
                glAttachShader(programHandle, fsH);

                glLinkProgram(programHandle);

                glDeleteShader(vsH);
                glDeleteShader(fsH);

                checkLinkSuccess();
            }

            checkGLError();
        }

        ~OGLRenderPipeline() override {
            glDeleteProgram(programHandle);
            destructor(this);
        }

        void checkLinkSuccess() const {
            auto msg = getLinkError();
            if (!msg.empty())
                throw std::runtime_error(msg);
        }

        std::string getLinkError() const {
            GLint success;
            glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
                std::string error = "Failed to link shader program: ";
                error.append(infoLog);
                return error;
            }
            return "";
        }

        const RenderPipelineDesc &getDescription() override {
            return desc;
        }

        void renderBegin(RenderTarget &target, Vec2i viewportOffset, Vec2i viewportSize) override {
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

            auto &fb = dynamic_cast<OGLRenderTarget &>(target);

            GLint vpData[4];
            glGetIntegerv(GL_VIEWPORT, vpData);

            glViewport(viewportOffset.x,
                       viewportOffset.y,
                       viewportSize.x,
                       viewportSize.y);

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
            if (programHandle) {
                glUseProgram(programHandle);
            }

            checkGLError();

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

            checkGLError();
        }

        std::unique_ptr<GpuFence> renderPresent() override {
            //Unbind textures and uniform buffers
            for (int bindingPoint = 0; bindingPoint < desc.bindings.size(); bindingPoint++) {
                auto &b = desc.bindings.at(bindingPoint);
                switch (b) {
                    case BIND_TEXTURE_BUFFER:
                        glActiveTexture(getTextureSlot(bindingPoint));
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                        break;
                    case BIND_TEXTURE_ARRAY_BUFFER:
                        glActiveTexture(getTextureSlot(bindingPoint));
                        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
                        break;
                    case BIND_SHADER_BUFFER:
                        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
                        break;
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            checkGLError();

            return std::make_unique<OGLFence>();
        }

        void clearColorAttachments(ColorRGBA val) override {
            auto clearColor = val.divide();
            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClear(GL_COLOR_BUFFER_BIT);
            checkGLError();
        }

        void clearDepthAttachments(float clearDepthValue) override {
            glClearDepth(clearDepthValue);
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            checkGLError();
        }

        void bindVertexArrayObject(VertexArrayObject &vertexArrayObject) override {
            auto &obj = dynamic_cast<OGLVertexArrayObject &>(vertexArrayObject);
            glBindVertexArray(obj.VAO);
            checkGLError();
        }

        void bindShaderData(const std::vector<ShaderData> &bindings) override {
            if (bindings.size() != desc.bindings.size()) {
                throw std::runtime_error("Invalid bindings");
            }
            for (auto i = 0; i < bindings.size(); i++) {
                if (getShaderDataType(bindings.at(i)) != desc.bindings.at(i)) {
                    throw std::runtime_error("Invalid bindings");
                }
            }

            // Bind textures and uniform buffers
            for (int bindingPoint = 0; bindingPoint < bindings.size(); bindingPoint++) {
                auto &b = bindings.at(bindingPoint);
                switch (b.index()) {
                    case 0: {
                        auto texture = dynamic_cast<OGLTextureBuffer *>(&std::get<std::reference_wrapper<TextureBuffer>>(
                                b).get());
                        glActiveTexture(getTextureSlot(bindingPoint));
                        glBindTexture(convert(texture->getDescription().textureType), texture->handle);
                    }
                        break;
                    case 1: {
                        auto textureArray = dynamic_cast<OGLTextureArrayBuffer *>(&std::get<std::reference_wrapper<TextureArrayBuffer>>(
                                b).get());
                        glActiveTexture(getTextureSlot(bindingPoint));
                        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray->handle);
                    }
                        break;
                    case 2: {
                        auto shaderBuffer = dynamic_cast<OGLShaderBuffer *>(&std::get<std::reference_wrapper<ShaderBuffer>>(
                                b).get());
                        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, shaderBuffer->ubo);
                    }
                        break;
                }
            }
            checkGLError();
        }

        void drawArray(const DrawCall &drawCall) override {
            glDrawArrays(convert(desc.primitive),
                         static_cast<GLint>(drawCall.offset),
                         static_cast<GLsizei>(drawCall.count));
            checkGLError();
        }

        void drawIndexed(const DrawCall &drawCall) override {
            glDrawElements(convert(desc.primitive),
                           static_cast<GLsizei>(drawCall.count),
                           convert(drawCall.indexType),
                           reinterpret_cast<void *>(drawCall.offset));
            checkGLError();
        }

        void instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances) override {
            glDrawArraysInstanced(convert(desc.primitive),
                                  static_cast<GLint>(drawCall.offset),
                                  static_cast<GLsizei>(drawCall.count),
                                  static_cast<GLsizei>(numberOfInstances));
            checkGLError();
        }

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances) override {
            glDrawElementsInstanced(convert(desc.primitive),
                                    static_cast<GLsizei>(drawCall.count),
                                    convert(drawCall.indexType),
                                    reinterpret_cast<void *>(drawCall.offset),
                                    static_cast<GLsizei>(numberOfInstances));
            checkGLError();
        }

        void multiDrawArray(const std::vector<DrawCall> &drawCalls) override {
            std::vector<GLint> first;
            std::vector<GLsizei> count;

            first.reserve(drawCalls.size());
            count.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                first.emplace_back(call.offset);
                count.emplace_back(call.count);
            }

            glMultiDrawArrays(convert(desc.primitive),
                              first.data(),
                              count.data(),
                              static_cast<GLsizei>(drawCalls.size()));
            checkGLError();
        }

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls) override {
            std::vector<GLsizei> count;
            std::vector<unsigned int> indices;

            count.reserve(drawCalls.size());
            indices.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                count.emplace_back(call.count);
                indices.emplace_back(call.offset);
            }

            glMultiDrawElements(convert(desc.primitive),
                                count.data(),
                                GL_UNSIGNED_INT,
                                reinterpret_cast<const void *const *>(indices.data()),
                                static_cast<GLsizei>(drawCalls.size()));
            checkGLError();
        }

        void drawIndexed(const DrawCall &drawCall, size_t baseVertex) override {
            glDrawElementsBaseVertex(convert(desc.primitive),
                                     static_cast<GLsizei>(drawCall.count),
                                     convert(drawCall.indexType),
                                     reinterpret_cast<void *>(drawCall.offset),
                                     static_cast<GLint>(baseVertex));
            checkGLError();
        }

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances, size_t baseVertex) override {
            glDrawElementsInstancedBaseVertex(convert(desc.primitive),
                                              static_cast<GLsizei>(drawCall.count),
                                              convert(drawCall.indexType),
                                              reinterpret_cast<void *>(drawCall.offset),
                                              static_cast<GLsizei>(numberOfInstances),
                                              static_cast<GLint>(baseVertex));
            checkGLError();
        }

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls, std::vector<size_t> baseVertices) override {
            std::vector<GLsizei> count;
            std::vector<unsigned int> indices;

            count.reserve(drawCalls.size());
            indices.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                count.emplace_back(call.count);
                indices.emplace_back(call.offset);
            }

            std::vector<GLint> vertices;
            for (auto &v: baseVertices)
                vertices.emplace_back(static_cast<GLint>(v));

            glMultiDrawElementsBaseVertex(convert(desc.primitive),
                                          count.data(),
                                          GL_UNSIGNED_INT,
                                          reinterpret_cast<const void *const *>(indices.data()),
                                          static_cast<GLsizei>(drawCalls.size()),
                                          vertices.data());
            checkGLError();
        }

        std::vector<uint8_t> cache() override {
            throw std::runtime_error("Not Implemented");
        }

    private:
        static GLuint getTextureSlot(int slot) {
            return GL_TEXTURE0 + slot;
        }
    };
}

#endif //XENGINE_OGLRENDERPIPELINE_HPP
