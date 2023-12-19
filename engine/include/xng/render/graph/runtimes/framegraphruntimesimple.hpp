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

#ifndef XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP
#define XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP

#include "xng/render/graph/framegraph.hpp"
#include "xng/render/graph/framegraphruntime.hpp"

#include "xng/shader/shadercompiler.hpp"
#include "xng/shader/shaderdecompiler.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    /**
     * A frame graph runtime with a simple sequential execution model on a single queue and pooled resource allocations.
     */
    class XENGINE_EXPORT FrameGraphRuntimeSimple : public FrameGraphRuntime {
    public:
        FrameGraphRuntimeSimple(RenderTarget &backBuffer,
                                RenderDevice &device,
                                ShaderCompiler &shaderCompiler,
                                ShaderDecompiler &shaderDecompiler);

        void execute(const FrameGraph &graph) override;

        const RenderTargetDesc &getBackBufferDesc() override;

        const RenderDeviceInfo &getRenderDeviceInfo() override;

    private:
        RenderObject &getObject(FrameGraphResource resource);

        void flushBufferCommands();

        void flushRenderCommands();

        void cmdCreate(const FrameGraphCommand &cmd);

        void cmdUpload(const FrameGraphCommand &cmd);

        void cmdCopy(const FrameGraphCommand &cmd);

        void cmdBlit(const FrameGraphCommand &cmd);

        void cmdBeginPass(const FrameGraphCommand &cmd);

        void cmdFinishPass(const FrameGraphCommand &cmd);

        void cmdClear(const FrameGraphCommand &cmd);

        void cmdBindPipeline(const FrameGraphCommand &cmd);

        void cmdBindVertexBuffers(const FrameGraphCommand &cmd);

        void cmdBindShaderResources(const FrameGraphCommand &cmd);

        void cmdSetViewport(const FrameGraphCommand &cmd);

        void cmdDraw(const FrameGraphCommand &cmd);

        RenderObject &allocate(const FrameGraphResource &res,
                               RenderObject::Type type,
                               std::variant<RenderTargetDesc,
                                       RenderPipelineDesc,
                                       TextureBufferDesc,
                                       TextureArrayBufferDesc,
                                       VertexBufferDesc,
                                       IndexBufferDesc,
                                       VertexArrayObjectDesc,
                                       ShaderUniformBufferDesc,
                                       ShaderStorageBufferDesc,
                                       RenderPassDesc> data);

        void deallocate(const FrameGraphResource &resource);

        void persist(const FrameGraphResource &resource);

        void collectGarbage();

        RenderPipeline &getPipeline(const RenderPipelineDesc &desc);

        RenderPass &getRenderPass(const RenderPassDesc &desc);

        VertexBuffer &createVertexBuffer(const VertexBufferDesc &desc);

        IndexBuffer &createIndexBuffer(const IndexBufferDesc &desc);

        VertexArrayObject &createVertexArrayObject(const VertexArrayObjectDesc &desc);

        TextureBuffer &createTextureBuffer(const TextureBufferDesc &desc);

        TextureArrayBuffer &createTextureArrayBuffer(const TextureArrayBufferDesc &desc);

        ShaderUniformBuffer &createShaderUniformBuffer(const ShaderUniformBufferDesc &desc);

        ShaderStorageBuffer &createShaderStorageBuffer(const ShaderStorageBufferDesc &desc);

        RenderTarget &createRenderTarget(const RenderTargetDesc &desc);

        CommandBuffer &createCommandBuffer();

        std::unique_ptr<RenderObject> persist(RenderObject &obj);

        std::map<FrameGraphCommand::Type, std::function<void(const FrameGraphCommand &cmd)>> commandJumpTable;

        RenderTarget &backBuffer;
        RenderDevice &device;

        ShaderCompiler &shaderCompiler;
        ShaderDecompiler &shaderDecompiler;

        std::unordered_map<RenderPipelineDesc, std::unique_ptr<RenderPipeline>> pipelines;
        std::unordered_map<RenderPassDesc, std::unique_ptr<RenderPass>> passes;
        std::unordered_map<VertexBufferDesc, std::vector<std::unique_ptr<VertexBuffer>>> vertexBuffers;
        std::unordered_map<IndexBufferDesc, std::vector<std::unique_ptr<IndexBuffer>>> indexBuffers;
        std::unordered_map<VertexArrayObjectDesc, std::vector<std::unique_ptr<VertexArrayObject>>> vertexArrayObjects;
        std::unordered_map<TextureBufferDesc, std::vector<std::unique_ptr<TextureBuffer>>> textures;
        std::unordered_map<TextureArrayBufferDesc, std::vector<std::unique_ptr<TextureArrayBuffer>>> textureArrays;
        std::unordered_map<ShaderUniformBufferDesc, std::vector<std::unique_ptr<ShaderUniformBuffer>>> shaderBuffers;
        std::unordered_map<ShaderStorageBufferDesc, std::vector<std::unique_ptr<ShaderStorageBuffer>>> shaderStorageBuffers;
        std::unordered_map<RenderTargetDesc, std::vector<std::unique_ptr<RenderTarget>>> targets;
        std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

        std::unordered_map<RenderPipelineDesc, int> usedPipelines;
        std::unordered_map<RenderPassDesc, int> usedPasses;
        std::unordered_map<VertexBufferDesc, int> usedVertexBuffers;
        std::unordered_map<IndexBufferDesc, int> usedIndexBuffers;
        std::unordered_map<VertexArrayObjectDesc, int> usedVertexArrayObjects;
        std::unordered_map<TextureBufferDesc, int> usedTextures;
        std::unordered_map<TextureArrayBufferDesc, int> usedTextureArrays;
        std::unordered_map<ShaderUniformBufferDesc, int> usedShaderBuffers;
        std::unordered_map<ShaderStorageBufferDesc, int> usedShaderStorageBuffers;
        std::unordered_map<RenderTargetDesc, int> usedTargets;
        size_t usedCommandBuffers = 0;

        FrameGraph graph;

        std::map<FrameGraphResource, RenderObject *> objects;
        std::map<FrameGraphResource, std::unique_ptr<RenderObject>> persistentObjects;

        std::set<FrameGraphResource> dirtyBuffers;
        std::vector<Command> pendingBufferCommands;

        std::set<FrameGraphResource> attachments;
        std::set<FrameGraphResource> vertexBindings;

        std::vector<Command> pendingRenderCommands;

        std::unique_ptr<CommandBuffer> commandBuffer;

        RenderPass *pass = nullptr;
        RenderPipeline *pipeline = nullptr;
    };
}
#endif //XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP
