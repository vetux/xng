/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_OGLCOMMANDQUEUE_HPP
#define XENGINE_OGLCOMMANDQUEUE_HPP

#include "xng/gpu/commandqueue.hpp"

#include "gpu/opengl/oglfence.hpp"

#include "gpu/opengl/oglcomputepipeline.hpp"

namespace xng::opengl {
    typedef struct {
        unsigned int count;
        unsigned int instanceCount;
        unsigned int firstIndex;
        int baseVertex;
        unsigned int baseInstance;
    } DrawElementsIndirectCommand;

    typedef  struct {
        unsigned int  count;
        unsigned int  instanceCount;
        unsigned int  first;
        unsigned int  baseInstance;
    } DrawArraysIndirectCommand;

    class OGLCommandQueue : public CommandQueue {
    public:
        static GLuint getTextureSlot(int slot) {
            return GL_TEXTURE0 + slot;
        }

        OGLRenderPipeline *mRenderPipeline = nullptr;
        OGLVertexArrayObject *mVertexObject = nullptr;
        std::vector<ShaderResource> mShaderBindings;

        OGLComputePipeline *mComputePipeline = nullptr;

        RenderStatistics &stats;

        bool runningPass = false;

        GLuint indirectBuffer;

        explicit OGLCommandQueue(RenderStatistics &stats) : stats(stats) {
            glGenBuffers(1, &indirectBuffer);
            checkGLError();
        }

        ~OGLCommandQueue() override {
            glDeleteBuffers(1, &indirectBuffer);
        }

        Type getType() override {
            return RENDER_OBJECT_COMMAND_QUEUE;
        }

        std::unique_ptr<CommandFence> submit(const std::vector<std::reference_wrapper<CommandBuffer>> &buffers,
                                             const std::vector<std::shared_ptr<CommandSemaphore>> &waitSemaphores,
                                             const std::vector<std::shared_ptr<CommandSemaphore>> &signalSemaphores) override {
            for (auto &buffer: buffers) {
                auto &buf = dynamic_cast<OGLCommandBuffer &>(buffer.get());
                for (auto &c: buf.commands) {
                    runCommand(c);
                }
            }
            return std::unique_ptr<OGLFence>();
        }

        void ensureRunningPass() {
            if (!runningPass)
                throw std::runtime_error("Pass is not running.");
        }

        void unbindVertexArrayObject() {
            ensureRunningPass();
            mVertexObject = nullptr;
            glBindVertexArray(0);
            checkGLError();
        }

        void unbindShaderData() {
            if (!mShaderBindings.empty()) {
                //Unbind textures and uniform buffers
                for (int bindingPoint = 0; bindingPoint < mShaderBindings.size(); bindingPoint++) {
                    auto &b = mShaderBindings.at(bindingPoint);
                    auto bind = mRenderPipeline ? mRenderPipeline->desc.bindings.at(bindingPoint)
                                                : mComputePipeline->desc.bindings.at(bindingPoint);
                    switch (bind) {
                        case BIND_TEXTURE_BUFFER:
                            glActiveTexture(getTextureSlot(bindingPoint));
                            glBindTexture(GL_TEXTURE_2D, 0);
                            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                            break;
                        case BIND_TEXTURE_ARRAY_BUFFER:
                            glActiveTexture(getTextureSlot(bindingPoint));
                            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
                            break;
                        case BIND_SHADER_UNIFORM_BUFFER:
                            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
                            break;
                        case BIND_SHADER_STORAGE_BUFFER:
                            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, 0);
                            break;
                        case BIND_IMAGE_BUFFER:
                        case BIND_IMAGE_ARRAY_BUFFER:
                            glBindImageTexture(bindingPoint, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA8);
                            break;
                    }
                }
            }
            mShaderBindings.clear();
            checkGLError();
        }

