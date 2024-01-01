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

#ifndef XENGINE_FRAMEGRAPHBUILDER_HPP
#define XENGINE_FRAMEGRAPHBUILDER_HPP

#include <utility>
#include <cstring>

#include "xng/render/graph/framegraphresource.hpp"
#include "xng/render/graph/framegraph.hpp"
#include "xng/render/graph/framegraphslot.hpp"
#include "xng/render/graph/framegraphcommand.hpp"

#include "xng/render/scenerenderersettings.hpp"
#include "xng/render/scene/scene.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphBuilder {
    public:
        FrameGraphBuilder(RenderTargetDesc backBufferDesc,
                          RenderDeviceInfo deviceInfo,
                          const Scene &scene,
                          const SceneRendererSettings &settings,
                          std::set<FrameGraphResource> persistentResources);

        /**
         * Setup and compile a frame graph using the supplied passes.
         *
         * @param passes
         * @param persistHandles  The mapping of persist handles to the
         * @return
         */
        FrameGraph build(const std::vector<std::shared_ptr<FrameGraphPass>> &passes);

        FrameGraphResource createRenderPipeline(const RenderPipelineDesc &desc);

        FrameGraphResource createTextureBuffer(const TextureBufferDesc &desc);

        FrameGraphResource createTextureArrayBuffer(const TextureArrayBufferDesc &desc);

        FrameGraphResource createVertexBuffer(const VertexBufferDesc &desc);

        FrameGraphResource createIndexBuffer(const IndexBufferDesc &desc);

        FrameGraphResource createShaderUniformBuffer(const ShaderUniformBufferDesc &desc);

        FrameGraphResource createShaderStorageBuffer(const ShaderStorageBufferDesc &desc);

        void upload(FrameGraphResource buffer, std::function<FrameGraphUploadBuffer()> dataSource) {
            upload(buffer, 0, 0, {}, {}, std::move(dataSource));
        }

        void upload(FrameGraphResource buffer,
                    size_t offset,
                    std::function<FrameGraphUploadBuffer()> dataSource) {
            upload(buffer, 0, offset, {}, {}, std::move(dataSource));
        }

        void upload(FrameGraphResource buffer,
                    size_t index,
                    size_t offset,
                    ColorFormat colorFormat,
                    CubeMapFace cubeMapFace,
                    std::function<FrameGraphUploadBuffer()> dataSource);

        void copy(FrameGraphResource source,
                  FrameGraphResource dest,
                  size_t readOffset,
                  size_t writeOffset,
                  size_t count);

        /**
         *
         * @param source Either RenderTarget (Backbuffer) or TextureBuffer
         * @param target Either RenderTarget (Backbuffer) or TextureBuffer
         * @param sourceOffset
         * @param targetOffset
         * @param sourceRect
         * @param targetRect
         * @param filter
         * @param sourceIndex
         * @param targetIndex
         */
        void blitColor(FrameGraphResource source,
                       FrameGraphResource target,
                       Vec2i sourceOffset,
                       Vec2i targetOffset,
                       Vec2i sourceRect,
                       Vec2i targetRect,
                       TextureFiltering filter,
                       int sourceIndex,
                       int targetIndex);

        void blitDepth(FrameGraphResource source,
                       FrameGraphResource target,
                       Vec2i sourceOffset,
                       Vec2i targetOffset,
                       Vec2i sourceRect,
                       Vec2i targetRect);

        void blitStencil(FrameGraphResource source,
                         FrameGraphResource target,
                         Vec2i sourceOffset,
                         Vec2i targetOffset,
                         Vec2i sourceRect,
                         Vec2i targetRect);

        void beginPass(const std::vector<FrameGraphAttachment> &colorAttachments,
                       FrameGraphAttachment depthAttachment);

        void beginPass(FrameGraphResource target);

        void finishPass();

        void clearColor(ColorRGBA color);

        void clearDepth(float depth);

        void setViewport(Vec2i viewportOffset, Vec2i viewportSize);

        void bindPipeline(FrameGraphResource pipeline);

        void bindVertexBuffers(FrameGraphResource vertexBuffer,
                               FrameGraphResource indexBuffer,
                               FrameGraphResource instanceBuffer,
                               VertexLayout vertexLayout,
                               VertexLayout instanceArrayLayout);

        void bindShaderResources(const std::vector<FrameGraphCommand::ShaderData> &resources);

        void drawArray(const DrawCall &drawCall);

        void drawIndexed(const DrawCall &drawCall);

        void instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances);

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances);

        void multiDrawArray(const std::vector<DrawCall> &drawCalls);

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls);

        void drawIndexed(const DrawCall &drawCall, size_t baseVertex);

        void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances, size_t baseVertex);

        void multiDrawIndexed(const std::vector<DrawCall> &drawCalls, const std::vector<size_t> &baseVertices);

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
         * Check if a slot has been assigned
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
         * @return The scene containing the user specified data.
         */
        const Scene &getScene() const;

        /**
         * The settings contain static configuration data.
         *
         * @return
         */
        const SceneRendererSettings &getSettings() const;

        const RenderDeviceInfo &getDeviceInfo();

        Vec2i getRenderResolution();

    private:
        FrameGraphResource createResourceId();

        void checkResourceHandle(FrameGraphResource res);

        RenderTargetDesc backBufferDesc;

        const Scene &scene;
        const SceneRendererSettings &settings;

        FrameGraph graph;

        std::vector<FrameGraphCommand> commands;
        std::set<FrameGraphResource> persists;

        std::set<FrameGraphResource> persistentResources;

        size_t resourceCounter = 1;

        RenderDeviceInfo deviceInfo;
    };
}
#endif //XENGINE_FRAMEGRAPHBUILDER_HPP
