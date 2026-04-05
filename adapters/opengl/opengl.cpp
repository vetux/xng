/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/adapters/opengl/opengl.hpp"

#include "surfacegl.hpp"
#include "pipelinecachegl.hpp"
#include "heapgl.hpp"
#include "passresources.hpp"

#include "context/computecontextgl.hpp"
#include "context/rastercontextgl.hpp"
#include "context/transfercontextgl.hpp"

#include "resource/buffergl.hpp"
#include "resource/texturegl.hpp"

#include "colorbytesize.hpp"

namespace xng::opengl {
    struct Runtime::MemberData {
        std::unique_ptr<HeapGL> heap;

        PipelineCacheGL pipelineCache;

        std::string vendor;
        std::string renderer;
        std::string version;

        std::unordered_map<Buffer, std::vector<std::shared_ptr<BufferGL> >, BufferHash> cachedBuffers{};
        std::unordered_map<Texture, std::vector<std::shared_ptr<TextureGL> >, TextureHash> cachedTextures{};

        Framebuffer framebuffer;
    };

    Vec2i bindAttachments(const Framebuffer &framebuffer,
                         const rendergraph::RasterPass &pass,
                         const PassResources &passResources) {
        oglDebugStartGroup("Runtime::bindAttachments");

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.FBO);

        Vec2i ret(0);
        for (auto i = 0; i < pass.colorAttachments.size(); ++i) {
            auto &attachment = pass.colorAttachments.at(i);

            // Get the attachment texture.
            const TextureGL *tex = nullptr;
            if (std::holds_alternative<std::shared_ptr<Surface> >(attachment.target)) {
                auto &surface = std::get<std::shared_ptr<Surface> >(attachment.target);
                auto &surfaceGL = down_cast<SurfaceGL &>(*surface.get());

                // Per OpenGL spec this backBufferColor texture should be shareable between the surface context and the global context.
                tex = surfaceGL.backBufferColor.get();
            } else {
                tex = &passResources.getTexture(std::get<Resource<Texture> >(attachment.target));
            }
            assert(tex != nullptr);

            auto &texture = *tex;
            if (ret.x + ret.y > 0 && ret != texture.desc.size) {
                throw std::runtime_error("All attachments must have the same size");
            } else {
                ret = texture.desc.size;
            }
            if (attachment.clearValue.has_value()) {
                TransferContextGL::clearTexture(texture, attachment.targetSubResource, attachment.clearValue.value());
            }
            framebuffer.attach(GL_COLOR_ATTACHMENT0 + i, texture, attachment.targetSubResource);
        }

        if (pass.depthStencilAttachment.has_value()) {
            if (std::holds_alternative<RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value())) {
                // Separate depth / stencil attachments
                auto &attachments = std::get<RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value());

                // Depth Attachment
                if (attachments.depthAttachment.has_value()) {
                    auto &attachment = attachments.depthAttachment.value();
                    if (std::holds_alternative<std::shared_ptr<Surface> >(attachment.target)) {
                        throw std::runtime_error("Depth attachment cannot be a surface");
                    }
                    const auto &texture = passResources.getTexture(std::get<Resource<Texture> >(attachment.target));
                    if (ret.x + ret.y > 0 && ret != texture.desc.size) {
                        throw std::runtime_error("All attachments must have the same size");
                    } else {
                        ret = texture.desc.size;
                    }
                    if (attachment.clearValue.has_value()) {
                        TransferContextGL::clearTexture(texture,
                                                        attachment.targetSubResource,
                                                        attachment.clearValue.value());
                    }
                    framebuffer.attach(GL_DEPTH_ATTACHMENT, texture, attachment.targetSubResource);
                }

