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

#include "xng/render/graph/framegraphbuilder.hpp"

#include <utility>

#include "xng/render/graph/framegraphpass.hpp"

#include "xng/render/graph/framegraphsettings.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTargetDesc backBuffer,
                                         RenderDeviceInfo deviceInfo,
                                         const Scene &scene,
                                         const SceneRendererSettings &settings,
                                         std::set<FrameGraphResource> persistentResources)
            : backBufferDesc(std::move(backBuffer)),
              deviceInfo(std::move(deviceInfo)),
              scene(scene),
              settings(settings),
              persistentResources(std::move(persistentResources)) {}

    FrameGraphResource FrameGraphBuilder::createRenderPipeline(const RenderPipelineDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_RENDER_PIPELINE;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_TEXTURE;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureArrayBuffer(const TextureArrayBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_TEXTURE_ARRAY;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createVertexBuffer(const VertexBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_VERTEX_BUFFER;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createIndexBuffer(const IndexBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_INDEX_BUFFER;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_SHADER_UNIFORM_BUFFER;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) {
        auto ret = createResourceId();
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CREATE_SHADER_STORAGE_BUFFER;
        cmd.data = desc;
        cmd.resources.emplace_back(ret);
        commands.emplace_back(cmd);
        return ret;
    }

    void FrameGraphBuilder::upload(FrameGraphResource buffer,
                                   size_t index,
                                   size_t offset,
                                   ColorFormat colorFormat,
                                   CubeMapFace cubeMapFace,
                                   std::function<FrameGraphUploadBuffer()> dataSource) {
        if (!buffer.assigned)
            throw std::runtime_error("Unassigned resource");
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::UPLOAD;
        cmd.resources.emplace_back(buffer);
        cmd.data = FrameGraphCommand::UploadData{index, offset, colorFormat, cubeMapFace, std::move(dataSource)};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::copy(FrameGraphResource source,
                                 FrameGraphResource dest,
                                 size_t readOffset,
                                 size_t writeOffset,
                                 size_t count) {
        if (!source.assigned)
            throw std::runtime_error("Unassigned resource");
        if (!dest.assigned)
            throw std::runtime_error("Unassigned resource");
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::COPY;
        cmd.resources.emplace_back(source);
        cmd.resources.emplace_back(dest);
        cmd.data = FrameGraphCommand::CopyData{readOffset, writeOffset, count};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::blitColor(FrameGraphResource source,
                                      FrameGraphResource target,
                                      Vec2i sourceOffset,
                                      Vec2i targetOffset,
                                      Vec2i sourceRect,
                                      Vec2i targetRect,
                                      TextureFiltering filter,
                                      int sourceIndex,
                                      int targetIndex) {
        if (!source.assigned)
            throw std::runtime_error("Unassigned resource");
        if (!target.assigned)
            throw std::runtime_error("Unassigned resource");
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BLIT_COLOR;
        cmd.resources.emplace_back(source);
        cmd.resources.emplace_back(target);
        cmd.data = FrameGraphCommand::BlitData{std::move(sourceOffset),
                                               std::move(targetOffset),
                                               std::move(sourceRect),
                                               std::move(targetRect),
                                               filter,
                                               sourceIndex,
                                               targetIndex};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::blitDepth(FrameGraphResource source,
                                      FrameGraphResource target,
                                      Vec2i sourceOffset,
                                      Vec2i targetOffset,
                                      Vec2i sourceRect,
                                      Vec2i targetRect) {
        if (!source.assigned)
            throw std::runtime_error("Unassigned resource");
        if (!target.assigned)
            throw std::runtime_error("Unassigned resource");

        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BLIT_DEPTH;
        cmd.resources.emplace_back(source);
        cmd.resources.emplace_back(target);
        cmd.data = FrameGraphCommand::BlitData{std::move(sourceOffset),
                                               std::move(targetOffset),
                                               std::move(sourceRect),
                                               std::move(targetRect),
                                               TextureFiltering::LINEAR,
                                               0,
                                               0};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::blitStencil(FrameGraphResource source,
                                        FrameGraphResource target,
                                        Vec2i sourceOffset,
                                        Vec2i targetOffset,
                                        Vec2i sourceRect,
                                        Vec2i targetRect) {
        if (!source.assigned)
            throw std::runtime_error("Unassigned resource");
        if (!target.assigned)
            throw std::runtime_error("Unassigned resource");

        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BLIT_STENCIL;
        cmd.resources.emplace_back(source);
        cmd.resources.emplace_back(target);
        cmd.data = FrameGraphCommand::BlitData{std::move(sourceOffset),
                                               std::move(targetOffset),
                                               std::move(sourceRect),
                                               std::move(targetRect),
                                               TextureFiltering::LINEAR,
                                               0,
                                               0};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::beginPass(const std::vector<FrameGraphAttachment> &colorAttachments,
                                      FrameGraphAttachment depthAttachment) {
        for (auto &r: colorAttachments) {
            if (!r.resource.assigned)
                throw std::runtime_error("Unassigned resource");
        }

        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BEGIN_PASS;
        cmd.data = FrameGraphCommand::BeginPassData{colorAttachments, depthAttachment};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::beginPass(FrameGraphResource target) {
        if (target != FrameGraphResource(0))
            throw std::runtime_error("Invalid render target resource (Must be backbuffer)");
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BEGIN_PASS;
        cmd.resources.emplace_back(target);
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::finishPass() {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::FINISH_PASS;
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::clearColor(ColorRGBA color) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CLEAR_COLOR;
        cmd.data = FrameGraphCommand::ClearData{color, {}};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::clearDepth(float depth) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::CLEAR_DEPTH;
        cmd.data = FrameGraphCommand::ClearData{{}, depth};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::setViewport(Vec2i viewportOffset, Vec2i viewportSize) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::SET_VIEWPORT;
        cmd.data = FrameGraphCommand::ViewportData{std::move(viewportOffset), std::move(viewportSize)};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::bindPipeline(FrameGraphResource pipeline) {
        if (!pipeline.assigned)
            throw std::runtime_error("Unassigned resource");

        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BIND_PIPELINE;
        cmd.resources.emplace_back(pipeline);
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::bindVertexBuffers(FrameGraphResource vertexBuffer,
                                              FrameGraphResource indexBuffer,
                                              FrameGraphResource instanceBuffer,
                                              VertexLayout vertexLayout,
                                              VertexLayout instanceArrayLayout) {
        if (!vertexBuffer.assigned)
            throw std::runtime_error("Unassigned resource");
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BIND_VERTEX_BUFFERS;
        cmd.resources.emplace_back(vertexBuffer);
        cmd.resources.emplace_back(indexBuffer);
        cmd.resources.emplace_back(instanceBuffer);
        cmd.data = FrameGraphCommand::BindVertexData{std::move(vertexLayout), std::move(instanceArrayLayout)};
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::bindShaderResources(const std::vector<FrameGraphCommand::ShaderData> &resources) {
        for (auto &r: resources) {
            if (!r.resource.assigned)
                throw std::runtime_error("Unassigned resource");
        }
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::BIND_SHADER_RESOURCES;
        cmd.data = resources;
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::drawArray(const DrawCall &drawCall) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_ARRAY;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {},
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::drawIndexed(const DrawCall &drawCall) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_INDEXED;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {},
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_INSTANCED_ARRAY;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {},
                numberOfInstances
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_INSTANCED_INDEXED;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {},
                numberOfInstances
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::multiDrawArray(const std::vector<DrawCall> &drawCalls) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_MULTI_ARRAY;
        cmd.data = FrameGraphCommand::DrawCallData{
                drawCalls,
                {},
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::multiDrawIndexed(const std::vector<DrawCall> &drawCalls) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_MULTI_INDEXED;
        cmd.data = FrameGraphCommand::DrawCallData{
                drawCalls,
                {},
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::drawIndexed(const DrawCall &drawCall, size_t baseVertex) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_INDEXED_BASE_VERTEX;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {baseVertex},
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::instancedDrawIndexed(const DrawCall &drawCall,
                                                 size_t numberOfInstances,
                                                 size_t baseVertex) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_INSTANCED_INDEXED_BASE_VERTEX;
        cmd.data = FrameGraphCommand::DrawCallData{
                {drawCall},
                {baseVertex},
                numberOfInstances
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::multiDrawIndexed(const std::vector<DrawCall> &drawCalls,
                                             const std::vector<size_t> &baseVertices) {
        auto cmd = FrameGraphCommand();
        cmd.type = FrameGraphCommand::DRAW_MULTI_INDEXED_BASE_VERTEX;
        cmd.data = FrameGraphCommand::DrawCallData{
                drawCalls,
                baseVertices,
                0
        };
        commands.emplace_back(cmd);
    }

    void FrameGraphBuilder::persist(FrameGraphResource resource) {
        checkResourceHandle(resource);
        persists.insert(resource);
    }

    void FrameGraphBuilder::assignSlot(FrameGraphSlot slot, FrameGraphResource resource) {
        if (graph.slots.find(slot) != graph.slots.end()) {
            throw std::runtime_error("Slot " + std::to_string(slot) + " already assigned.");
        }
        graph.slots[slot] = resource;
    }

    FrameGraphResource FrameGraphBuilder::getSlot(FrameGraphSlot slot) {
        return graph.slots.at(slot);
    }

    bool FrameGraphBuilder::checkSlot(FrameGraphSlot slot) {
        return graph.slots.find(slot) != graph.slots.end();
    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    RenderTargetDesc FrameGraphBuilder::getBackBufferDescription() {
        return backBufferDesc;
    }

    const Scene &FrameGraphBuilder::getScene() const {
        return scene;
    }

    const SceneRendererSettings &FrameGraphBuilder::getSettings() const {
        return settings;
    }

    const RenderDeviceInfo &FrameGraphBuilder::getDeviceInfo() {
        return deviceInfo;
    }

    Vec2i FrameGraphBuilder::getRenderResolution() {
        return getBackBufferDescription().size * getSettings().get<float>(FrameGraphSettings::SETTING_RENDER_SCALE);
    }

    FrameGraphResource FrameGraphBuilder::createResourceId() {
        if (resourceCounter >= std::numeric_limits<size_t>::max()) {
            throw std::runtime_error("Resource id counter overflow");
        }
        auto ret = resourceCounter++;
        while (persistentResources.find(FrameGraphResource(ret)) != persistentResources.end()) {
            if (resourceCounter >= std::numeric_limits<size_t>::max()) {
                throw std::runtime_error("Resource id counter overflow");
            }
            ret = resourceCounter++;
        }
        return FrameGraphResource(ret);
    }

    void FrameGraphBuilder::checkResourceHandle(FrameGraphResource res) {
        if (!res.assigned)
            throw std::runtime_error("Unassigned resource");
        if (!graph.checkResource(res)) {
            bool found = false;
            for (auto &c: commands) {
                if (std::find(c.resources.begin(), c.resources.end(), res) != c.resources.end()) {
                    found = true;
                    break;
                }
            }
            if (!found && persistentResources.find(res) == persistentResources.end())
                throw std::runtime_error("Unknown resource");
        }
    }

    FrameGraph FrameGraphBuilder::build(const std::vector<std::shared_ptr<FrameGraphPass>> &passes) {
        graph = {};
        graph.backBuffer = FrameGraphResource(0);

        resourceCounter = 1;

        for (auto &pass: passes) {
            commands = {};
            persists = {};
            pass->setup(*this);
            FrameGraphContext context;
            context.commands = commands;
            context.persists = persists;
            context.pass = pass->getTypeIndex();
            graph.contexts.emplace_back(context);
        }

        return graph;
    }
}