        void checkBindings(bool indexed) {
            if (mRenderPipeline == nullptr)
                throw std::runtime_error("No pipeline bound");
            if (mVertexObject == nullptr)
                throw std::runtime_error("No vertex array object bound");
            else if (mVertexObject->mVertexBuffer == nullptr)
                throw std::runtime_error("No vertex buffer bound in vertex array object");
            else if (mVertexObject->mIndexBuffer == nullptr && indexed)
                throw std::runtime_error("No index buffer bound in vertex array object");

            for (auto i = 0; i < mRenderPipeline->desc.bindings.size(); i++) {
                if (i >= mShaderBindings.size()) {
                    break;
                }
                auto bindingType = static_cast<ShaderDataType>(mShaderBindings.at(i).data.index());
                auto reqBindingType = mRenderPipeline->desc.bindings.at(i);
                switch (reqBindingType) {
                    case BIND_TEXTURE_BUFFER:
                        if (bindingType != SHADER_TEXTURE_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                    case BIND_TEXTURE_ARRAY_BUFFER:
                        if (bindingType != SHADER_TEXTURE_ARRAY_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                    case BIND_IMAGE_BUFFER:
                        if (bindingType != SHADER_TEXTURE_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                    case BIND_IMAGE_ARRAY_BUFFER:
                        if (bindingType != SHADER_TEXTURE_ARRAY_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                    case BIND_SHADER_UNIFORM_BUFFER:
                        if (bindingType != SHADER_UNIFORM_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                    case BIND_SHADER_STORAGE_BUFFER:
                        if (bindingType != SHADER_STORAGE_BUFFER) {
                            throw std::runtime_error(
                                    "Invalid bound shader data type at index " + std::to_string(i));
                        }
                        break;
                }
            }
        }

        void runCommand(const Command &c) {
            switch (c.type) {
                case Command::NONE:
                    break;
                case Command::COPY_INDEX_BUFFER: {
                    auto data = std::get<IndexBufferCopy>(c.data);
                    auto &buf = dynamic_cast<OGLIndexBuffer &>(*data.source);
                    auto &target = dynamic_cast<OGLIndexBuffer &>(*data.target);
                    if (data.readOffset >= buf.desc.size
                        || data.readOffset + data.count > buf.desc.size
                        || data.writeOffset >= target.desc.size
                        || data.writeOffset + data.count > target.desc.size) {
                        throw std::runtime_error("Invalid copy range");
                    }
                    glBindBuffer(GL_COPY_READ_BUFFER, buf.EBO);
                    glBindBuffer(GL_COPY_WRITE_BUFFER, target.EBO);
                    glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                        GL_COPY_WRITE_BUFFER,
                                        static_cast<GLintptr>(data.readOffset),
                                        static_cast<GLintptr>(data.writeOffset),
                                        static_cast<GLsizeiptr>(data.count));
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
                    checkGLError();
                    break;
                }
                case Command::BLIT_COLOR: {
                    auto data = std::get<RenderTargetBlit>(c.data);
                    auto &src = dynamic_cast<OGLRenderTarget &>(*data.source);
                    auto &target = dynamic_cast<OGLRenderTarget &>(*data.target);

                    if (data.sourceRect.x < 0 || data.sourceRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.sourceOffset.x < 0 || data.sourceOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }
                    if (data.targetRect.x < 0 || data.targetRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.targetOffset.x < 0 || data.targetOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }

                    auto &fbS = dynamic_cast<OGLRenderTarget &>(*data.source);

                    Vec2i sourceSize = fbS.getDescription().size;
                    if (sourceSize.x < data.sourceRect.x + data.sourceOffset.x ||
                        sourceSize.y < data.sourceRect.y + data.sourceOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                    Vec2i targetSize = target.desc.size;
                    if (targetSize.x < data.targetRect.x + data.targetOffset.x ||
                        targetSize.y < data.targetRect.y + data.targetOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

                    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());

                    // The default framebuffer always reads/writes from/to default color buffer.
                    if (fbS.getFBO() != 0) {
                        glReadBuffer(getColorAttachment(data.sourceIndex));
                    }

                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.getFBO());

                    // The default framebuffer always reads/writes from/to default color buffer.
                    if (target.getFBO() != 0) {
                        glDrawBuffer(getColorAttachment(data.targetIndex));
                    }

                    glBlitFramebuffer(data.sourceOffset.x,
                                      data.sourceOffset.y,
                                      data.sourceRect.x,
                                      data.sourceRect.y,
                                      data.targetOffset.x,
                                      data.targetOffset.y,
                                      data.targetRect.x,
                                      data.targetRect.y,
                                      GL_COLOR_BUFFER_BIT,
                                      convert(data.filter));

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    checkGLError();
                    break;
                }
                case Command::BLIT_DEPTH: {
                    auto data = std::get<RenderTargetBlit>(c.data);
                    auto &src = dynamic_cast<OGLRenderTarget &>(*data.source);
                    auto &target = dynamic_cast<OGLRenderTarget &>(*data.target);

                    if (data.sourceRect.x < 0 || data.sourceRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.sourceOffset.x < 0 || data.sourceOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }
                    if (data.targetRect.x < 0 || data.targetRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.targetOffset.x < 0 || data.targetOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }

                    Vec2i sourceSize = src.getDescription().size;
                    if (sourceSize.x < data.sourceRect.x + data.sourceOffset.x ||
                        sourceSize.y < data.sourceRect.y + data.sourceOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                    Vec2i targetSize = target.desc.size;
                    if (targetSize.x < data.targetRect.x + data.targetOffset.x ||
                        targetSize.y < data.targetRect.y + data.targetOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

                    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.getFBO());
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.getFBO());

                    glBlitFramebuffer(data.sourceOffset.x,
                                      data.sourceOffset.y,
                                      data.sourceRect.x,
                                      data.sourceRect.y,
                                      data.targetOffset.x,
                                      data.targetOffset.y,
                                      data.targetRect.x,
                                      data.targetRect.y,
                                      GL_DEPTH_BUFFER_BIT,
                                      GL_NEAREST);

                    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                    checkGLError();
                    break;
                }
                case Command::BLIT_STENCIL: {
                    auto data = std::get<RenderTargetBlit>(c.data);
                    auto &target = dynamic_cast<OGLRenderTarget &>(*data.target);

                    if (data.sourceRect.x < 0 || data.sourceRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.sourceOffset.x < 0 || data.sourceOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }
                    if (data.targetRect.x < 0 || data.targetRect.y < 0) {
                        throw std::runtime_error("Rect cannot be negative");
                    }
                    if (data.targetOffset.x < 0 || data.targetOffset.y < 0) {
                        throw std::runtime_error("Offset cannot be negative");
                    }

                    auto &fbS = dynamic_cast< OGLRenderTarget &>(*data.source);

                    Vec2i sourceSize = fbS.getDescription().size;
                    if (sourceSize.x < data.sourceRect.x + data.sourceOffset.x ||
                        sourceSize.y < data.sourceRect.y + data.sourceOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for source framebuffer");

                    Vec2i targetSize = target.desc.size;
                    if (targetSize.x < data.targetRect.x + data.targetOffset.x ||
                        targetSize.y < data.targetRect.y + data.targetOffset.y)
                        throw std::runtime_error("Blit rect out of bounds for target framebuffer.");

                    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbS.getFBO());
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.getFBO());

                    glBlitFramebuffer(data.sourceOffset.x,
                                      data.sourceOffset.y,
                                      data.sourceRect.x,
                                      data.sourceRect.y,
                                      data.targetOffset.x,
                                      data.targetOffset.y,
                                      data.targetRect.x,
                                      data.targetRect.y,
                                      GL_STENCIL_BUFFER_BIT,
                                      GL_NEAREST);

                    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                    checkGLError();
                    break;
                }
                case Command::BEGIN_PASS: {
                    if (runningPass) {
                        throw std::runtime_error("Pass is already running (Nested calls to begin ?)");
                    }

                    auto data = std::get<RenderPassBegin>(c.data);

                    auto &pass = dynamic_cast<OGLRenderPass &>(*data.pass);

                    auto &fb = dynamic_cast<OGLRenderTarget &>(*data.target);

                    if (fb.getDescription().hasDepthStencilAttachment != pass.passDesc.hasDepthStencilAttachment
                        || fb.getDescription().numberOfColorAttachments != pass.passDesc.numberOfColorAttachments) {
                        throw std::runtime_error("Invalid render target for render pass");
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, fb.getFBO());

#ifndef NDEBUG
                    auto ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                    if (ret != GL_FRAMEBUFFER_COMPLETE) {
                        throw std::runtime_error(
                                "Render Target framebuffer is not complete: " + std::to_string(ret));
                    }
#endif

                    glEnable(GL_LINE_SMOOTH); // Enable smooth lines by default

                    checkGLError();

                    runningPass = true;

                    break;
                }
                case Command::END_PASS:
                    ensureRunningPass();
                    unbindVertexArrayObject();
                    unbindShaderData();
                    mRenderPipeline = nullptr;
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    checkGLError();
                    runningPass = false;
                    break;
                case Command::CLEAR_COLOR: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassClear>(c.data);
                    auto clearColor = data.color.divide();
                    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
                    glClear(GL_COLOR_BUFFER_BIT);
                    checkGLError();
                    break;
                }
                case Command::CLEAR_DEPTH: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassClear>(c.data);
                    glClearDepth(data.depth);
                    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    checkGLError();
                    break;
                }
                case Command::SET_VIEWPORT: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassViewport>(c.data);
                    glViewport(data.viewportOffset.x,
                               data.viewportOffset.y,
                               data.viewportSize.x,
                               data.viewportSize.y);
                    checkGLError();
                    break;
                }
                case Command::DRAW_ARRAY: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(false);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawArrays(convert(mRenderPipeline->getDescription().primitive),
                                 static_cast<GLint>(data.drawCalls.at(0).offset),
                                 static_cast<GLsizei>(data.drawCalls.at(0).count));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawElements(convert(mRenderPipeline->getDescription().primitive),
                                   static_cast<GLsizei>(data.drawCalls.at(0).count),
                                   convert(data.drawCalls.at(0).indexType),
                                   reinterpret_cast<void *>(data.drawCalls.at(0).offset));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_ARRAY_INSTANCED: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(false);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawArraysInstanced(convert(mRenderPipeline->getDescription().primitive),
                                          static_cast<GLint>(data.drawCalls.at(0).offset),
                                          static_cast<GLsizei>(data.drawCalls.at(0).count),
                                          static_cast<GLsizei>(data.numberOfInstances));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED_INSTANCED: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawElementsInstanced(convert(mRenderPipeline->getDescription().primitive),
                                            static_cast<GLsizei>(data.drawCalls.at(0).count),
                                            convert(data.drawCalls.at(0).indexType),
                                            reinterpret_cast<void *>(data.drawCalls.at(0).offset),
                                            static_cast<GLsizei>(data.numberOfInstances));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_ARRAY_MULTI: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(false);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }

                    std::vector<DrawArraysIndirectCommand> cmds;
                    for (auto & drawCall : data.drawCalls) {
                        DrawArraysIndirectCommand cmd;
                        cmd.count = drawCall.count;
                        cmd.first = drawCall.offset;
                        cmd.instanceCount = 1;
                        cmd.baseInstance = 0;
                        cmds.emplace_back(cmd);
                    }

                    auto cmdSize = static_cast<GLsizeiptr >(cmds.size() * sizeof(DrawElementsIndirectCommand));

                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
                    glBufferData(GL_DRAW_INDIRECT_BUFFER,
                                 static_cast<GLsizeiptr >(cmds.size() * sizeof(DrawElementsIndirectCommand)),
                                 reinterpret_cast<void *>(cmds.data()),
                                 GL_DYNAMIC_DRAW);
                    glMultiDrawArraysIndirect(convert(mRenderPipeline->getDescription().primitive),
                                                nullptr,
                                                static_cast<GLsizei>(cmds.size()),
                                                0);
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

                    stats.uploadCommand += cmdSize;

                    stats.drawCalls++;
                    for (auto &call: data.drawCalls) {
                        stats.polys += call.count / mRenderPipeline->getDescription().primitive;
                    }
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED_MULTI: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    std::vector<DrawElementsIndirectCommand> cmds;
                    for (auto i = 0; i < data.drawCalls.size(); i++) {
                        DrawElementsIndirectCommand cmd;
                        cmd.count = data.drawCalls.at(i).count;
                        cmd.instanceCount = 1;
                        cmd.firstIndex = data.drawCalls.at(i).offset / getIndexTypeSize(data.drawCalls.at(i).indexType);
                        cmd.baseVertex = 0;
                        cmd.baseInstance = 0;
                        cmds.emplace_back(cmd);
                    }

                    auto cmdSize = static_cast<GLsizeiptr >(cmds.size() * sizeof(DrawElementsIndirectCommand));

                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
                    glBufferData(GL_DRAW_INDIRECT_BUFFER,
                                 cmdSize,
                                 reinterpret_cast<void *>(cmds.data()),
                                 GL_DYNAMIC_DRAW);
                    glMultiDrawElementsIndirect(convert(mRenderPipeline->getDescription().primitive),
                                                GL_UNSIGNED_INT,
                                                nullptr,
                                                static_cast<GLsizei>(cmds.size()),
                                                0);
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

                    stats.uploadCommand += cmdSize;

                    stats.drawCalls++;
                    for (auto &call: data.drawCalls) {
                        stats.polys += call.count / mRenderPipeline->getDescription().primitive;
                    }
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED_BASE_VERTEX: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawElementsBaseVertex(convert(mRenderPipeline->getDescription().primitive),
                                             static_cast<GLsizei>(data.drawCalls.at(0).count),
                                             convert(data.drawCalls.at(0).indexType),
                                             reinterpret_cast<void *>(data.drawCalls.at(0).offset),
                                             static_cast<GLint>(data.baseVertices.at(0)));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED_INSTANCED_BASE_VERTEX: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }
                    glDrawElementsInstancedBaseVertex(convert(mRenderPipeline->getDescription().primitive),
                                                      static_cast<GLsizei>(data.drawCalls.at(0).count),
                                                      convert(data.drawCalls.at(0).indexType),
                                                      reinterpret_cast<void *>(data.drawCalls.at(0).offset),
                                                      static_cast<GLsizei>(data.numberOfInstances),
                                                      static_cast<GLint>(data.baseVertices.at(0)));
                    stats.drawCalls++;
                    stats.polys += data.drawCalls.at(0).count / mRenderPipeline->getDescription().primitive;
                    checkGLError();
                    break;
                }
                case Command::DRAW_INDEXED_MULTI_BASE_VERTEX: {
                    ensureRunningPass();
                    auto data = std::get<RenderPassDraw>(c.data);
                    checkBindings(true);
                    if (!mRenderPipeline) {
                        throw std::runtime_error("No pipeline bound");
                    }

                    std::vector<DrawElementsIndirectCommand> cmds;
                    for (auto i = 0; i < data.drawCalls.size(); i++) {
                        DrawElementsIndirectCommand cmd;
                        cmd.count = data.drawCalls.at(i).count;
                        cmd.instanceCount = 1;
                        cmd.firstIndex = data.drawCalls.at(i).offset / getIndexTypeSize(data.drawCalls.at(i).indexType);
                        cmd.baseVertex = static_cast<int>(data.baseVertices.at(i));
                        cmd.baseInstance = 0;
                        cmds.emplace_back(cmd);
                    }

                    // Because on Mesa glMultiDrawElementsBaseVertex does not draw the meshes after exceeding a certain
                    // draw count we use glMultiDrawElementsIndirect which might incur a small overhead but appears to work correctly on mesa.
                    auto cmdSize = static_cast<GLsizeiptr >(cmds.size() * sizeof(DrawElementsIndirectCommand));
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
                    glBufferData(GL_DRAW_INDIRECT_BUFFER,
                                 cmdSize,
                                 reinterpret_cast<void *>(cmds.data()),
                                 GL_DYNAMIC_DRAW);
                    glMultiDrawElementsIndirect(convert(mRenderPipeline->getDescription().primitive),
                                                GL_UNSIGNED_INT,
                                                nullptr,
                                                static_cast<GLsizei>(cmds.size()),
                                                0);
                    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

                    stats.uploadCommand += cmdSize;

                    stats.drawCalls++;
                    for (auto &call: data.drawCalls) {
                        stats.polys += call.count / mRenderPipeline->getDescription().primitive;
                    }
                    checkGLError();
                    break;
                }
                case Command::BIND_PIPELINE: {
                    ensureRunningPass();
                    auto data = std::get<RenderPipelineBind>(c.data);

                    auto &pip = dynamic_cast<OGLRenderPipeline &>(*data.pipeline);

                    mRenderPipeline = &pip;

                    auto &desc = data.pipeline->getDescription();

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

                    glBlendFuncSeparate(convert(desc.colorBlendSourceMode),
                                        convert(desc.colorBlendDestinationMode),
                                        convert(desc.alphaBlendSourceMode),
                                        convert(desc.alphaBlendDestinationMode));
                    glBlendEquationSeparate(convert(desc.colorBlendEquation),
                                            convert(desc.alphaBlendEquation));

                    if (desc.enableBlending) {
                        glEnable(GL_BLEND);
                    } else {
                        glDisable(GL_BLEND);
                    }

                    checkGLError();

                    stats.binds++;

                    break;
                }
                case Command::BIND_SHADER_RESOURCES: {
                    if (mRenderPipeline == nullptr && mComputePipeline == nullptr) {
                        throw std::runtime_error("Pipeline must be bound in order to bind shader resources");
                    }

                    auto bindings = std::get<ShaderResourceBind>(c.data);

                    unbindShaderData();

                    mShaderBindings = bindings.resources;

                    // Bind textures and uniform buffers
                    for (int bindingPoint = 0; bindingPoint < bindings.resources.size(); bindingPoint++) {
                        auto &b = bindings.resources.at(bindingPoint);
                        auto bind = mRenderPipeline ? mRenderPipeline->getDescription().bindings.at(bindingPoint)
                                                    : mComputePipeline->getDescription().bindings.at(bindingPoint);
                        switch (bind) {
                            case BIND_TEXTURE_BUFFER: {
                                auto texture = dynamic_cast<OGLTextureBuffer *>(&std::get<std::reference_wrapper<TextureBuffer>>(
                                        b.data).get());
                                glActiveTexture(getTextureSlot(bindingPoint));
                                glBindTexture(convert(texture->getDescription().textureType), texture->handle);
                                break;
                            }
                            case BIND_TEXTURE_ARRAY_BUFFER: {
                                auto textureArray = dynamic_cast<OGLTextureArrayBuffer *>(&std::get<std::reference_wrapper<TextureArrayBuffer>>(
                                        b.data).get());
                                glActiveTexture(getTextureSlot(bindingPoint));
                                auto target = textureArray->desc.textureDesc.textureType == TEXTURE_CUBE_MAP
                                              ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_2D_ARRAY;
                                glBindTexture(target, textureArray->handle);
                                break;
                            }
                            case BIND_SHADER_UNIFORM_BUFFER: {
                                auto shaderBuffer = dynamic_cast<OGLShaderUniformBuffer *>(&std::get<std::reference_wrapper<ShaderUniformBuffer>>(
                                        b.data).get());
                                glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, shaderBuffer->ubo);
                                break;
                            }
                            case BIND_SHADER_STORAGE_BUFFER: {
                                auto buf = dynamic_cast<OGLShaderStorageBuffer *>(
                                        &std::get<std::reference_wrapper<ShaderStorageBuffer>>(b.data).get());
                                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buf->ssbo);
                                break;
                            }
                            case BIND_IMAGE_BUFFER: {
                                auto texture = dynamic_cast<OGLTextureBuffer *>(&std::get<std::reference_wrapper<TextureBuffer>>(
                                        b.data).get());
                                GLenum mode;
                                for (auto &v: b.stages) {
                                    switch (v.second) {
                                        case ShaderResource::READ:
                                            if (mode == ShaderResource::WRITE) {
                                                mode = GL_READ_WRITE;
                                            } else {
                                                mode = GL_READ_ONLY;
                                            }
                                            break;
                                        case ShaderResource::WRITE:
                                            if (mode == ShaderResource::READ) {
                                                mode = GL_READ_WRITE;
                                            } else {
                                                mode = GL_WRITE_ONLY;
                                            }
                                            break;
                                        case ShaderResource::READ_WRITE:
                                            mode = GL_WRITE_ONLY;
                                            break;
                                    }
                                }
                                glBindImageTexture(bindingPoint,
                                                   texture->handle,
                                                   0,
                                                   false,
                                                   0,
                                                   mode,
                                                   texture->texInternalFormat);
                                break;
                            }
                            case BIND_IMAGE_ARRAY_BUFFER: {
                                auto texture = dynamic_cast<OGLTextureArrayBuffer *>(&std::get<std::reference_wrapper<TextureArrayBuffer>>(
                                        b.data).get());
                                GLenum mode;
                                for (auto &v: b.stages) {
                                    switch (v.second) {
                                        case ShaderResource::READ:
                                            if (mode == ShaderResource::WRITE) {
                                                mode = GL_READ_WRITE;
                                            } else {
                                                mode = GL_READ_ONLY;
                                            }
                                            break;
                                        case ShaderResource::WRITE:
                                            if (mode == ShaderResource::READ) {
                                                mode = GL_READ_WRITE;
                                            } else {
                                                mode = GL_WRITE_ONLY;
                                            }
                                            break;
                                        case ShaderResource::READ_WRITE:
                                            mode = GL_WRITE_ONLY;
                                            break;
                                    }
                                }
                                glBindImageTexture(bindingPoint,
                                                   texture->handle,
                                                   0,
                                                   false,
                                                   0,
                                                   mode,
                                                   texture->internalFormat);
                                break;
                            }
                        }
                        stats.binds++;
                    }
                    checkGLError();
                    break;
                }
                case Command::COPY_TEXTURE_ARRAY: {
                    auto data = std::get<TextureArrayBufferCopy>(c.data);

                    auto &target = dynamic_cast<OGLTextureArrayBuffer &>(*data.target);
                    auto &src = dynamic_cast<OGLTextureArrayBuffer &>(*data.source);
                    if (src.desc.textureDesc != target.desc.textureDesc) {
                        throw std::runtime_error("Cannot copy texture array buffer");
                    }

                    auto count = src.desc.textureCount > target.desc.textureCount ? target.desc.textureCount
                                                                                  : src.desc.textureCount;

                    if (count > 0) {
                        glCopyImageSubData(src.handle,
                                           GL_TEXTURE_2D_ARRAY,
                                           0,
                                           0,
                                           0,
                                           0,
                                           target.handle,
                                           GL_TEXTURE_2D_ARRAY,
                                           0,
                                           0,
                                           0,
                                           0,
                                           src.desc.textureDesc.size.x,
                                           src.desc.textureDesc.size.y,
                                           static_cast<GLsizei>(count));
                    }

                    glBindTexture(GL_TEXTURE_2D_ARRAY, target.handle);

                    auto texWrap = convert(target.desc.textureDesc.wrapping);

                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                    GL_TEXTURE_WRAP_S,
                                    texWrap);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                    GL_TEXTURE_WRAP_T,
                                    texWrap);

                    if (target.desc.textureDesc.generateMipmap) {
                        glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                        GL_TEXTURE_MIN_FILTER,
                                        convert(target.desc.textureDesc.mipmapFilter));
                        glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                        GL_TEXTURE_MAG_FILTER,
                                        convert(target.desc.textureDesc.filterMag));
                        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
                    } else {
                        glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                        GL_TEXTURE_MIN_FILTER,
                                        convert(target.desc.textureDesc.filterMin));
                        glTexParameteri(GL_TEXTURE_2D_ARRAY,
                                        GL_TEXTURE_MAG_FILTER,
                                        convert(target.desc.textureDesc.filterMag));
                    }

                    auto col = target.desc.textureDesc.borderColor.divide();
                    float borderColor[] = {col.x, col.y, col.z, col.w};
                    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

                    checkGLError();

                    break;
                }
                case Command::COPY_TEXTURE: {
                    auto data = std::get<TextureBufferCopy>(c.data);

                    auto &target = dynamic_cast<OGLTextureBuffer &>(*data.target);
                    auto &src = dynamic_cast<OGLTextureBuffer &>(*data.source);
                    glCopyImageSubData(src.handle,
                                       GL_TEXTURE_2D,
                                       0,
                                       0,
                                       0,
                                       0,
                                       target.handle,
                                       GL_TEXTURE_2D,
                                       0,
                                       0,
                                       0,
                                       0,
                                       src.desc.size.x,
                                       src.desc.size.y,
                                       1);

                    glBindTexture(target.textureType, target.handle);

                    glTexParameteri(target.textureType, GL_TEXTURE_WRAP_S, convert(target.desc.wrapping));
                    glTexParameteri(target.textureType, GL_TEXTURE_WRAP_T, convert(target.desc.wrapping));

                    if (target.desc.generateMipmap) {
                        glTexParameteri(target.textureType,
                                        GL_TEXTURE_MIN_FILTER,
                                        convert(target.desc.mipmapFilter));
                        glTexParameteri(target.textureType,
                                        GL_TEXTURE_MAG_FILTER,
                                        convert(target.desc.filterMag));
                        glGenerateMipmap(target.textureType);
                    } else {
                        glTexParameteri(target.textureType,
                                        GL_TEXTURE_MIN_FILTER,
                                        convert(target.desc.filterMin));
                        glTexParameteri(target.textureType,
                                        GL_TEXTURE_MAG_FILTER,
                                        convert(target.desc.filterMag));
                    }

                    auto col = target.desc.borderColor.divide();
                    float borderColor[] = {col.x, col.y, col.z, col.w};
                    glTexParameterfv(target.textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

                    glBindTexture(target.textureType, 0);

                    checkGLError();

                    break;
                }
                case Command::BIND_VERTEX_ARRAY_OBJECT: {
                    ensureRunningPass();
                    auto data = std::get<VertexArrayObjectBind>(c.data);
                    mVertexObject = dynamic_cast<OGLVertexArrayObject *>(data.target);
                    glBindVertexArray(mVertexObject->VAO);
                    stats.binds++;
                    checkGLError();
                    break;
                }
                case Command::COPY_VERTEX_BUFFER: {
                    auto data = std::get<VertexBufferCopy>(c.data);

                    auto &target = dynamic_cast<OGLVertexBuffer &>(*data.target);
                    auto &buf = dynamic_cast<OGLVertexBuffer &>(*data.source);
                    if (data.readOffset >= buf.desc.size
                        || data.readOffset + data.count > buf.desc.size
                        || data.writeOffset >= target.desc.size
                        || data.writeOffset + data.count > target.desc.size) {
                        throw std::runtime_error("Invalid copy range");
                    }
                    glBindBuffer(GL_COPY_READ_BUFFER, buf.VBO);
                    glBindBuffer(GL_COPY_WRITE_BUFFER, target.VBO);
                    glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                        GL_COPY_WRITE_BUFFER,
                                        static_cast<GLintptr>(data.readOffset),
                                        static_cast<GLintptr>(data.writeOffset),
                                        static_cast<GLsizeiptr>(data.count));
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
                    checkGLError();
                    break;
                }
                case Command::COPY_SHADER_STORAGE_BUFFER: {
                    auto data = std::get<ShaderStorageBufferCopy>(c.data);
                    auto &target = dynamic_cast<OGLShaderStorageBuffer &>(*data.target);
                    auto &source = dynamic_cast<OGLShaderStorageBuffer &>(*data.source);
                    if (data.readOffset >= source.desc.size
                        || data.readOffset + data.count >= source.desc.size
                        || data.writeOffset >= target.desc.size
                        || data.writeOffset + data.count >= target.desc.size) {
                        throw std::runtime_error("Invalid copy range");
                    }
                    glBindBuffer(GL_COPY_WRITE_BUFFER, target.ssbo);
                    glBindBuffer(GL_COPY_READ_BUFFER, source.ssbo);
                    glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                        GL_COPY_WRITE_BUFFER,
                                        static_cast<GLintptr>(data.readOffset),
                                        static_cast<GLintptr>(data.writeOffset),
                                        static_cast<GLsizeiptr>(data.count));
                    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    checkGLError();
                    break;
                }
                case Command::COPY_SHADER_UNIFORM_BUFFER: {
                    auto data = std::get<ShaderUniformBufferCopy>(c.data);
                    auto &target = dynamic_cast<OGLShaderUniformBuffer &>(*data.target);
                    auto &source = dynamic_cast<OGLShaderUniformBuffer &>(*data.source);
                    if (data.readOffset >= source.desc.size
                        || data.readOffset + data.count >= source.desc.size
                        || data.writeOffset >= target.desc.size
                        || data.writeOffset + data.count >= target.desc.size) {
                        throw std::runtime_error("Invalid copy range");
                    }
                    glBindBuffer(GL_COPY_WRITE_BUFFER, target.ubo);
                    glBindBuffer(GL_COPY_READ_BUFFER, source.ubo);
                    glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                        GL_COPY_WRITE_BUFFER,
                                        static_cast<GLintptr>(data.readOffset),
                                        static_cast<GLintptr>(data.writeOffset),
                                        static_cast<GLsizeiptr>(data.count));
                    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
                    glBindBuffer(GL_COPY_READ_BUFFER, 0);
                    checkGLError();
                    break;
                }
                case Command::COMPUTE_BIND_PIPELINE: {
                    auto data = std::get<ComputePipelineBind>(c.data);
                    auto &pip = dynamic_cast<OGLComputePipeline &>(*data.pipeline);
                    mComputePipeline = &pip;

                    auto &desc = data.pipeline->getDescription();

                    // Bind shader program
                    if (pip.programHandle) {
                        glUseProgram(pip.programHandle);
                    }

                    checkGLError();
                    break;
                }
                case Command::COMPUTE_EXECUTE: {
                    auto data = std::get<ComputePipelineExecute>(c.data);
                    glDispatchCompute(data.num_groups.x, data.num_groups.y, data.num_groups.z);
                    checkGLError();
                    break;
                }
            }
        }
    };


}
#endif //XENGINE_OGLCOMMANDQUEUE_HPP
