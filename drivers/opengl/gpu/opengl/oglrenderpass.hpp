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

#ifndef XENGINE_OGLRENDERPASS_HPP
#define XENGINE_OGLRENDERPASS_HPP

#include "xng/gpu/renderpass.hpp"

#include <utility>

namespace xng::opengl {
    class OGLRenderPass : public RenderPass {
    public:
        std::function<void(RenderObject * )> destructor;

        RenderPassDesc passDesc;

        OGLRenderPipeline *mPipeline = nullptr;
        OGLVertexArrayObject *mVertexObject = nullptr;
        std::vector<ShaderData> mShaderBindings;

        OGLRenderPass(std::function<void(RenderObject * )> destructor,
                      RenderPassDesc passDesc)
                : destructor(std::move(destructor)),
                  passDesc(passDesc) {}

        ~OGLRenderPass() override = default;

        const RenderPassDesc &getDescription() override {
            return passDesc;
        }

        void beginRenderPass(RenderTarget &target, Vec2i viewportOffset, Vec2i viewportSize) override {
            auto &fb = dynamic_cast<OGLRenderTarget &>(target);

            if (fb.getDescription().hasDepthStencilAttachment != passDesc.hasDepthStencilAttachment
                || fb.getDescription().numberOfColorAttachments != passDesc.numberOfColorAttachments) {
                throw std::runtime_error("Invalid render target for render pass");
            }

            glViewport(viewportOffset.x,
                       viewportOffset.y,
                       viewportSize.x,
                       viewportSize.y);

            glBindFramebuffer(GL_FRAMEBUFFER, fb.getFBO());

            auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (ret != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Render Target framebuffer is not complete: " + std::to_string(ret));
            }

            checkGLError();
        }

        std::unique_ptr<GpuFence> endRenderPass() override {
            unbindVertexArrayObject();
            unbindShaderData();
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
            glDepthMask(GL_TRUE);
            glClearDepth(clearDepthValue);
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            if (mPipeline != nullptr
                && !mPipeline->getDescription().depthTestWrite) {
                glDepthMask(GL_FALSE);
            }
            checkGLError();
        }

        void setViewport(Vec2i viewportOffset, Vec2i viewportSize) override {
            glViewport(viewportOffset.x,
                       viewportOffset.y,
                       viewportSize.x,
                       viewportSize.y);
        }

