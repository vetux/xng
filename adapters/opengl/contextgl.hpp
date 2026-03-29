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

#ifndef XENGINE_CONTEXTGL_HPP
#define XENGINE_CONTEXTGL_HPP

#include <utility>

#include "xng/rendergraph/context.hpp"
#include "xng/rendergraph/statistics.hpp"

#include "resource/oglframebuffer.hpp"
#include "resource/oglvertexarrayobject.hpp"

#include "compiledshaderprogram.hpp"
#include "graphresources.hpp"
#include "heapgl.hpp"

using namespace xng;

class ContextGL final : public RasterContext {
public:
    ~ContextGL() override = default;

    explicit ContextGL(std::shared_ptr<OGLTexture> backBufferColor,
                       std::shared_ptr<OGLTexture> backBufferDepthStencil,
                       const GraphResources &res,
                       const HeapGL &heap,
                       Statistics &stats)
        : vertexArray(std::make_shared<OGLVertexArrayObject>()),
          backBufferColor(std::move(backBufferColor)),
          backBufferDepthStencil(std::move(backBufferDepthStencil)),
          emptySSBO(std::make_unique<OGLShaderStorageBuffer>(1)),
          resources(std::move(res)),
          heap(heap),
          stats(stats) {
    }

    void uploadVertexBuffer(const Resource<VertexBuffer> &target,
                            const uint8_t *buffer,
                            size_t bufferSize,
                            size_t targetOffset) override;

    void uploadIndexBuffer(const Resource<IndexBuffer> &target,
                           const uint8_t *buffer,
                           size_t bufferSize,
                           size_t targetOffset) override;

    void uploadStorageBuffer(const Resource<StorageBuffer> &target,
                             const uint8_t *buffer,
                             size_t bufferSize,
                             size_t targetOffset) override;

    void uploadTextureBuffer(const Resource<Texture> &texture,
                             const uint8_t *buffer,
                             size_t bufferSize,
                             ColorFormat bufferFormat,
                             size_t index,
                             CubeMapFace face,
                             size_t mipMapLevel,
                             const Vec2i &size,
                             const Vec2i &offset) override;

    void generateMipMaps(const Resource<Texture> &texture) override;

    void clearTextureColor(const Resource<Texture> &texture,
                           const ColorRGBA &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(const Resource<Texture> &texture,
                           const Vec4f &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(const Resource<Texture> &texture,
                           const Vec4i &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(const Resource<Texture> &texture,
                           const Vec4u &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureDepthStencil(const Resource<Texture> &texture,
                                  float clearDepth,
                                  unsigned clearStencil,
                                  size_t index,
                                  CubeMapFace face,
                                  size_t mipMapLevel) override;

    void copyVertexBuffer(Resource<VertexBuffer> target,
                          Resource<VertexBuffer> source,
                          size_t targetOffset,
                          size_t sourceOffset,
                          size_t count) override;

    void copyIndexBuffer(Resource<IndexBuffer> target,
                         Resource<IndexBuffer> source,
                         size_t targetOffset,
                         size_t sourceOffset,
                         size_t count) override;

    void copyStorageBuffer(Resource<StorageBuffer> target,
                           Resource<StorageBuffer> source,
                           size_t targetOffset,
                           size_t sourceOffset,
                           size_t count) override;

    void copyTextureBuffer(Resource<Texture> target, Resource<Texture> source) override;

    void copyTextureBuffer(Resource<Texture> target,
                           Resource<Texture> source,
                           Vec3i srcOffset,
                           Vec3i dstOffset,
                           Vec3i size,
                           size_t srcMipMapLevel,
                           size_t dstMipMapLevel) override;

    void beginRenderPass(const std::vector<Attachment> &colorAttachments,
                         const Attachment &depthAttachment,
                         const Attachment &stencilAttachment) override;

    void beginRenderPass(const std::vector<Attachment> &colorAttachments,
                         const Attachment &depthStencilAttachment) override;

    void bindPipeline(const Resource<RasterPipeline> &pipeline) override;

    void bindVertexBuffer(const Resource<VertexBuffer> &buffer) override;

    void bindIndexBuffer(const Resource<IndexBuffer> &buffer) override;

    void bindTexture(const std::string &target,
                           const std::vector<Resource<Texture> > &textureArray) override;

    void bindStorageBuffer(const std::string &target,
                           const Resource<StorageBuffer> &buffer,
                           size_t offset,
                           size_t size) override;

    void setShaderParameter(const std::string &name, const ShaderPrimitive &value) override;

    void clearColorAttachment(size_t binding, ColorRGBA clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4i &clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4u &clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4f &clearColor) override;

    void clearDepthStencilAttachment(float clearDepth, unsigned clearStencil) override;

    void clearDepthAttachment(float clearDepth) override;

    void clearStencilAttachment(unsigned clearStencil) override;

    void setViewport(Vec2i viewportOffset, Vec2i viewportSize) override;

    void drawArray(const DrawCall &drawCall) override;

    void drawIndexed(const DrawCall &drawCall, size_t indexOffset) override;

    void endRenderPass() override;

    std::vector<uint8_t> downloadStorageBuffer(const Resource<StorageBuffer> &buffer) override;

    Image<ColorRGBA> downloadTextureBuffer(const Resource<Texture> &texture,
                                           size_t index,
                                           size_t mipMapLevel,
                                           CubeMapFace face) override;

    std::unordered_map<Shader::Stage, std::string> getShaderSource(const Resource<RasterPipeline> &pipeline) override;

private:
    const OGLVertexBuffer &getVertexBuffer(const ResourceId &resource) const;

    const OGLIndexBuffer &getIndexBuffer(const ResourceId &resource) const;

    const OGLShaderStorageBuffer &getStorageBuffer(const ResourceId &resource) const;

    const OGLTexture &getTextureBuffer(const ResourceId &resource) const;

    const OGLShaderProgram &getShaderProgram(const ResourceId &resource) const;

    const ShaderAttributeLayout &getVertexLayout(const ResourceId &resource) const;

    const CompiledShader &getCompiledShader(const ResourceId &resource) const;

    std::vector<Attachment> framebufferColorAttachments{};
    Attachment framebufferDepthStencilAttachment;
    Attachment framebufferDepthAttachment;
    Attachment framebufferStencilAttachment;

    Resource<RasterPipeline> boundPipeline;

    std::shared_ptr<OGLFramebuffer> framebuffer = nullptr;
    std::shared_ptr<OGLVertexArrayObject> vertexArray = nullptr;

    std::shared_ptr<OGLTexture> backBufferColor = nullptr;
    std::shared_ptr<OGLTexture> backBufferDepthStencil = nullptr;

    std::unique_ptr<OGLShaderStorageBuffer> emptySSBO = nullptr;

    const GraphResources &resources;
    const HeapGL &heap;
    Statistics &stats;
};

#endif //XENGINE_CONTEXTGL_HPP
