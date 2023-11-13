/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_OGLRENDERDEVICE_HPP
#define XENGINE_OGLRENDERDEVICE_HPP

#include "xng/gpu/renderdevice.hpp"

#include <utility>
#include <mutex>
#include <unordered_set>

#include "opengl_include.hpp"

#include "gpu/opengl/oglrenderpipeline.hpp"
#include "gpu/opengl/oglrendertarget.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/oglshaderuniformbuffer.hpp"
#include "gpu/opengl/oglshaderstoragebuffer.hpp"
#include "gpu/opengl/oglindexbuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/oglvertexarrayobject.hpp"
#include "gpu/opengl/ogltexturearraybuffer.hpp"
#include "gpu/opengl/oglvideomemory.hpp"
#include "gpu/opengl/oglrenderpass.hpp"
#include "gpu/opengl/oglcommandbuffer.hpp"
#include "gpu/opengl/oglcommandqueue.hpp"
#include "gpu/opengl/oglsemaphore.hpp"
#include "gpu/opengl/oglcomputepipeline.hpp"

static std::function<void(const std::string &)> callback;

static std::string getDebugSource(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            return "GL_DEBUG_SOURCE_API";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "GL_DEBUG_SOURCE_APPLICATION";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "GL_DEBUG_SOURCE_SHADER_COMPILER";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_OTHER:
            return "GL_DEBUG_SOURCE_OTHER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "GL_DEBUG_SOURCE_THIRD_PARTY";
        default:
            return "UNKNOWN SOURCE";
    }
}

static std::string getDebugType(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "GL_DEBUG_TYPE_ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_MARKER:
            return "GL_DEBUG_TYPE_MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "GL_DEBUG_TYPE_OTHER";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "GL_DEBUG_TYPE_PERFORMANCE";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "GL_DEBUG_TYPE_POP_GROUP";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "GL_DEBUG_TYPE_PORTABILITY";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "GL_DEBUG_TYPE_PUSH_GROUP";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        default:
            return "UNKNOWN TYPE";
    }
}

static std::string getDebugSeverity(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
            return "GL_DEBUG_SEVERITY_LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "GL_DEBUG_SEVERITY_MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "GL_DEBUG_SEVERITY_HIGH";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "GL_DEBUG_SEVERITY_NOTIFICATION";
        default:
            return "UNKNOWN SEVERITY";
    }
}

static void oglDebugHandler(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam) {
    std::string msg = getDebugSource(source)
                      + "/"
                      + getDebugType(type)
                      + "/"
                      + getDebugSeverity(severity)
                      + " "
                      + std::to_string(id)
                      + " "
                      + std::string(message, length);
    if (callback) {
        callback(msg);
    }
}


namespace xng::opengl {
    class OGLRenderDevice : public RenderDevice {
    public:
        RenderDeviceInfo info;
        std::set<RenderObject *> objects;

        OGLCommandQueue queue;

        RenderStatistics statistics;

        explicit OGLRenderDevice(RenderDeviceInfo infoArg) : info(std::move(infoArg)),
                                                             queue(OGLCommandQueue(statistics)) {
            if (GLAD_GL_KHR_debug) {
                glDebugMessageCallback(oglDebugHandler, nullptr);
            }
        }

        ~OGLRenderDevice() override = default;

        const RenderDeviceInfo &getInfo() override {
            return info;
        }

        std::vector<std::reference_wrapper<CommandQueue>> getRenderCommandQueues() override {
            return {queue};
        }

        std::vector<std::reference_wrapper<CommandQueue>> getComputeCommandQueues() override {
            return {queue};
        }

        std::vector<std::reference_wrapper<CommandQueue>> getTransferCommandQueues() override {
            return {};
        }

        std::unique_ptr<CommandBuffer> createCommandBuffer() override {
            return std::make_unique<OGLCommandBuffer>();
        }

        std::shared_ptr<CommandSemaphore> createSemaphore() override {
            return std::make_shared<OGLSemaphore>();
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc,
                                                             ShaderDecompiler &decompiler) override {
            return std::make_unique<OGLRenderPipeline>( desc, decompiler);
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) override {
            throw std::runtime_error("Not Implemented");
        }

        std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc,
                                                               ShaderDecompiler &decompiler) override {
            return std::make_unique<OGLComputePipeline>( desc, decompiler);
        }

        std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) override {
            throw std::runtime_error("Not Implemented");
        }

        std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) override {
            return std::make_unique<OGLRenderTarget>( desc);
        }

        std::unique_ptr<VertexArrayObject> createVertexArrayObject(const VertexArrayObjectDesc &desc) override {
            return std::make_unique<OGLVertexArrayObject>( desc);
        }

        std::unique_ptr<RenderPass> createRenderPass(const RenderPassDesc &desc) override {
            return std::make_unique<OGLRenderPass>( desc);
        }

        std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) override {
            return std::make_unique<OGLVertexBuffer>( desc, statistics);
        }

        std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc) override {
            return std::make_unique<OGLIndexBuffer>( desc, statistics);
        }

        std::unique_ptr<ShaderUniformBuffer> createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) override {
            if (desc.size > info.uniformBufferMaxSize) {
                throw std::runtime_error(
                        "ShaderUniformBuffer size too large, size: "
                        + std::to_string(desc.size)
                        + " max: "
                        + std::to_string(info.uniformBufferMaxSize));
            }
            return std::make_unique<OGLShaderUniformBuffer>( desc, statistics);
        }

        std::unique_ptr<ShaderStorageBuffer> createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) override {
            if (desc.size > info.storageBufferMaxSize) {
                throw std::runtime_error(
                        "ShaderStorageBuffer size too large, size: "
                        + std::to_string(desc.size)
                        + " max: "
                        + std::to_string(info.storageBufferMaxSize));
            }
            return std::make_unique<OGLShaderStorageBuffer>( desc, statistics);
        }

        std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) override {
            return std::make_unique<OGLTextureBuffer>(desc, statistics);
        }

        std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc) override {
            return std::make_unique<OGLTextureArrayBuffer>(desc, statistics);
        }

        std::unique_ptr<VideoMemory> createMemory(const VideoMemoryDesc &desc) override {
            return std::make_unique<OGLVideoMemory>(*this, desc);
        }

        void setDebugCallback(const std::function<void(const std::string &)> &c) override {
            callback = c;
        }

        RenderStatistics getFrameStats() override {
            auto ret = statistics;
            statistics = {};
            return ret;
        }
    };
}

#endif //XENGINE_OGLRENDERDEVICE_HPP
