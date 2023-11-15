/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_VKRENDERDEVICE_HPP
#define XENGINE_VKRENDERDEVICE_HPP

#include "xng/gpu/renderdevice.hpp"

#include <vulkan/vulkan.h>

namespace xng::vulkan {
    class VkRenderDevice : public RenderDevice {
    public:
        VkInstance instance;

        VkRenderDevice(VkInstance instance) : instance(instance) {}

        ~VkRenderDevice() {}

        const RenderDeviceInfo &getInfo() override {
            throw std::runtime_error("");
        }

        std::vector<std::reference_wrapper<CommandQueue>> getRenderCommandQueues() override {
            return std::vector<std::reference_wrapper<CommandQueue>>();
        }

        std::vector<std::reference_wrapper<CommandQueue>> getComputeCommandQueues() override {
            return std::vector<std::reference_wrapper<CommandQueue>>();
        }

        std::vector<std::reference_wrapper<CommandQueue>> getTransferCommandQueues() override {
            return std::vector<std::reference_wrapper<CommandQueue>>();
        }

        std::unique_ptr<CommandBuffer> createCommandBuffer() override {
            return std::unique_ptr<CommandBuffer>();
        }

        std::shared_ptr<CommandSemaphore> createSemaphore() override {
            return std::shared_ptr<CommandSemaphore>();
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc,
                                                             ShaderDecompiler &decompiler) override {
            return std::unique_ptr<RenderPipeline>();
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) override {
            return std::unique_ptr<RenderPipeline>();
        }

        std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc,
                                                               ShaderDecompiler &decompiler) override {
            return std::unique_ptr<ComputePipeline>();
        }

        std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) override {
            return std::unique_ptr<RaytracePipeline>();
        }

        std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) override {
            return std::unique_ptr<RenderTarget>();
        }

        std::unique_ptr<VertexArrayObject> createVertexArrayObject(const VertexArrayObjectDesc &desc) override {
            return std::unique_ptr<VertexArrayObject>();
        }

        std::unique_ptr<RenderPass> createRenderPass(const RenderPassDesc &desc) override {
            return std::unique_ptr<RenderPass>();
        }

        std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) override {
            return std::unique_ptr<VertexBuffer>();
        }

        std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc) override {
            return std::unique_ptr<IndexBuffer>();
        }

        std::unique_ptr<ShaderUniformBuffer> createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) override {
            return std::unique_ptr<ShaderUniformBuffer>();
        }

        std::unique_ptr<ShaderStorageBuffer> createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) override {
            return std::unique_ptr<ShaderStorageBuffer>();
        }

        std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) override {
            return std::unique_ptr<TextureBuffer>();
        }

        std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc) override {
            return std::unique_ptr<TextureArrayBuffer>();
        }

        std::unique_ptr<VideoMemory> createMemory(const VideoMemoryDesc &desc) override {
            return std::unique_ptr<VideoMemory>();
        }

        void setDebugCallback(const std::function<void(const std::string &)> &callback) override {

        }

        RenderStatistics getFrameStats() override {
            return RenderStatistics();
        }
    };
}

#endif //XENGINE_VKRENDERDEVICE_HPP
