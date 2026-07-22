/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "xng/adapters/opengl/opengl.hpp"

#include "surfacegl.hpp"
#include "pipelinecachegl.hpp"
#include "heapgl.hpp"
#include "passresources.hpp"
#include "fencegl.hpp"

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

        std::unordered_map<Buffer, std::vector<std::shared_ptr<BufferGL> >, BufferHash> cachedBuffers{};
        std::unordered_map<Texture, std::vector<std::shared_ptr<TextureGL> >, TextureHash> cachedTextures{};

        DeviceInformation deviceInfo;

        bool enableTimers = false;
    };

    //TODO: Implement DAG based barrier insertion.
    static void insertBarrier(const GraphicsPass &pass) {
        for (auto &pair: pass.bufferUsages) {
            for (auto &entry: pair.second.entries) {
                switch (entry.access.type) {
                    case BufferAccess::StorageWrite:
                        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
                                        | GL_ELEMENT_ARRAY_BARRIER_BIT
                                        | GL_UNIFORM_BARRIER_BIT
                                        | GL_TEXTURE_FETCH_BARRIER_BIT
                                        | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                                        | GL_COMMAND_BARRIER_BIT
                                        | GL_PIXEL_BUFFER_BARRIER_BIT
                                        | GL_TEXTURE_UPDATE_BARRIER_BIT
                                        | GL_BUFFER_UPDATE_BARRIER_BIT
                                        | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
                                        | GL_FRAMEBUFFER_BARRIER_BIT
                                        | GL_TRANSFORM_FEEDBACK_BARRIER_BIT
                                        | GL_ATOMIC_COUNTER_BARRIER_BIT
                                        | GL_SHADER_STORAGE_BARRIER_BIT
                                        | GL_QUERY_BUFFER_BARRIER_BIT);
                        return;
                    default:
                        break;
                }
            }
        }
        for (auto &pair: pass.textureUsages) {
            for (auto &entry: pair.second.entries) {
                switch (entry.access.type) {
                    case TextureAccess::TextureStorageWrite:
                        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
                                        | GL_ELEMENT_ARRAY_BARRIER_BIT
                                        | GL_UNIFORM_BARRIER_BIT
                                        | GL_TEXTURE_FETCH_BARRIER_BIT
                                        | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                                        | GL_COMMAND_BARRIER_BIT
                                        | GL_PIXEL_BUFFER_BARRIER_BIT
                                        | GL_TEXTURE_UPDATE_BARRIER_BIT
                                        | GL_BUFFER_UPDATE_BARRIER_BIT
                                        | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
                                        | GL_FRAMEBUFFER_BARRIER_BIT
                                        | GL_TRANSFORM_FEEDBACK_BARRIER_BIT
                                        | GL_ATOMIC_COUNTER_BARRIER_BIT
                                        | GL_SHADER_STORAGE_BARRIER_BIT
                                        | GL_QUERY_BUFFER_BARRIER_BIT);
                        return;
                    default:
                        break;
                }
            }
        }
    }

    static void insertBarrier(const ComputePass &pass) {
        for (auto &pair: pass.bufferUsages) {
            for (auto &entry: pair.second.entries) {
                switch (entry.type) {
                    case BufferAccess::StorageWrite:
                        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
                                        | GL_ELEMENT_ARRAY_BARRIER_BIT
                                        | GL_UNIFORM_BARRIER_BIT
                                        | GL_TEXTURE_FETCH_BARRIER_BIT
                                        | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                                        | GL_COMMAND_BARRIER_BIT
                                        | GL_PIXEL_BUFFER_BARRIER_BIT
                                        | GL_TEXTURE_UPDATE_BARRIER_BIT
                                        | GL_BUFFER_UPDATE_BARRIER_BIT
                                        | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
                                        | GL_FRAMEBUFFER_BARRIER_BIT
                                        | GL_TRANSFORM_FEEDBACK_BARRIER_BIT
                                        | GL_ATOMIC_COUNTER_BARRIER_BIT
                                        | GL_SHADER_STORAGE_BARRIER_BIT
                                        | GL_QUERY_BUFFER_BARRIER_BIT);
                        return;
                    default:
                        break;
                }
            }
        }
        for (auto &pair: pass.textureUsages) {
            for (auto &entry: pair.second.entries) {
                switch (entry.type) {
                    case TextureAccess::TextureStorageWrite:
                        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
                                        | GL_ELEMENT_ARRAY_BARRIER_BIT
                                        | GL_UNIFORM_BARRIER_BIT
                                        | GL_TEXTURE_FETCH_BARRIER_BIT
                                        | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                                        | GL_COMMAND_BARRIER_BIT
                                        | GL_PIXEL_BUFFER_BARRIER_BIT
                                        | GL_TEXTURE_UPDATE_BARRIER_BIT
                                        | GL_BUFFER_UPDATE_BARRIER_BIT
                                        | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
                                        | GL_FRAMEBUFFER_BARRIER_BIT
                                        | GL_TRANSFORM_FEEDBACK_BARRIER_BIT
                                        | GL_ATOMIC_COUNTER_BARRIER_BIT
                                        | GL_SHADER_STORAGE_BARRIER_BIT
                                        | GL_QUERY_BUFFER_BARRIER_BIT);
                        return;
                    default:
                        break;
                }
            }
        }
    }

    Runtime::Runtime(DisplayEnvironment &env)
        : data(std::make_unique<MemberData>()) {
#ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                          GLsizei length, const GLchar* message, const void* userParam) {
            // Filter specifically for errors or API mismanagement
            if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
                std::cerr << "[OpenGL Error] " << message << std::endl;
            }
}, nullptr);
#endif

        data->heap = std::make_unique<HeapGL>();

        data->deviceInfo.vendor = std::string(reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
        data->deviceInfo.name = std::string(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
        data->deviceInfo.version = std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));

        for (auto format = COLOR_FORMAT_BEGIN; format != COLOR_FORMAT_COMPRESSED_START;
             format = static_cast<rg::ColorFormat>(format + 1)) {
            data->deviceInfo.capabilities.supportedColorFormats.insert(format);
        }

        data->deviceInfo.capabilities.supportedColorFormats.insert(RGBA_BC7);
        data->deviceInfo.capabilities.supportedColorFormats.insert(RGBA_BC7_SRGB);
        data->deviceInfo.capabilities.supportedColorFormats.insert(RGB_BC6H_SFLOAT);
        data->deviceInfo.capabilities.supportedColorFormats.insert(RGB_BC6H_UFLOAT);
        data->deviceInfo.capabilities.supportedColorFormats.insert(RG_BC5_SNORM);
        data->deviceInfo.capabilities.supportedColorFormats.insert(RG_BC5_UNORM);
        data->deviceInfo.capabilities.supportedColorFormats.insert(R_BC4_SNORM);
        data->deviceInfo.capabilities.supportedColorFormats.insert(R_BC4_UNORM);

        GLint maxUboBindings = 0;
        GLint maxUboSize = 0;
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUboBindings);
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUboSize);
        data->deviceInfo.capabilities.maxUniformBufferBindings = static_cast<size_t>(maxUboBindings);
        data->deviceInfo.capabilities.maxUniformBufferSize = static_cast<size_t>(maxUboSize);

        GLint maxSsboBindings = 0;
        GLint64 maxSsboSize = 0;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSsboBindings);
        glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSsboSize);
        data->deviceInfo.capabilities.maxStorageBufferBindings = static_cast<size_t>(maxSsboBindings);
        data->deviceInfo.capabilities.maxStorageBufferSize = static_cast<size_t>(maxSsboSize);
    }

    Runtime::~Runtime() = default;

    std::shared_ptr<rg::Surface> Runtime::createSurface(std::shared_ptr<Window> window, size_t swapCount) {
        return std::make_shared<SurfaceGL>(std::move(window));
    }

    void Runtime::setEnableTimers(const bool enableTimers) {
        data->enableTimers = enableTimers;
    }

    rg::Heap &Runtime::getResourceHeap() {
        return *data->heap;
    }

    rg::PipelineCache &Runtime::getPipelineCache() {
        return data->pipelineCache;
    }

    const Runtime::DeviceInformation &Runtime::getDeviceInformation() {
        return data->deviceInfo;
    }

    rg::TextureFormatLimits Runtime::getTextureFormatLimits(rg::TextureType type,
                                                            rg::ColorFormat colorFormat,
                                                            rg::Texture::Capability capabilities) {
        // In GL the GL_TEXTURE_PROXY path for getting format limits is unreliable; therefore, we will return the guaranteed minimum from the 4.6 spec.
        return TextureFormatLimits(Vec2u(16384, 16384), 15, 2048);
    }

    std::unique_ptr<Fence> Runtime::execute(const rg::Graph &graph) {
        std::unordered_set<SurfaceGL *> surfaces;

        for (auto &pass: graph.passes) {
            if (std::holds_alternative<GraphicsPass>(pass)) {
                auto p = std::get<GraphicsPass>(pass);
                for (auto &pair: p.surfaceUsages) {
                    surfaces.insert(down_cast<SurfaceGL *>(pair.first.get()));
                }
            }
        }

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

        auto heapResources = data->heap->getResources();

        auto passResources = PassResources(transientResources, heapResources);

        TransferContextGL transferContext(passResources);
        RasterContextGL rasterContext(passResources, data->pipelineCache);
        ComputeContextGL computeContext(passResources, data->pipelineCache);

        Timeline timeline;
        if (data->enableTimers) {
            GLint64 gpuNow;
            glGetInteger64v(GL_TIMESTAMP, &gpuNow);
            timeline.submitTimeDevice = std::chrono::nanoseconds(gpuNow);
            timeline.submitTimeHost = std::chrono::high_resolution_clock::now();
        }

        std::vector<Query> queries;

        if (data->enableTimers) {
            for (auto &pass: graph.passes) {
                switch (pass.index()) {
                    case 0: {
                        auto p = std::get<TransferPass>(pass);
                        queries.emplace_back(p.name);
                        glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                        p.callback(transferContext);
                        glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                        break;
                    }
                    case 1: {
                        auto p = std::get<ComputePass>(pass);
                        queries.emplace_back(p.name);
                        glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                        p.callback(computeContext);
                        insertBarrier(p);
                        glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                        break;
                    }
                    case 2: {
                        auto p = std::get<GraphicsPass>(pass);
                        queries.emplace_back(p.name);
                        glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                        p.callback(rasterContext, transferContext, computeContext);
                        insertBarrier(p);
                        glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid pass type");
                }
            }
        } else {
            for (auto &pass: graph.passes) {
                switch (pass.index()) {
                    case 0: {
                        auto p = std::get<TransferPass>(pass);
                        OGLDebugGroup debug(p.name);
                        p.callback(transferContext);
                        break;
                    }
                    case 1: {
                        auto p = std::get<ComputePass>(pass);
                        OGLDebugGroup debug(p.name);
                        p.callback(computeContext);
                        insertBarrier(p);
                        break;
                    }
                    case 2: {
                        auto p = std::get<GraphicsPass>(pass);
                        OGLDebugGroup debug(p.name);
                        p.callback(rasterContext, transferContext, computeContext);
                        insertBarrier(p);
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid pass type");
                }
            }
        }

        for (auto &surface: surfaces) {
            // TODO: Verify surface synchronization
            // Here might be the cause of the occasional ghosting because the commands might not have finished before blitting the texture to the framebuffer.
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

        if (data->enableTimers) {
            return std::make_unique<FenceGL>(std::move(timeline), std::move(queries));
        }
        return std::make_unique<FenceGL>();
    }

    std::unique_ptr<Fence> Runtime::execute(const std::vector<rg::Graph> &graphs) {
        std::unordered_set<SurfaceGL *> surfaces;

        Timeline timeline;
        if (data->enableTimers) {
            GLint64 gpuNow;
            glGetInteger64v(GL_TIMESTAMP, &gpuNow);
            timeline.submitTimeDevice = std::chrono::nanoseconds(gpuNow);
            timeline.submitTimeHost = std::chrono::high_resolution_clock::now();
        }

        std::vector<Query> queries;

        for (auto &graph: graphs) {
            for (auto &pass: graph.passes) {
                switch (pass.index()) {
                    case 1: {
                        auto p = std::get<GraphicsPass>(pass);
                        for (auto &pair: p.surfaceUsages) {
                            surfaces.insert(down_cast<SurfaceGL *>(pair.first.get()));
                        }
                        break;
                    }
                    default:
                        continue;
                }
            }

            // TODO: Transient Aliasing
            ResourceScope transientResources;
            for (auto &alloc: graph.bufferAllocations) {
                auto it = data->cachedBuffers.find(alloc.second);
                if (it != data->cachedBuffers.end()) {
                    transientResources.buffers.emplace(alloc.first.getHandle(), it->second.back());
                    it->second.pop_back();
                } else {
                    transientResources.buffers.emplace(alloc.first.getHandle(),
                                                       std::make_shared<BufferGL>(alloc.second));
                }
            }

            for (auto &alloc: graph.textureAllocations) {
                auto it = data->cachedTextures.find(alloc.second);
                if (it != data->cachedTextures.end()) {
                    transientResources.textures.emplace(alloc.first.getHandle(), it->second.back());
                    it->second.pop_back();
                } else {
                    transientResources.textures.emplace(alloc.first.getHandle(),
                                                        std::make_shared<TextureGL>(alloc.second));
                }
            }

            auto heapResources = data->heap->getResources();

            auto passResources = PassResources(transientResources, heapResources);

            TransferContextGL transferContext(passResources);
            RasterContextGL rasterContext(passResources, data->pipelineCache);
            ComputeContextGL computeContext(passResources, data->pipelineCache);

            if (data->enableTimers) {
                for (auto &pass: graph.passes) {
                    switch (pass.index()) {
                        case 0: {
                            auto p = std::get<TransferPass>(pass);
                            queries.emplace_back(p.name);
                            glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                            p.callback(transferContext);
                            glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                            break;
                        }
                        case 1: {
                            auto p = std::get<ComputePass>(pass);
                            queries.emplace_back(p.name);
                            glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                            p.callback(computeContext);
                            insertBarrier(p);
                            glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                            break;
                        }
                        case 2: {
                            auto p = std::get<GraphicsPass>(pass);
                            queries.emplace_back(p.name);
                            glQueryCounter(queries.back().queries[0], GL_TIMESTAMP);
                            p.callback(rasterContext, transferContext, computeContext);
                            insertBarrier(p);
                            glQueryCounter(queries.back().queries[1], GL_TIMESTAMP);
                            break;
                        }
                        default:
                            throw std::runtime_error("Invalid pass type");
                    }
                }
            } else {
                for (auto &pass: graph.passes) {
                    switch (pass.index()) {
                        case 0: {
                            auto p = std::get<TransferPass>(pass);
                            p.callback(transferContext);
                            break;
                        }
                        case 1: {
                            auto p = std::get<ComputePass>(pass);
                            p.callback(computeContext);
                            insertBarrier(p);
                            break;
                        }
                        case 2: {
                            auto p = std::get<GraphicsPass>(pass);
                            p.callback(rasterContext, transferContext, computeContext);
                            insertBarrier(p);
                            break;
                        }
                        default:
                            throw std::runtime_error("Invalid pass type");
                    }
                }
            }

            for (auto &surface: surfaces) {
                // TODO: Verify surface synchronization
                // Here might be the cause of the occasional ghosting because the commands might not have finished before blitting the texture to the framebuffer.
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
        }

        if (data->enableTimers) {
            return std::make_unique<FenceGL>(std::move(timeline), std::move(queries));
        }
        return std::make_unique<FenceGL>();
    }
}
