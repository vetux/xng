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
    };

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

        //TODO: Pass Ordering
        TransferContextGL transferContext(PassResources(transientResources, heapResources), stats);
        RasterContextGL rasterContext(PassResources(transientResources, heapResources),
                                      data->pipelineCache,
                                      stats);
        ComputeContextGL computeContext(PassResources(transientResources, heapResources),
                                        data->pipelineCache);
        for (auto &pass: graph.passes) {
            switch (pass.index()) {
                case 0: {
                    auto p = std::get<TransferPass>(pass);
                    p.callback(transferContext);
                    break;
                }
                case 1: {
                    auto p = std::get<RasterPass>(pass);
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
