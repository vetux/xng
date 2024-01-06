#include <utility>

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

#ifndef XENGINE_COMMANDDATA_HPP
#define XENGINE_COMMANDDATA_HPP

namespace xng {
    class IndexBuffer;

    class RenderTarget;

    class RenderPipeline;

    class VertexArrayObject;

    class VertexBuffer;

    class RenderPass;

    class ComputePipeline;

    struct IndexBufferCopy {
        IndexBuffer *source;
        IndexBuffer *target;

        size_t readOffset;
        size_t writeOffset;
        size_t count;

        IndexBufferCopy() = default;

        IndexBufferCopy(IndexBuffer *source,
                        IndexBuffer *target,
                        size_t readOffset,
                        size_t writeOffset,
                        size_t count) : source(source),
                                        target(target),
                                        readOffset(readOffset),
                                        writeOffset(writeOffset),
                                        count(count) {}
    };

    struct RenderTargetBlit {
        RenderTarget *source{};
        RenderTarget *target{};
        Vec2i sourceOffset;
        Vec2i targetOffset;
        Vec2i sourceRect;
        Vec2i targetRect;
        TextureFiltering filter{};
        int sourceIndex{};
        int targetIndex{};

        RenderTargetBlit() = default;

        RenderTargetBlit(RenderTarget *source,
                         RenderTarget *target,
                         Vec2i sourceOffset,
                         Vec2i targetOffset,
                         Vec2i sourceRect,
                         Vec2i targetRect,
                         TextureFiltering filter,
                         int sourceIndex,
                         int targetIndex) : source(source),
                                            target(target),
                                            sourceOffset(std::move(sourceOffset)),
                                            targetOffset(std::move(targetOffset)),
                                            sourceRect(std::move(sourceRect)),
                                            targetRect(std::move(targetRect)),
                                            filter(filter),
                                            sourceIndex(sourceIndex),
                                            targetIndex(targetIndex) {}
    };

    struct RenderPassBegin {
        RenderPass *pass{};
        RenderTarget *target{};

        RenderPassBegin() = default;

        RenderPassBegin(RenderPass *pass, RenderTarget *target) : pass(pass), target(target) {}
    };

    struct RenderPassClear {
        ColorRGBA color;
        float depth{};

        RenderPassClear() = default;

        RenderPassClear(const ColorRGBA &color, float depth) : color(color), depth(depth) {}
    };

    struct RenderPassViewport {
        Vec2i viewportOffset;
        Vec2i viewportSize;

        RenderPassViewport() = default;

        RenderPassViewport(Vec2i viewportOffset, Vec2i viewportSize) : viewportOffset(std::move(viewportOffset)),
                                                                       viewportSize(std::move(viewportSize)) {}
    };

    struct RenderPassDraw {
        std::vector<DrawCall> drawCalls;
        size_t numberOfInstances{};
        std::vector<size_t> baseVertices;

        RenderPassDraw() = default;

        RenderPassDraw(std::vector<DrawCall> drawCalls,
                       size_t numberOfInstances,
                       std::vector<size_t> baseVertices) : drawCalls(std::move(drawCalls)),
                                                           numberOfInstances(numberOfInstances),
                                                           baseVertices(std::move(baseVertices)) {}
    };

    struct RenderPipelineBind {
        RenderPipeline *pipeline;

        RenderPipelineBind() = default;


        explicit RenderPipelineBind(RenderPipeline *pipeline) : pipeline(pipeline) {}
    };

    struct ShaderResourceBind {
        std::vector<ShaderResource> resources;

        ShaderResourceBind() = default;

        explicit ShaderResourceBind(std::vector<ShaderResource> resources) : resources(std::move(resources)) {}
    };

    struct TextureArrayBufferCopy {
        TextureArrayBuffer *source;
        TextureArrayBuffer *target;

        TextureArrayBufferCopy() = default;

        TextureArrayBufferCopy(TextureArrayBuffer *source, TextureArrayBuffer *target) : source(source),
                                                                                         target(target) {}
    };

    struct TextureBufferCopy {
        TextureBuffer *source;
        TextureBuffer *target;

        TextureBufferCopy() = default;

        TextureBufferCopy(TextureBuffer *source, TextureBuffer *target) : source(source), target(target) {}
    };

    struct VertexArrayObjectBind {
        VertexArrayObject *target;

        VertexArrayObjectBind() = default;

        explicit VertexArrayObjectBind(VertexArrayObject *target) : target(target) {}
    };

    struct VertexBufferCopy {
        VertexBuffer *source;
        VertexBuffer *target;
        size_t readOffset;
        size_t writeOffset;
        size_t count;

        VertexBufferCopy() = default;

        VertexBufferCopy(VertexBuffer *source, VertexBuffer *target, size_t readOffset, size_t writeOffset,
                         size_t count) : source(source), target(target), readOffset(readOffset),
                                         writeOffset(writeOffset), count(count) {}
    };

    struct ShaderStorageBufferCopy {
        ShaderStorageBuffer *source;
        ShaderStorageBuffer *target;
        size_t readOffset;
        size_t writeOffset;
        size_t count;

        ShaderStorageBufferCopy() = default;

        ShaderStorageBufferCopy(ShaderStorageBuffer *source, ShaderStorageBuffer *target, size_t readOffset,
                                size_t writeOffset, size_t count) : source(source), target(target),
                                                                    readOffset(readOffset),
                                                                    writeOffset(writeOffset), count(count) {}
    };

    struct ShaderUniformBufferCopy {
        ShaderUniformBuffer *source;
        ShaderUniformBuffer *target;
        size_t readOffset;
        size_t writeOffset;
        size_t count;

        ShaderUniformBufferCopy() = default;

        ShaderUniformBufferCopy(ShaderUniformBuffer *source, ShaderUniformBuffer *target, size_t readOffset,
                                size_t writeOffset, size_t count) : source(source), target(target),
                                                                    readOffset(readOffset),
                                                                    writeOffset(writeOffset), count(count) {}

    };

    struct ComputePipelineBind {
        ComputePipeline *pipeline;

        ComputePipelineBind() = default;

        explicit ComputePipelineBind(ComputePipeline *pipeline) : pipeline(pipeline) {}

    };

    struct ComputePipelineExecute {
        Vector3<unsigned int> num_groups;

        ComputePipelineExecute() = default;

        explicit ComputePipelineExecute(const Vector3<unsigned int> &numGroups) : num_groups(numGroups) {}
    };

    struct DebugGroup {
        std::string name;

        DebugGroup() = default;

        explicit DebugGroup(std::string name) : name(std::move(name)) {}
    };

    typedef std::variant<IndexBufferCopy,
            RenderTargetBlit,
            RenderPassBegin,
            RenderPassClear,
            RenderPassViewport,
            RenderPassDraw,
            RenderPipelineBind,
            ShaderResourceBind,
            TextureArrayBufferCopy,
            TextureBufferCopy,
            VertexArrayObjectBind,
            VertexBufferCopy,
            ShaderStorageBufferCopy,
            ShaderUniformBufferCopy,
            ComputePipelineBind,
            ComputePipelineExecute,
            DebugGroup> CommandData;
}

#endif //XENGINE_COMMANDDATA_HPP