                // Stencil Attachment
                if (attachments.stencilAttachment.has_value()) {
                    auto &attachment = attachments.stencilAttachment.value();
                    if (std::holds_alternative<std::shared_ptr<Surface> >(attachment.target)) {
                        throw std::runtime_error("Stencil attachment cannot be a surface");
                    }
                    const auto &texture = passResources.getTexture(std::get<Resource<Texture> >(attachment.target));
                    if (ret.x + ret.y > 0 && ret != texture.desc.size) {
                        throw std::runtime_error("All attachments must have the same size");
                    } else {
                        ret = texture.desc.size;
                    }
                    if (attachment.clearValue.has_value()) {
                        TransferContextGL::clearTexture(texture,
                                                        attachment.targetSubResource,
                                                        attachment.clearValue.value());
                    }
                    framebuffer.attach(GL_STENCIL_ATTACHMENT, texture, attachment.targetSubResource);
                }
            } else {
                // Combined depth / stencil attachment
                auto &attachment = std::get<Attachment>(pass.depthStencilAttachment.value());

                if (std::holds_alternative<std::shared_ptr<Surface> >(attachment.target)) {
                    throw std::runtime_error("DepthStencil attachment cannot be a surface");
                }
                const auto &texture = passResources.getTexture(std::get<Resource<Texture> >(attachment.target));
                if (ret.x + ret.y > 0 && ret != texture.desc.size) {
                    throw std::runtime_error("All attachments must have the same size");
                } else {
                    ret = texture.desc.size;
                }
                if (attachment.clearValue.has_value()) {
                    TransferContextGL::clearTexture(texture,
                                                    attachment.targetSubResource,
                                                    attachment.clearValue.value());
                }
                framebuffer.attach(GL_DEPTH_STENCIL_ATTACHMENT, texture, attachment.targetSubResource);
            }
        }

        auto fstatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (fstatus != GL_FRAMEBUFFER_COMPLETE) {
            const char *msg = "UNKNOWN";
            switch (fstatus) {
                case GL_FRAMEBUFFER_UNDEFINED: msg = "UNDEFINED";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "INCOMPLETE_ATTACHMENT";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "MISSING_ATTACHMENT";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: msg = "INCOMPLETE_DRAW_BUFFER";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: msg = "INCOMPLETE_READ_BUFFER";
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED: msg = "UNSUPPORTED";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: msg = "INCOMPLETE_MULTISAMPLE";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: msg = "INCOMPLETE_LAYER_TARGETS";
                    break;
            }
            throw std::runtime_error(std::string("Framebuffer is incomplete ") + msg);
        }
        oglCheckError();

        oglDebugEndGroup();

        return ret;
    }

    Runtime::Runtime(DisplayEnvironment &env)
        : data(std::make_unique<MemberData>()) {
        auto wndAttr = WindowAttributes();
        wndAttr.visible = false;
        auto wnd = env.createWindow("XNG_HeapContext", Vec2i(0, 0), wndAttr);
        down_cast<WindowGl &>(*wnd).unbindContext();
        data->heap = std::make_unique<HeapGL>(std::move(wnd));
        data->vendor = std::string(reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
        data->renderer = std::string(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
        data->version = std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    }

    Runtime::~Runtime() = default;

    std::shared_ptr<rendergraph::Surface> Runtime::createSurface(std::shared_ptr<Window> window) {
        return std::make_shared<SurfaceGL>(std::move(window));
    }

    void Runtime::setFramesInFlight(size_t framesInFlight) {
    }

    rendergraph::Heap &Runtime::getResourceHeap() {
        return *data->heap;
    }

    rendergraph::PipelineCache &Runtime::getPipelineCache() {
        return data->pipelineCache;
    }

    rendergraph::Statistics Runtime::execute(const rendergraph::Graph &graph) {
        Statistics stats;

        std::unordered_set<SurfaceGL *> surfaces;

        // TODO: Transient Aliasing
        ResourceScope transientResources;
        for (auto &alloc: graph.bufferAllocations) {
            auto it = data->cachedBuffers.find(alloc.second);
            if (it != data->cachedBuffers.end()) {
                transientResources.buffers.emplace(alloc.first.getHandle(), it->second.back());
                it->second.pop_back();
            } else {
                transientResources.buffers.emplace(alloc.first.getHandle(), std::make_shared<BufferGL>(alloc.second));
            }
        }

        for (auto &alloc: graph.textureAllocations) {
            auto it = data->cachedTextures.find(alloc.second);
            if (it != data->cachedTextures.end()) {
                transientResources.textures.emplace(alloc.first.getHandle(), it->second.back());
                it->second.pop_back();
            } else {
                transientResources.textures.emplace(alloc.first.getHandle(), std::make_shared<TextureGL>(alloc.second));
            }
        }

        // Sync Referenced Heap Resource Accesses
        for (auto &pass: graph.passes) {
            switch (pass.index()) {
                case 0: {
                    auto p = std::get<TransferPass>(pass);
                    for (auto &usage: p.bufferUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    for (auto &usage: p.textureUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    break;
                }
                case 1: {
                    auto p = std::get<RasterPass>(pass);
                    for (auto &usage: p.bufferUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    for (auto &usage: p.textureUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    for (auto &att: p.colorAttachments) {
                        if (std::holds_alternative<std::shared_ptr<Surface> >(att.target)) {
                            surfaces.insert(down_cast<SurfaceGL *>(
                                std::get<std::shared_ptr<Surface> >(att.target).get()));
                        }
                    }
                    break;
                }
                case 2: {
                    auto p = std::get<ComputePass>(pass);
                    for (auto &usage: p.bufferUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    for (auto &usage: p.textureUsages) {
                        if (usage.first.getNameSpace() == ResourceId::HEAP) {
                            data->heap->getTransferContextGL().wait(usage.first.getHandle());
                        }
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Invalid pass type");
            }
        }

        auto heapResources = data->heap->getResources();

        auto passResources = PassResources(transientResources, heapResources);

        TransferContextGL transferContext(passResources, stats);
        RasterContextGL rasterContext(passResources,
                                      data->pipelineCache,
                                      stats);
        ComputeContextGL computeContext(passResources,
                                        data->pipelineCache);

        //TODO: Pass Ordering
        for (auto &pass: graph.passes) {
            switch (pass.index()) {
                case 0: {
                    auto p = std::get<TransferPass>(pass);
                    p.callback(transferContext);
                    break;
                }
                case 1: {
                    auto p = std::get<RasterPass>(pass);
                    auto dimensions = bindAttachments(data->framebuffer, p, passResources);
                    rasterContext.setFrameBufferHeight(dimensions.y);
                    p.callback(rasterContext);
                    break;
                }
                case 2: {
                    auto p = std::get<ComputePass>(pass);
                    p.callback(computeContext);
                    break;
                }
                default:
                    throw std::runtime_error("Invalid pass type");
            }
        }

        for (auto &surface: surfaces) {
            surface->bindContext();
            surface->present();
            surface->update();
            surface->unbindContext();
        }

        data->cachedBuffers.clear();
        data->cachedTextures.clear();

        for (auto &buf: transientResources.buffers) {
            data->cachedBuffers[buf.second->desc].emplace_back(std::move(buf.second));
        }

        for (auto &tex: transientResources.textures) {
            data->cachedTextures[tex.second->desc].emplace_back(std::move(tex.second));
        }

        return stats;
    }

    rendergraph::Statistics Runtime::execute(const std::vector<rendergraph::Graph> &graphs) {
        Statistics stats;
        stats.vendor = data->vendor;
        stats.renderer = data->renderer;
        stats.version = data->version;
        for (auto &graph: graphs) {
            stats += execute(graph);
        }
        return stats;
    }
}