        void bindPipeline(RenderPipeline &pipeline) override {
            if (!mShaderBindings.empty()) {
                if (mShaderBindings.size() != pipeline.getDescription().bindings.size()) {
                    throw std::runtime_error("Invalid bindings");
                }

                for (auto i = 0; i < mShaderBindings.size(); i++) {
                    if (getShaderDataType(mShaderBindings.at(i)) != pipeline.getDescription().bindings.at(i)) {
                        throw std::runtime_error("Invalid bindings");
                    }
                }
            }

            auto prevVert = mVertexObject;
            auto prevShaderData = mShaderBindings;

            unbindVertexArrayObject();
            unbindShaderData();

            auto &pip = dynamic_cast<OGLRenderPipeline &>(pipeline);

            mPipeline = &pip;

            auto &desc = pipeline.getDescription();

            if (desc.multiSample)
                glEnable(GL_MULTISAMPLE);
            else
                glDisable(GL_MULTISAMPLE);

            if (desc.multiSampleEnableFrequency)
                glEnable(GL_SAMPLE_COVERAGE);
            else
                glDisable(GL_SAMPLE_COVERAGE);

            glSampleCoverage(desc.multiSampleFrequency, GL_TRUE);

            // Bind shader program
            if (pip.programHandle) {
                glUseProgram(pip.programHandle);
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

            // Clear
            auto clearColor = desc.clearColorValue.divide();

            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClearDepth(desc.clearDepthValue);

            GLbitfield clearMask = 0;
            if (desc.clearColor && !desc.clearDepth && !desc.clearStencil) {
                clearMask = GL_COLOR_BUFFER_BIT;
            } else if (desc.clearColor && desc.clearDepth && !desc.clearStencil) {
                clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            } else if (desc.clearColor && desc.clearDepth && desc.clearStencil) {
                clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
            }

            glClear(clearMask);

            checkGLError();

            if (prevVert) {
                bindVertexArrayObject(*prevVert);
            }
            if (!prevShaderData.empty()) {
                bindShaderData(prevShaderData);
            }
        }

        void bindVertexArrayObject(VertexArrayObject &vertexArrayObject) override {
            mVertexObject = dynamic_cast<OGLVertexArrayObject *>(&vertexArrayObject);
            auto &obj = dynamic_cast<OGLVertexArrayObject &>(vertexArrayObject);
            glBindVertexArray(obj.VAO);
            checkGLError();
        }

        void unbindVertexArrayObject() override {
            mVertexObject = nullptr;
            glBindVertexArray(0);
            checkGLError();
        }

        void bindShaderData(const std::vector<ShaderData> &bindings) override {
            if (mPipeline) {
                if (bindings.size() != mPipeline->getDescription().bindings.size()) {
                    throw std::runtime_error("Invalid bindings");
                }

                for (auto i = 0; i < bindings.size(); i++) {
                    if (getShaderDataType(bindings.at(i)) != mPipeline->getDescription().bindings.at(i)) {
                        throw std::runtime_error("Invalid bindings");
                    }
                }
            }

            unbindShaderData();

            mShaderBindings = bindings;

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
                        auto shaderBuffer = dynamic_cast<OGLShaderUniformBuffer *>(&std::get<std::reference_wrapper<ShaderUniformBuffer>>(
                                b).get());
                        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, shaderBuffer->ubo);
                    }
                        break;
                }
            }
            checkGLError();
        }

        void unbindShaderData() override {
            if (!mShaderBindings.empty()) {
                //Unbind textures and uniform buffers
                for (int bindingPoint = 0; bindingPoint < mShaderBindings.size(); bindingPoint++) {
                    auto &b = mShaderBindings.at(bindingPoint);
                    switch (b.index()) {
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
            }
            mShaderBindings.clear();
        }

        void checkBindings(bool indexed) {
            if (mPipeline == nullptr)
                throw std::runtime_error("No pipeline bound");
            if (mVertexObject == nullptr)
                throw std::runtime_error("No vertex array object bound");
            else if (mVertexObject->mVertexBuffer == nullptr)
                throw std::runtime_error("No vertex buffer bound in vertex array object");
            else if (mVertexObject->mIndexBuffer == nullptr && indexed)
                throw std::runtime_error("No index buffer bound in vertex array object");

            for (auto i = 0; i < mPipeline->desc.bindings.size(); i++) {
                if (i >= mShaderBindings.size()) {
                    throw std::runtime_error("Invalid bound shader data size.");
                }
                switch (mShaderBindings.at(i).index()) {
                    case 0:
                        if (mPipeline->desc.bindings.at(i) != BIND_TEXTURE_BUFFER)
                            throw std::runtime_error("Invalid bound shader data type at index " + std::to_string(i));
                        break;
                    case 1:
                        if (mPipeline->desc.bindings.at(i) != BIND_TEXTURE_ARRAY_BUFFER)
                            throw std::runtime_error("Invalid bound shader data type at index " + std::to_string(i));
                        break;
                    case 2:
                        if (mPipeline->desc.bindings.at(i) != BIND_SHADER_BUFFER)
                            throw std::runtime_error("Invalid bound shader data type at index " + std::to_string(i));
                        break;
                }
            }
        }

        void drawArray(const DrawCall &drawCall) override {
            checkBindings(false);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawArrays(convert(mPipeline->getDescription().primitive),
                         static_cast<GLint>(drawCall.offset),
                         static_cast<GLsizei>(drawCall.count));
            checkGLError();
        }

        void drawIndexed(const DrawCall &drawCall) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawElements(convert(mPipeline->getDescription().primitive),
                           static_cast<GLsizei>(drawCall.count),
                           convert(drawCall.indexType),
                           reinterpret_cast<void *>(drawCall.offset));
            checkGLError();
        }

        void instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances) override {
            checkBindings(false);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawArraysInstanced(convert(mPipeline->getDescription().primitive),
                                  static_cast<GLint>(drawCall.offset),
                                  static_cast<GLsizei>(drawCall.count),
                                  static_cast<GLsizei>(numberOfInstances));
            checkGLError();
        }

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawElementsInstanced(convert(mPipeline->getDescription().primitive),
                                    static_cast<GLsizei>(drawCall.count),
                                    convert(drawCall.indexType),
                                    reinterpret_cast<void *>(drawCall.offset),
                                    static_cast<GLsizei>(numberOfInstances));
            checkGLError();
        }

        void multiDrawArray(const std::vector<DrawCall> &drawCalls) override {
            checkBindings(false);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            std::vector<GLint> first;
            std::vector<GLsizei> count;

            first.reserve(drawCalls.size());
            count.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                first.emplace_back(call.offset);
                count.emplace_back(call.count);
            }

            glMultiDrawArrays(convert(mPipeline->getDescription().primitive),
                              first.data(),
                              count.data(),
                              static_cast<GLsizei>(drawCalls.size()));
            checkGLError();
        }

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            std::vector<GLsizei> count;
            std::vector<unsigned int> indices;

            count.reserve(drawCalls.size());
            indices.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                count.emplace_back(call.count);
                indices.emplace_back(call.offset);
            }

            glMultiDrawElements(convert(mPipeline->getDescription().primitive),
                                count.data(),
                                GL_UNSIGNED_INT,
                                reinterpret_cast<const void *const *>(indices.data()),
                                static_cast<GLsizei>(drawCalls.size()));
            checkGLError();
        }

        void drawIndexed(const DrawCall &drawCall, size_t baseVertex) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawElementsBaseVertex(convert(mPipeline->getDescription().primitive),
                                     static_cast<GLsizei>(drawCall.count),
                                     convert(drawCall.indexType),
                                     reinterpret_cast<void *>(drawCall.offset),
                                     static_cast<GLint>(baseVertex));
            checkGLError();
        }

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances, size_t baseVertex) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            glDrawElementsInstancedBaseVertex(convert(mPipeline->getDescription().primitive),
                                              static_cast<GLsizei>(drawCall.count),
                                              convert(drawCall.indexType),
                                              reinterpret_cast<void *>(drawCall.offset),
                                              static_cast<GLsizei>(numberOfInstances),
                                              static_cast<GLint>(baseVertex));
            checkGLError();
        }

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls, std::vector<size_t> baseVertices) override {
            checkBindings(true);
            if (!mPipeline) {
                throw std::runtime_error("No pipeline bound");
            }
            std::vector<GLsizei> count;
            std::vector<void *> indices;

            count.reserve(drawCalls.size());
            indices.reserve(drawCalls.size());

            for (auto &call: drawCalls) {
                count.emplace_back(call.count);
                indices.emplace_back(reinterpret_cast<void *>(call.offset));
            }

            std::vector<GLint> vertices;
            for (auto &v: baseVertices)
                vertices.emplace_back(static_cast<GLint>(v));

            glMultiDrawElementsBaseVertex(convert(mPipeline->getDescription().primitive),
                                          count.data(),
                                          GL_UNSIGNED_INT,
                                          indices.data(),
                                          static_cast<GLsizei>(drawCalls.size()),
                                          vertices.data());
            checkGLError();
        }

        static GLuint getTextureSlot(int slot) {
            return GL_TEXTURE0 + slot;
        }
    };
}

#endif //XENGINE_OGLRENDERPASS_HPP
