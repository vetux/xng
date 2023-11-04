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

#ifndef XENGINE_FRAMEGRAPHBUILDER_HPP
#define XENGINE_FRAMEGRAPHBUILDER_HPP

#include "xng/render/graph/framegraphresource.hpp"
#include "xng/render/graph/framegraph.hpp"
#include "xng/render/graph/framegraphslot.hpp"

#include "xng/render/scene.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphBuilder {
    public:
        FrameGraphBuilder(RenderTarget &backBuffer,
                          RenderDeviceInfo deviceInfo,
                          const Scene &scene,
                          const GenericMapString &properties,
                          std::set<FrameGraphResource> persistentResources,
                          ShaderCompiler &shaderCompiler,
                          ShaderDecompiler &shaderDecompiler);

        /**
         * Setup and compile a frame graph using the supplied passes.
         *
         * @param passes
         * @param persistHandles  The mapping of persist handles to the
         * @return
         */
        FrameGraph build(const std::vector<std::shared_ptr<FrameGraphPass>> &passes);

        FrameGraphResource createRenderTarget(const RenderTargetDesc &desc);

        FrameGraphResource createPipeline(const RenderPipelineDesc &desc);

        FrameGraphResource createRenderPass(const RenderPassDesc &desc);

        FrameGraphResource createTextureBuffer(const TextureBufferDesc &desc);

        FrameGraphResource createTextureArrayBuffer(const TextureArrayBufferDesc &desc);

        FrameGraphResource createVertexBuffer(const VertexBufferDesc &desc);

        FrameGraphResource createIndexBuffer(const IndexBufferDesc &desc);

        FrameGraphResource createVertexArrayObject(const VertexArrayObjectDesc &desc);

        FrameGraphResource createShaderUniformBuffer(const ShaderUniformBufferDesc &desc);

        FrameGraphResource createShaderStorageBuffer(const ShaderStorageBufferDesc &desc);

        FrameGraphResource createCommandBuffer();

        /**
         * Declare that the pass will write to the specified resource handle.
         * Resources which have not been declared with write / read or have been created can not be accessed by the pass.
         *
         * @param target
         */
        void write(FrameGraphResource target);

        /**
         * Declare that the pass will read from the specified resource handle.
         * Resources which have not been declared with write / read or have been created can not be accessed by the pass.
         *
         * @param source
         */
        void read(FrameGraphResource source);

        /**
         * Request the passed resource handle to be persisted to the next frame.
         *
         * This allows the pass to avoid reallocating gpu buffers for
         * resources which do not change such as scene textures or meshes and allowing the pass to implement
         * memory management techniques for the data inside the buffers allocated by the FrameGraphAllocator.
         *
         * The pass can store the handle and reuse it in the next frame to retrieve the persisted resource.
         * persist must be called again by the pass each frame while the handle should be persisted.
         *
         * The render object itself might be moved or reused by the allocator but it is ensured that the data
         * referred to by the persisted handle is identical in the next frame.
         *
         * @param resource
         */
        void persist(FrameGraphResource resource);

        /**
         * Declare a resource to be bound to the corresponding output slot for this frame.
         * If the slot is already occupied an exception is thrown.
         *
         * @param slot
         * @param resource
         */
        void assignSlot(FrameGraphSlot slot, FrameGraphResource resource);

        /**
         * Declare access on a slot.
         * If the slot has not been assigned an exception is thrown.
         *
         * @param slot
         * @return
         */
        FrameGraphResource getSlot(FrameGraphSlot slot);

        /**
         * Check wheter a slot has been assigned
         *
         * @param slot
         * @return
         */
        bool checkSlot(FrameGraphSlot slot);

        /**
         * @return The resource handle of the back buffer to render into.
         */
        FrameGraphResource getBackBuffer();

        /**
         * @return The description object of the back buffer target.
         */
        RenderTargetDesc getBackBufferDescription();

        /**
         * @return The resolution to render at
         */
        Vec2i getRenderSize();

        /**
         * @return The scene containing the user specified data.
         */
        const Scene &getScene() const;

        /**
         * The properties map contains static configuration data.
         *
         * @return
         */
        const GenericMapString &getProperties() const;

        ShaderCompiler &getShaderCompiler();

        ShaderDecompiler &getShaderDecompiler();

        const RenderDeviceInfo &getDeviceInfo();

    private:
        FrameGraphResource createResourceId();

        void checkResourceHandle(FrameGraphResource res);

        struct PassSetup {
            std::set<FrameGraphResource> allocations;
            std::set<FrameGraphResource> writes;
            std::set<FrameGraphResource> reads;
            std::set<FrameGraphResource> persists;
        };

        RenderTarget &backBuffer;
        const Scene &scene;
        const GenericMapString &properties;

        FrameGraph graph;

        PassSetup currentPass;

        std::set<FrameGraphResource> persistentResources;

        std::map<FrameGraphSlot, FrameGraphResource> graphSlots;

        std::vector<FrameGraphResource> renderQueues;
        std::vector<FrameGraphResource> computeQueues;
        std::vector<FrameGraphResource> transferQueues;

        size_t resourceCounter = 1;

        ShaderCompiler &shaderCompiler;
        ShaderDecompiler &shaderDecompiler;

        RenderDeviceInfo deviceInfo;
    };
}
#endif //XENGINE_FRAMEGRAPHBUILDER_HPP
