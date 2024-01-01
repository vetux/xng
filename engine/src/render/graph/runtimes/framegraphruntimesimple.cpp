/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <unordered_set>

#include "xng/render/graph/runtimes/framegraphruntimesimple.hpp"

namespace xng {
    FrameGraphRuntimeSimple::FrameGraphRuntimeSimple(RenderTarget &backBuffer,
                                                     RenderDevice &device,
                                                     ShaderCompiler &shaderCompiler,
                                                     ShaderDecompiler &shaderDecompiler)
            : backBuffer(backBuffer),
              device(device),
              shaderCompiler(shaderCompiler),
              shaderDecompiler(shaderDecompiler) {
        for (auto type = FrameGraphCommand::Type::CREATE_RENDER_PIPELINE;
             type <= FrameGraphCommand::Type::CREATE_SHADER_STORAGE_BUFFER;
             type = (FrameGraphCommand::Type) ((int) type + 1)) {
            commandJumpTable[type] = [this](const FrameGraphCommand &cmd) { cmdCreate(cmd); };
        }

        commandJumpTable[FrameGraphCommand::UPLOAD] = [this](const FrameGraphCommand &cmd) { cmdUpload(cmd); };
        commandJumpTable[FrameGraphCommand::COPY] = [this](const FrameGraphCommand &cmd) { cmdCopy(cmd); };

        commandJumpTable[FrameGraphCommand::BLIT_COLOR] = [this](const FrameGraphCommand &cmd) { cmdBlit(cmd); };
        commandJumpTable[FrameGraphCommand::BLIT_DEPTH] = [this](const FrameGraphCommand &cmd) { cmdBlit(cmd); };
        commandJumpTable[FrameGraphCommand::BLIT_STENCIL] = [this](const FrameGraphCommand &cmd) { cmdBlit(cmd); };

        commandJumpTable[FrameGraphCommand::BEGIN_PASS] = [this](const FrameGraphCommand &cmd) { cmdBeginPass(cmd); };
        commandJumpTable[FrameGraphCommand::FINISH_PASS] = [this](const FrameGraphCommand &cmd) { cmdFinishPass(cmd); };

        commandJumpTable[FrameGraphCommand::CLEAR_COLOR] = [this](const FrameGraphCommand &cmd) {
            cmdClear(cmd);
        };
        commandJumpTable[FrameGraphCommand::CLEAR_DEPTH] = [this](const FrameGraphCommand &cmd) {
            cmdClear(cmd);
        };

        commandJumpTable[FrameGraphCommand::BIND_PIPELINE] = [this](const FrameGraphCommand &cmd) {
            cmdBindPipeline(cmd);
        };

        commandJumpTable[FrameGraphCommand::BIND_VERTEX_BUFFERS] = [this](
                const FrameGraphCommand &cmd) { cmdBindVertexBuffers(cmd); };

        commandJumpTable[FrameGraphCommand::BIND_SHADER_RESOURCES] = [this](
                const FrameGraphCommand &cmd) { cmdBindShaderResources(cmd); };
        commandJumpTable[FrameGraphCommand::SET_VIEWPORT] = [this](const FrameGraphCommand &cmd) {
            cmdSetViewport(cmd);
        };

        for (auto type = FrameGraphCommand::Type::DRAW_ARRAY;
             type <= FrameGraphCommand::Type::DRAW_MULTI_INDEXED_BASE_VERTEX;
             type = (FrameGraphCommand::Type) ((int) type + 1)) {
            commandJumpTable[type] = [this](const FrameGraphCommand &cmd) { cmdDraw(cmd); };
        }

        commandBuffer = device.createCommandBuffer();
    }

    void FrameGraphRuntimeSimple::execute(const FrameGraph &v) {
        graph = v;

        for (auto &c: graph.contexts) {
            auto name = c.pass.name();
            for (auto &cmd: c.commands) {
                commandJumpTable.at(cmd.type)(cmd);
            }
            for (auto &r: c.persists) {
                persist(r);
            }
        }

        collectGarbage();
    }

    const RenderTargetDesc &FrameGraphRuntimeSimple::getBackBufferDesc() {
        return backBuffer.getDescription();
    }

    const RenderDeviceInfo &FrameGraphRuntimeSimple::getRenderDeviceInfo() {
        return device.getInfo();
    }

    RenderObject &FrameGraphRuntimeSimple::getObject(FrameGraphResource resource) {
        auto it = persistentObjects.find(resource);
        if (it != persistentObjects.end())
            return *it->second;
        else
            return *objects.at(resource);
    }

    void FrameGraphRuntimeSimple::flushBufferCommands() {
        commandBuffer->begin();
        commandBuffer->add(pendingBufferCommands);
        commandBuffer->end();

        device.getRenderCommandQueues().at(0).get().submit(*commandBuffer);

        pendingBufferCommands.clear();
        dirtyBuffers.clear();
    }

    void FrameGraphRuntimeSimple::flushRenderCommands() {
        commandBuffer->begin();
        commandBuffer->add(pendingRenderCommands);
        commandBuffer->end();

        device.getRenderCommandQueues().at(0).get().submit(*commandBuffer);

        pendingRenderCommands.clear();
    }

    void FrameGraphRuntimeSimple::cmdCreate(const FrameGraphCommand &cmd) {
        switch (cmd.type) {
            case FrameGraphCommand::CREATE_RENDER_PIPELINE:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_RENDER_PIPELINE,
                         std::get<RenderPipelineDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_TEXTURE:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_TEXTURE_BUFFER,
                         std::get<TextureBufferDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_TEXTURE_ARRAY:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER,
                         std::get<TextureArrayBufferDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_VERTEX_BUFFER:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_VERTEX_BUFFER,
                         std::get<VertexBufferDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_INDEX_BUFFER:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_INDEX_BUFFER,
                         std::get<IndexBufferDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_SHADER_UNIFORM_BUFFER:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER,
                         std::get<ShaderUniformBufferDesc>(cmd.data));
                break;
            case FrameGraphCommand::CREATE_SHADER_STORAGE_BUFFER:
                allocate(cmd.resources.at(0),
                         RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER,
                         std::get<ShaderStorageBufferDesc>(cmd.data));
                break;
            default:
                assert(false);
                break;
        }
    }

    void FrameGraphRuntimeSimple::cmdUpload(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::UploadData>(cmd.data);
        auto buffer = data.dataSource();

        auto obj = &getObject(cmd.resources.at(0));

        switch (obj->getType()) {
            case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                auto &vb = dynamic_cast<VertexBuffer &>(*obj);
                vb.upload(data.offset, buffer.data.data(), buffer.data.size());
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                auto &tb = dynamic_cast<TextureBuffer &>(*obj);
                if (tb.getDescription().textureType == TEXTURE_CUBE_MAP)
                    tb.upload(data.face, data.colorFormat, buffer.data.data(), buffer.data.size());
                else
                    tb.upload(data.colorFormat, buffer.data.data(), buffer.data.size());
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                auto &tb = dynamic_cast<TextureArrayBuffer &>(*obj);
                tb.upload(data.index, data.colorFormat, buffer.data.data(), buffer.data.size());
                break;
            }
            case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                auto &sb = dynamic_cast<ShaderUniformBuffer &>(*obj);
                sb.upload(data.offset, buffer.data.data(), buffer.data.size());
                break;
            }
            case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                auto &sb = dynamic_cast<ShaderStorageBuffer &>(*obj);
                sb.upload(data.offset, buffer.data.data(), buffer.data.size());
                break;
            }
            case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                auto &ib = dynamic_cast<IndexBuffer &>(*obj);
                ib.upload(data.offset, buffer.data.data(), buffer.data.size());
                break;
            }
            default:
                assert(false);
                break;
        }
    }

    void FrameGraphRuntimeSimple::cmdCopy(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::CopyData>(cmd.data);

        auto objA = &getObject(cmd.resources.at(0));
        auto objB = &getObject(cmd.resources.at(1));

        assert(objA->getType() == objB->getType());

        switch (objA->getType()) {
            case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                auto &vbA = dynamic_cast<VertexBuffer &>(*objA);
                auto &vbB = dynamic_cast<VertexBuffer &>(*objB);
                pendingBufferCommands.emplace_back(vbB.copy(vbA, data.readOffset, data.writeOffset, data.count));
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                auto &tbA = dynamic_cast<TextureBuffer &>(*objA);
                auto &tbB = dynamic_cast<TextureBuffer &>(*objB);
                pendingBufferCommands.emplace_back(tbB.copy(tbA));
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                auto &tbA = dynamic_cast<TextureArrayBuffer &>(*objA);
                auto &tbB = dynamic_cast<TextureArrayBuffer &>(*objB);
                pendingBufferCommands.emplace_back(tbB.copy(tbA));
                break;
            }
            case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                auto &sbA = dynamic_cast<ShaderUniformBuffer &>(*objA);
                auto &sbB = dynamic_cast<ShaderUniformBuffer &>(*objB);
                pendingBufferCommands.emplace_back(sbB.copy(sbA, data.readOffset, data.writeOffset, data.count));
                break;
            }
            case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                auto &sbA = dynamic_cast<ShaderStorageBuffer &>(*objA);
                auto &sbB = dynamic_cast<ShaderStorageBuffer &>(*objB);
                pendingBufferCommands.emplace_back(sbB.copy(sbA, data.readOffset, data.writeOffset, data.count));
                break;
            }
            case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                auto &ibA = dynamic_cast<IndexBuffer &>(*objA);
                auto &ibB = dynamic_cast<IndexBuffer &>(*objB);
                pendingBufferCommands.emplace_back(ibB.copy(ibA, data.readOffset, data.writeOffset, data.count));
                break;
            }
            default:
                assert(false);
                break;
        }

        dirtyBuffers.insert(cmd.resources.at(0));
        dirtyBuffers.insert(cmd.resources.at(1));
    }

    void FrameGraphRuntimeSimple::cmdBlit(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::BlitData>(cmd.data);

        auto objA = cmd.resources.at(0) == graph.backBuffer ? &backBuffer : &getObject(cmd.resources.at(0));
        auto objB = cmd.resources.at(1) == graph.backBuffer ? &backBuffer : &getObject(cmd.resources.at(1));

        switch (cmd.type) {
            case FrameGraphCommand::BLIT_COLOR:
                if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                    && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    throw std::runtime_error("Blit from backbuffer to backbuffer is not allowed.");
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 1;
                    desc.hasDepthStencilAttachment = false;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({RenderTargetAttachment::texture(tA)});

                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({RenderTargetAttachment::texture(tB)});

                    pendingBufferCommands.emplace_back(targetB.blitColor(targetA,
                                                                         data.sourceOffset,
                                                                         data.targetOffset,
                                                                         data.sourceRect,
                                                                         data.targetRect,
                                                                         data.filter,
                                                                         0,
                                                                         0));
                    dirtyBuffers.insert(cmd.resources.at(0));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<RenderTarget &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 1;
                    desc.hasDepthStencilAttachment = false;
                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({RenderTargetAttachment::texture(tB)});

                    pendingBufferCommands.emplace_back(targetB.blitColor(tA,
                                                                         data.sourceOffset,
                                                                         data.targetOffset,
                                                                         data.sourceRect,
                                                                         data.targetRect,
                                                                         data.filter,
                                                                         0,
                                                                         0));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<RenderTarget &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 1;
                    desc.hasDepthStencilAttachment = false;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({RenderTargetAttachment::texture(tA)});

                    pendingBufferCommands.emplace_back(tB.blitColor(targetA,
                                                                    data.sourceOffset,
                                                                    data.targetOffset,
                                                                    data.sourceRect,
                                                                    data.targetRect,
                                                                    data.filter,
                                                                    0,
                                                                    0));
                    dirtyBuffers.insert(cmd.resources.at(0));
                } else {
                    assert(false);
                }
                break;
            case FrameGraphCommand::BLIT_DEPTH:
                if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                    && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    throw std::runtime_error("Blit from backbuffer to backbuffer is not allowed.");
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({}, RenderTargetAttachment::texture(tA));

                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({}, RenderTargetAttachment::texture(tB));

                    pendingBufferCommands.emplace_back(targetB.blitDepth(targetA,
                                                                         data.sourceOffset,
                                                                         data.targetOffset,
                                                                         data.sourceRect,
                                                                         data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(0));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<RenderTarget &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({}, RenderTargetAttachment::texture(tB));

                    pendingBufferCommands.emplace_back(targetB.blitDepth(tA,
                                                                         data.sourceOffset,
                                                                         data.targetOffset,
                                                                         data.sourceRect,
                                                                         data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<RenderTarget &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({}, RenderTargetAttachment::texture(tA));

                    pendingBufferCommands.emplace_back(tB.blitDepth(targetA,
                                                                    data.sourceOffset,
                                                                    data.targetOffset,
                                                                    data.sourceRect,
                                                                    data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(0));
                } else {
                    assert(false);
                }
                break;
            case FrameGraphCommand::BLIT_STENCIL:
                if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                    && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    throw std::runtime_error("Blit from backbuffer to backbuffer is not allowed.");
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({}, RenderTargetAttachment::texture(tA));

                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({}, RenderTargetAttachment::texture(tB));

                    pendingBufferCommands.emplace_back(targetB.blitStencil(targetA,
                                                                           data.sourceOffset,
                                                                           data.targetOffset,
                                                                           data.sourceRect,
                                                                           data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(0));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET
                           && objB->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER) {
                    auto &tA = dynamic_cast<RenderTarget &>(*objA);
                    auto &tB = dynamic_cast<TextureBuffer &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tB.getDescription().size;

                    auto &targetB = createRenderTarget(desc);
                    targetB.setAttachments({}, RenderTargetAttachment::texture(tB));

                    pendingBufferCommands.emplace_back(targetB.blitStencil(tA,
                                                                           data.sourceOffset,
                                                                           data.targetOffset,
                                                                           data.sourceRect,
                                                                           data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(1));
                } else if (objA->getType() == RenderObject::RENDER_OBJECT_TEXTURE_BUFFER
                           && objB->getType() == RenderObject::RENDER_OBJECT_RENDER_TARGET) {
                    auto &tA = dynamic_cast<TextureBuffer &>(*objA);
                    auto &tB = dynamic_cast<RenderTarget &>(*objB);

                    RenderTargetDesc desc;
                    desc.numberOfColorAttachments = 0;
                    desc.hasDepthStencilAttachment = true;
                    desc.size = tA.getDescription().size;

                    auto &targetA = createRenderTarget(desc);
                    targetA.setAttachments({}, RenderTargetAttachment::texture(tA));

                    pendingBufferCommands.emplace_back(tB.blitStencil(targetA,
                                                                      data.sourceOffset,
                                                                      data.targetOffset,
                                                                      data.sourceRect,
                                                                      data.targetRect));
                    dirtyBuffers.insert(cmd.resources.at(0));
                } else {
                    assert(false);
                }
                break;
            default:
                assert(false);
                break;
        }
    }

    void FrameGraphRuntimeSimple::cmdBeginPass(const FrameGraphCommand &cmd) {
        if (cmd.resources.empty()) {
            auto &data = std::get<FrameGraphCommand::BeginPassData>(cmd.data);

            if (dirtyBuffers.contains(data.depthAttachment.resource)) {
                flushBufferCommands();
            } else {
                for (auto &att: data.colorAttachments) {
                    if (dirtyBuffers.contains(att.resource)) {
                        flushBufferCommands();
                        break;
                    }
                }
            }

            Vec2i size{};
            bool textureArray = false;

            std::vector<RenderTargetAttachment> colorAttachments;

            if (!data.colorAttachments.empty()) {
                for (auto &att: data.colorAttachments) {
                    RenderTargetAttachment colorAttachment;

                    auto obj = &getObject(att.resource);
                    switch (obj->getType()) {
                        case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER:
                            colorAttachment.textureBuffer = dynamic_cast<TextureBuffer *>(obj);
                            size = colorAttachment.textureBuffer->getDescription().size;
                            break;
                        case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER:
                            textureArray = true;
                            colorAttachment.textureArrayBuffer = dynamic_cast<TextureArrayBuffer *>(obj);
                            size = colorAttachment.textureArrayBuffer->getDescription().textureDesc.size;
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    colorAttachment.type = att.type;
                    colorAttachment.index = att.index;
                    colorAttachment.face = att.face;
                    colorAttachment.mipMapLevel = att.mipMapLevel;

                    colorAttachments.emplace_back(colorAttachment);
                }
            }
            RenderTargetAttachment depthAttachment;
            if (data.depthAttachment.resource.assigned) {
                auto obj = &getObject(data.depthAttachment.resource);
                switch (obj->getType()) {
                    case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER:
                        depthAttachment.textureBuffer = dynamic_cast<TextureBuffer *>(obj);
                        size = depthAttachment.textureBuffer->getDescription().size;
                        break;
                    case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER:
                        textureArray = true;
                        depthAttachment.textureArrayBuffer = dynamic_cast<TextureArrayBuffer *>(obj);
                        size = depthAttachment.textureArrayBuffer->getDescription().textureDesc.size;
                        break;
                    default:
                        assert(false);
                        break;
                }
                depthAttachment.type = data.depthAttachment.type;
                depthAttachment.index = data.depthAttachment.index;
                depthAttachment.face = data.depthAttachment.face;
                depthAttachment.mipMapLevel = data.depthAttachment.mipMapLevel;
            } else {
                if (textureArray) {
                    TextureArrayBufferDesc desc;
                    desc.textureCount = 1;
                    desc.textureDesc.size = size;
                    desc.textureDesc.format = DEPTH_STENCIL;
                    auto &tex = createTextureArrayBuffer(desc);
                    depthAttachment = RenderTargetAttachment::textureArrayLayered(tex);
                } else {
                    TextureBufferDesc desc;
                    desc.size = size;
                    desc.format = DEPTH_STENCIL;
                    auto &tex = createTextureBuffer(desc);
                    depthAttachment = RenderTargetAttachment::texture(tex);
                }
            }

            if (data.colorAttachments.empty()) {
                if (textureArray) {
                    TextureArrayBufferDesc desc;
                    desc.textureCount = 1;
                    desc.textureDesc.size = size;
                    desc.textureDesc.format = RGBA;
                    auto &tex = createTextureArrayBuffer(desc);
                    colorAttachments.emplace_back(RenderTargetAttachment::textureArrayLayered(tex));
                } else {
                    TextureBufferDesc desc;
                    desc.size = size;
                    desc.format = RGBA;
                    auto &tex = createTextureBuffer(desc);
                    colorAttachments.emplace_back(RenderTargetAttachment::texture(tex));
                }
            }

            RenderTargetDesc desc;
            desc.size = size;
            desc.numberOfColorAttachments = static_cast<int>(colorAttachments.size());
            desc.hasDepthStencilAttachment = true;

            auto &target = createRenderTarget(desc);

            target.setAttachments(colorAttachments, depthAttachment);

            RenderPassDesc passDesc;
            passDesc.numberOfColorAttachments = desc.numberOfColorAttachments;
            passDesc.hasDepthStencilAttachment = desc.hasDepthStencilAttachment;

            pass = &getRenderPass(passDesc);

            pendingRenderCommands.emplace_back(pass->begin(target));
        } else {
            assert(cmd.resources.at(0) == graph.backBuffer);

            RenderPassDesc desc;
            desc.numberOfColorAttachments = backBuffer.getDescription().numberOfColorAttachments;
            desc.hasDepthStencilAttachment = backBuffer.getDescription().hasDepthStencilAttachment;
            pass = &getRenderPass(desc);

            pendingRenderCommands.emplace_back(pass->begin(backBuffer));
        }
    }

    void FrameGraphRuntimeSimple::cmdFinishPass(const FrameGraphCommand &cmd) {
        pendingRenderCommands.emplace_back(pass->end());
        flushRenderCommands();
    }

    void FrameGraphRuntimeSimple::cmdClear(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::ClearData>(cmd.data);
        if (cmd.type == FrameGraphCommand::CLEAR_COLOR)
            pendingRenderCommands.emplace_back(pass->clearColorAttachments(data.color));
        else
            pendingRenderCommands.emplace_back(pass->clearDepthAttachment(data.depth));
    }

    void FrameGraphRuntimeSimple::cmdBindPipeline(const FrameGraphCommand &cmd) {
        pipeline = dynamic_cast<RenderPipeline *>(&getObject(cmd.resources.at(0)));
        pendingRenderCommands.emplace_back(pipeline->bind());
    }

    void FrameGraphRuntimeSimple::cmdBindVertexBuffers(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::BindVertexData>(cmd.data);

        FrameGraphResource vertexBuffer = cmd.resources.at(0);
        FrameGraphResource indexBuffer = cmd.resources.at(1);
        FrameGraphResource instanceBuffer = cmd.resources.at(2);

        if (dirtyBuffers.contains(vertexBuffer)
            || (indexBuffer.assigned && dirtyBuffers.contains(indexBuffer))
            || (instanceBuffer.assigned && dirtyBuffers.contains(instanceBuffer)))
            flushBufferCommands();

        VertexArrayObjectDesc desc;
        desc.vertexLayout = data.vertexLayout;
        desc.instanceArrayLayout = data.instanceArrayLayout;

        auto &vao = createVertexArrayObject(desc);

        if (indexBuffer.assigned && instanceBuffer.assigned)
            vao.setBuffers(*reinterpret_cast<VertexBuffer *>(&getObject(vertexBuffer)),
                           *reinterpret_cast<IndexBuffer *>(&getObject(indexBuffer)),
                           *reinterpret_cast<VertexBuffer *>(&getObject(instanceBuffer)));
        else if (indexBuffer.assigned)
            vao.setBuffers(*reinterpret_cast<VertexBuffer *>(&getObject(vertexBuffer)),
                           *reinterpret_cast<IndexBuffer *>(&getObject(indexBuffer)));
        else
            vao.setBuffers(*reinterpret_cast<VertexBuffer *>(&getObject(vertexBuffer)));

        pendingRenderCommands.emplace_back(vao.bind());
    }

    void FrameGraphRuntimeSimple::cmdBindShaderResources(const FrameGraphCommand &cmd) {
        auto &data = std::get<std::vector<FrameGraphCommand::ShaderData>>(cmd.data);
        std::vector<ShaderResource> resources;
        for (auto &r: data) {
            if (dirtyBuffers.contains(r.resource))
                flushBufferCommands();
            auto obj = &getObject(r.resource);
            switch (obj->getType()) {
                case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER:
                    resources.emplace_back(ShaderResource{*dynamic_cast<TextureBuffer *>(obj), r.accessModes});
                    break;
                case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER:
                    resources.emplace_back(ShaderResource{*dynamic_cast<TextureArrayBuffer *>(obj), r.accessModes});
                    break;
                case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER:
                    resources.emplace_back(ShaderResource{*dynamic_cast<ShaderUniformBuffer *>(obj), r.accessModes});
                    break;
                case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER:
                    resources.emplace_back(ShaderResource{*dynamic_cast<ShaderStorageBuffer *>(obj), r.accessModes});
                    break;
                default:
                    assert(false);
                    break;
            }
        }
        pendingRenderCommands.emplace_back(xng::RenderPipeline::bindShaderResources(resources));
    }

    void FrameGraphRuntimeSimple::cmdSetViewport(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::ViewportData>(cmd.data);
        pendingRenderCommands.emplace_back(pass->setViewport(data.viewportOffset, data.viewportSize));
    }

    void FrameGraphRuntimeSimple::cmdDraw(const FrameGraphCommand &cmd) {
        auto &data = std::get<FrameGraphCommand::DrawCallData>(cmd.data);
        switch (cmd.type) {
            case FrameGraphCommand::DRAW_ARRAY:
                pendingRenderCommands.emplace_back(pass->drawArray(data.drawCalls.at(0)));
                break;
            case FrameGraphCommand::DRAW_INDEXED:
                pendingRenderCommands.emplace_back(pass->drawIndexed(data.drawCalls.at(0)));
                break;
            case FrameGraphCommand::DRAW_INSTANCED_ARRAY:
                pendingRenderCommands.emplace_back(pass->instancedDrawArray(data.drawCalls.at(0),
                                                                            data.numberOfInstances));
                break;
            case FrameGraphCommand::DRAW_INSTANCED_INDEXED:
                pendingRenderCommands.emplace_back(pass->instancedDrawIndexed(data.drawCalls.at(0),
                                                                              data.numberOfInstances));
                break;
            case FrameGraphCommand::DRAW_MULTI_ARRAY:
                pendingRenderCommands.emplace_back(pass->multiDrawArray(data.drawCalls));
                break;
            case FrameGraphCommand::DRAW_MULTI_INDEXED:
                pendingRenderCommands.emplace_back(pass->multiDrawIndexed(data.drawCalls));
                break;
            case FrameGraphCommand::DRAW_INDEXED_BASE_VERTEX:
                pendingRenderCommands.emplace_back(pass->drawIndexed(data.drawCalls.at(0), data.baseVertices.at(0)));
                break;
            case FrameGraphCommand::DRAW_INSTANCED_INDEXED_BASE_VERTEX:
                pendingRenderCommands.emplace_back(pass->instancedDrawIndexed(data.drawCalls.at(0),
                                                                              data.numberOfInstances,
                                                                              data.baseVertices.at(0)));
                break;
            case FrameGraphCommand::DRAW_MULTI_INDEXED_BASE_VERTEX:
                pendingRenderCommands.emplace_back(pass->multiDrawIndexed(data.drawCalls, data.baseVertices));
                break;
            default:
                assert(false);
                break;
        }
    }

    RenderObject &FrameGraphRuntimeSimple::allocate(const FrameGraphResource &res,
                                                    RenderObject::Type type,
                                                    std::variant<RenderTargetDesc, RenderPipelineDesc, TextureBufferDesc, TextureArrayBufferDesc, VertexBufferDesc, IndexBufferDesc, VertexArrayObjectDesc, ShaderUniformBufferDesc, ShaderStorageBufferDesc, RenderPassDesc> data) {
        if (objects.find(res) != objects.end() || persistentObjects.find(res) != persistentObjects.end()) {
            throw std::runtime_error("Object already allocated for given resource handle");
        }

        switch (type) {
            case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                auto desc = std::get<VertexBufferDesc>(data);
                auto &tex = createVertexBuffer(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                auto desc = std::get<IndexBufferDesc>(data);
                auto &tex = createIndexBuffer(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_VERTEX_ARRAY_OBJECT: {
                auto desc = std::get<VertexArrayObjectDesc>(data);
                auto &tex = createVertexArrayObject(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                auto desc = std::get<TextureBufferDesc>(data);
                auto &tex = createTextureBuffer(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                auto desc = std::get<TextureArrayBufferDesc>(data);
                auto &tex = createTextureArrayBuffer(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                auto desc = std::get<ShaderUniformBufferDesc>(data);
                auto &buf = createShaderUniformBuffer(desc);
                objects[res] = &buf;
                return buf;
            }
            case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                auto desc = std::get<ShaderStorageBufferDesc>(data);
                auto &buf = createShaderStorageBuffer(desc);
                objects[res] = &buf;
                return buf;
            }
            case RenderObject::RENDER_OBJECT_RENDER_TARGET: {
                auto desc = std::get<RenderTargetDesc>(data);
                auto &target = createRenderTarget(desc);
                objects[res] = &target;
                return target;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PIPELINE: {
                auto desc = std::get<RenderPipelineDesc>(data);
                auto &pip = getPipeline(desc);
                objects[res] = &pip;
                return pip;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PASS: {
                auto desc = std::get<RenderPassDesc>(data);
                auto &tex = getRenderPass(desc);
                objects[res] = &tex;
                return tex;
            }
            case RenderObject::RENDER_OBJECT_COMMAND_BUFFER: {
                auto &buf = createCommandBuffer();
                objects[res] = &buf;
                return buf;
            }
            default:
                throw std::runtime_error("Invalid render object type");
        }
    }

    void FrameGraphRuntimeSimple::deallocate(const FrameGraphResource &resource) {
        objects.erase(resource);
    }

    void FrameGraphRuntimeSimple::persist(const FrameGraphResource &resource) {
        if (persistentObjects.find(resource) == persistentObjects.end()) {
            persistentObjects[resource] = persist(getObject(resource));
            objects.erase(resource);
        }
    }

    void FrameGraphRuntimeSimple::collectGarbage() {
        // Deallocate unused persistent objects
        std::set<FrameGraphResource> delObjects;
        for (auto &pair: persistentObjects) {
            bool deallocate = true;
            for (auto &cxt: graph.contexts) {
                if (cxt.persists.find(pair.first) != cxt.persists.end()) {
                    deallocate = false;
                    break;
                }
            }
            if (deallocate) {
                delObjects.insert(pair.first);
            }
        }

        for (auto &obj: delObjects) {
            persistentObjects.erase(obj);
        }

        objects.clear();

        // Resize object pools
        std::unordered_set<RenderPipelineDesc> pipelineDel;
        for (auto &pair: pipelines) {
            if (usedPipelines.find(pair.first) == usedPipelines.end()) {
                pipelineDel.insert(pair.first);
            }
        }
        for (auto &val: pipelineDel)
            pipelines.erase(val);

        std::unordered_set<RenderPassDesc> passDel;
        for (auto &pair: passes) {
            if (usedPasses.find(pair.first) == usedPasses.end()) {
                passDel.insert(pair.first);
            }
        }
        for (auto &val: passDel)
            passes.erase(val);

        std::unordered_set<VertexBufferDesc> vbDel;
        for (auto &pair: vertexBuffers) {
            if (usedVertexBuffers.find(pair.first) == usedVertexBuffers.end()) {
                vbDel.insert(pair.first);
            } else if (pair.second.size() > usedVertexBuffers.at(pair.first)) {
                pair.second.resize(usedVertexBuffers.at(pair.first));
            }
        }
        for (auto &val: vbDel)
            vertexBuffers.erase(val);

        std::unordered_set<IndexBufferDesc> ibDel;
        for (auto &pair: indexBuffers) {
            if (usedIndexBuffers.find(pair.first) == usedIndexBuffers.end()) {
                ibDel.insert(pair.first);
            } else if (pair.second.size() > usedIndexBuffers.at(pair.first)) {
                pair.second.resize(usedIndexBuffers.at(pair.first));
            }
        }
        for (auto &val: ibDel)
            indexBuffers.erase(val);

        std::unordered_set<VertexArrayObjectDesc> vaoDel;
        for (auto &pair: vertexArrayObjects) {
            if (usedVertexArrayObjects.find(pair.first) == usedVertexArrayObjects.end()) {
                vaoDel.insert(pair.first);
            } else if (pair.second.size() > usedVertexArrayObjects.at(pair.first)) {
                pair.second.resize(usedVertexArrayObjects.at(pair.first));
            }
        }
        for (auto &val: vaoDel)
            vertexArrayObjects.erase(val);

        std::unordered_set<TextureBufferDesc> texDel;
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                texDel.insert(pair.first);
            } else if (pair.second.size() > usedTextures.at(pair.first)) {
                pair.second.resize(usedTextures.at(pair.first));
            }
        }
        for (auto &val: texDel)
            textures.erase(val);

        std::unordered_set<TextureArrayBufferDesc> texArrayDel;
        for (auto &pair: textureArrays) {
            if (usedTextureArrays.find(pair.first) == usedTextureArrays.end()) {
                texArrayDel.insert(pair.first);
            } else if (pair.second.size() > usedTextureArrays.at(pair.first)) {
                pair.second.resize(usedTextureArrays.at(pair.first));
            }
        }
        for (auto &val: texArrayDel)
            textureArrays.erase(val);

        std::unordered_set<ShaderUniformBufferDesc> sbDel;
        for (auto &pair: shaderBuffers) {
            if (usedShaderBuffers.find(pair.first) == usedShaderBuffers.end()) {
                sbDel.insert(pair.first);
            } else if (pair.second.size() > usedShaderBuffers.at(pair.first)) {
                pair.second.resize(usedShaderBuffers.at(pair.first));
            }
        }
        for (auto &val: sbDel)
            shaderBuffers.erase(val);

        std::unordered_set<ShaderStorageBufferDesc> ssboDel;
        for (auto &pair: shaderStorageBuffers) {
            if (usedShaderStorageBuffers.find(pair.first) == usedShaderStorageBuffers.end()) {
                ssboDel.insert(pair.first);
            } else if (pair.second.size() > usedShaderStorageBuffers.at(pair.first)) {
                pair.second.resize(usedShaderStorageBuffers.at(pair.first));
            }
        }
        for (auto &val: ssboDel)
            shaderStorageBuffers.erase(val);

        std::unordered_set<RenderTargetDesc> targetDel;
        for (auto &pair: targets) {
            if (usedTargets.find(pair.first) == usedTargets.end()) {
                targetDel.insert(pair.first);
            } else if (pair.second.size() > usedTargets.at(pair.first)) {
                pair.second.resize(usedTargets.at(pair.first));
            }
        }
        for (auto &val: targetDel)
            targets.erase(val);

        if (commandBuffers.size() > usedCommandBuffers) {
            commandBuffers.resize(usedCommandBuffers);
        }

        usedPipelines.clear();
        usedPasses.clear();
        usedVertexBuffers.clear();
        usedIndexBuffers.clear();
        usedVertexArrayObjects.clear();
        usedTextures.clear();
        usedTextureArrays.clear();
        usedShaderBuffers.clear();
        usedShaderStorageBuffers.clear();
        usedTargets.clear();
        usedCommandBuffers = 0;
    }

    RenderPipeline &FrameGraphRuntimeSimple::getPipeline(const RenderPipelineDesc &desc) {
        if (usedPipelines[desc]++ == 0) {
            pipelines[desc] = device.createRenderPipeline(desc, shaderDecompiler);
        }
        return *pipelines.at(desc);
    }

    RenderPass &FrameGraphRuntimeSimple::getRenderPass(const RenderPassDesc &desc) {
        if (usedPasses[desc]++ == 0) {
            passes[desc] = device.createRenderPass(desc);
        }
        return *passes.at(desc);
    }

    VertexBuffer &FrameGraphRuntimeSimple::createVertexBuffer(const VertexBufferDesc &desc) {
        auto index = usedVertexBuffers[desc]++;
        if (vertexBuffers[desc].size() <= index) {
            vertexBuffers[desc].resize(usedVertexBuffers[desc]);
            vertexBuffers[desc].at(index) = device.createVertexBuffer(desc);
        }
        return *vertexBuffers[desc].at(index);
    }

    IndexBuffer &FrameGraphRuntimeSimple::createIndexBuffer(const IndexBufferDesc &desc) {
        auto index = usedIndexBuffers[desc]++;
        if (indexBuffers[desc].size() <= index) {
            indexBuffers[desc].resize(usedIndexBuffers[desc]);
            indexBuffers[desc].at(index) = device.createIndexBuffer(desc);
        }
        return *indexBuffers[desc].at(index);
    }

    VertexArrayObject &FrameGraphRuntimeSimple::createVertexArrayObject(const VertexArrayObjectDesc &desc) {
        auto index = usedVertexArrayObjects[desc]++;
        if (vertexArrayObjects[desc].size() <= index) {
            vertexArrayObjects[desc].resize(usedVertexArrayObjects[desc]);
            vertexArrayObjects[desc].at(index) = device.createVertexArrayObject(desc);
        }
        return *vertexArrayObjects[desc].at(index);
    }

    TextureBuffer &FrameGraphRuntimeSimple::createTextureBuffer(const TextureBufferDesc &desc) {
        auto index = usedTextures[desc]++;
        if (textures[desc].size() <= index) {
            textures[desc].resize(usedTextures[desc]);
            textures[desc].at(index) = device.createTextureBuffer(desc);
        }
        return *textures[desc].at(index);
    }

    TextureArrayBuffer &FrameGraphRuntimeSimple::createTextureArrayBuffer(const TextureArrayBufferDesc &desc) {
        auto index = usedTextureArrays[desc]++;
        if (textureArrays[desc].size() <= index) {
            textureArrays[desc].resize(usedTextureArrays[desc]);
            textureArrays[desc].at(index) = device.createTextureArrayBuffer(desc);
        }
        return *textureArrays[desc].at(index);
    }

    ShaderUniformBuffer &FrameGraphRuntimeSimple::createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) {
        auto index = usedShaderBuffers[desc]++;
        if (shaderBuffers[desc].size() <= index) {
            shaderBuffers[desc].resize(usedShaderBuffers[desc]);
            shaderBuffers[desc].at(index) = device.createShaderUniformBuffer(desc);
        }
        return *shaderBuffers[desc].at(index);
    }

    ShaderStorageBuffer &FrameGraphRuntimeSimple::createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) {
        auto index = usedShaderStorageBuffers[desc]++;
        if (shaderStorageBuffers[desc].size() <= index) {
            shaderStorageBuffers[desc].resize(usedShaderStorageBuffers[desc]);
            shaderStorageBuffers[desc].at(index) = device.createShaderStorageBuffer(desc);
        }
        return *shaderStorageBuffers[desc].at(index);
    }

    RenderTarget &FrameGraphRuntimeSimple::createRenderTarget(const RenderTargetDesc &desc) {
        auto index = usedTargets[desc]++;
        if (targets[desc].size() <= index) {
            targets[desc].resize(usedTargets[desc]);
            targets[desc].at(index) = device.createRenderTarget(desc);
        }
        return *targets[desc].at(index);
    }

    CommandBuffer &FrameGraphRuntimeSimple::createCommandBuffer() {
        auto index = usedCommandBuffers++;
        if (commandBuffers.size() <= index) {
            commandBuffers.resize(usedCommandBuffers);
            commandBuffers.at(index) = device.createCommandBuffer();
        }
        return *commandBuffers.at(index);
    }

    std::unique_ptr<RenderObject> FrameGraphRuntimeSimple::persist(RenderObject &obj) {
        switch (obj.getType()) {
            default:
                throw std::runtime_error("Invalid object type");
            case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                auto &buffer = dynamic_cast<VertexBuffer &>(obj);
                usedVertexBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < vertexBuffers[buffer.getDescription()].size(); i++) {
                    if (vertexBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(vertexBuffers.at(buffer.getDescription()).at(index).release());
                vertexBuffers.at(buffer.getDescription()).erase(
                        vertexBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                auto &buffer = dynamic_cast<IndexBuffer &>(obj);
                usedIndexBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < indexBuffers[buffer.getDescription()].size(); i++) {
                    if (indexBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(indexBuffers.at(buffer.getDescription()).at(index).release());
                indexBuffers.at(buffer.getDescription()).erase(
                        indexBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_VERTEX_ARRAY_OBJECT: {
                auto &buffer = dynamic_cast<VertexArrayObject &>(obj);
                usedVertexArrayObjects[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < vertexArrayObjects[buffer.getDescription()].size(); i++) {
                    if (vertexArrayObjects[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(
                        vertexArrayObjects.at(buffer.getDescription()).at(index).release());
                vertexArrayObjects.at(buffer.getDescription()).erase(
                        vertexArrayObjects.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                auto &buffer = dynamic_cast<TextureBuffer &>(obj);
                usedTextures[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < textures[buffer.getDescription()].size(); i++) {
                    if (textures[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(textures.at(buffer.getDescription()).at(index).release());
                textures.at(buffer.getDescription()).erase(textures.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                auto &buffer = dynamic_cast<TextureArrayBuffer &>(obj);
                usedTextureArrays[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < textureArrays[buffer.getDescription()].size(); i++) {
                    if (textureArrays[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(textureArrays.at(buffer.getDescription()).at(index).release());
                textureArrays.at(buffer.getDescription()).erase(
                        textureArrays.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                auto &buffer = dynamic_cast<ShaderUniformBuffer &>(obj);
                usedShaderBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < shaderBuffers[buffer.getDescription()].size(); i++) {
                    if (shaderBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(shaderBuffers.at(buffer.getDescription()).at(index).release());
                shaderBuffers.at(buffer.getDescription()).erase(
                        shaderBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                auto &buffer = dynamic_cast<ShaderStorageBuffer &>(obj);
                usedShaderStorageBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < shaderStorageBuffers[buffer.getDescription()].size(); i++) {
                    if (shaderStorageBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(
                        shaderStorageBuffers.at(buffer.getDescription()).at(index).release());
                shaderStorageBuffers.at(buffer.getDescription()).erase(
                        shaderStorageBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_TARGET: {
                auto &target = dynamic_cast<RenderTarget &>(obj);
                bool found = false;
                long index = 0;
                for (auto i = 0; i < targets[target.getDescription()].size(); i++) {
                    if (targets[target.getDescription()][i].get() == &target) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = device.createRenderTarget(target.getDescription());
                if (--usedTargets[target.getDescription()] == 0) {
                    usedTargets.erase(target.getDescription());
                    targets.at(target.getDescription()).erase(targets.at(target.getDescription()).begin() + index);
                }
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PIPELINE: {
                auto &pip = dynamic_cast<RenderPipeline &>(obj);
                auto ret = device.createRenderPipeline(pip.getDescription(), shaderDecompiler);
                if (--usedPipelines[pip.getDescription()] == 0) {
                    usedPipelines.erase(pip.getDescription());
                    pipelines.erase(pip.getDescription());
                }
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PASS: {
                auto &p = dynamic_cast<RenderPass &>(obj);
                auto ret = device.createRenderPass(p.getDescription());
                if (--usedPasses[p.getDescription()] == 0) {
                    usedPasses.erase(p.getDescription());
                    passes.erase(p.getDescription());
                }
                return ret;
            }
        }
    }
}