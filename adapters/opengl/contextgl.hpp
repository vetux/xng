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

#include "xng/rendergraph/rendergraphcontext.hpp"

#include "ogl/oglframebuffer.hpp"
#include "ogl/oglvertexarrayobject.hpp"

#include "compiledpipeline.hpp"
#include "graphresources.hpp"
#include "xng/rendergraph/rendergraphstatistics.hpp"

using namespace xng;

class ContextGL final : public RenderGraphContext {
public:
    ~ContextGL() override = default;

    explicit ContextGL(std::shared_ptr<OGLTexture> backBufferColor,
                       std::shared_ptr<OGLTexture> backBufferDepthStencil,
                       const GraphResources& res,
                       RenderGraphStatistics &stats)
        : vertexArray(std::make_shared<OGLVertexArrayObject>()),
          backBufferColor(std::move(backBufferColor)),
          backBufferDepthStencil(std::move(backBufferDepthStencil)),
          resources(std::move(res)),
          stats(stats) {
    }

    void uploadBuffer(RenderGraphResource target,
                      const uint8_t *buffer,
                      size_t bufferSize,
                      size_t targetOffset) override;

    void uploadTexture(RenderGraphResource texture,
                       const uint8_t *buffer,
                       size_t bufferSize,
                       ColorFormat bufferFormat,
                       size_t index,
                       CubeMapFace face,
                       size_t mipMapLevel) override;

    void generateMipMaps(RenderGraphResource texture) override;

    void clearTextureColor(RenderGraphResource texture,
                           const ColorRGBA &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(RenderGraphResource texture,
                           const Vec4f &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(RenderGraphResource texture,
                           const Vec4i &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureColor(RenderGraphResource texture,
                           const Vec4u &clearColor,
                           size_t index,
                           CubeMapFace face,
                           size_t mipMapLevel) override;

    void clearTextureDepthStencil(RenderGraphResource texture,
                                  float clearDepth,
                                  unsigned int clearStencil,
                                  size_t index,
                                  CubeMapFace face,
                                  size_t mipMapLevel) override;

    void copyBuffer(RenderGraphResource target,
                    RenderGraphResource source,
                    size_t targetOffset,
                    size_t sourceOffset,
                    size_t count) override;

    void copyTexture(RenderGraphResource target, RenderGraphResource source) override;

    void copyTexture(RenderGraphResource target,
                     RenderGraphResource source,
                     Vec3i srcOffset,
                     Vec3i dstOffset,
                     Vec3i size,
                     size_t srcMipMapLevel,
                     size_t dstMipMapLevel) override;

    void beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                         const RenderGraphAttachment &depthAttachment,
                         const RenderGraphAttachment &stencilAttachment) override;

    void beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                         const RenderGraphAttachment &depthStencilAttachment) override;

    void bindPipeline(RenderGraphResource pipeline) override;

    void bindVertexBuffer(RenderGraphResource buffer) override;

    void bindIndexBuffer(RenderGraphResource buffer) override;

    void bindTexture(const std::string &target, const std::vector<RenderGraphResource> &textureArray) override;

    void bindShaderBuffer(const std::string &target, RenderGraphResource buffer, size_t offset, size_t size) override;

    void setShaderParameter(const std::string &name, const ShaderLiteral &value) override;

    void clearColorAttachment(size_t binding, ColorRGBA clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4i &clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4u &clearColor) override;

    void clearColorAttachment(size_t binding, const Vec4f &clearColor) override;

    void clearDepthStencilAttachment(float clearDepth, unsigned int clearStencil) override;

    void clearDepthAttachment(float clearDepth) override;

    void clearStencilAttachment(unsigned int clearStencil) override;

    void setViewport(Vec2i viewportOffset, Vec2i viewportSize) override;

    void drawArray(const DrawCall &drawCall) override;

    void drawIndexed(const DrawCall &drawCall, size_t indexOffset) override;

    void endRenderPass() override;

    std::vector<uint8_t> downloadShaderBuffer(RenderGraphResource buffer) override;

    Image<ColorRGBA> downloadTexture(RenderGraphResource texture,
                                     size_t index,
                                     size_t mipMapLevel,
                                     CubeMapFace face) override;

    std::unordered_map<Shader::Stage, std::string> getShaderSource(RenderGraphResource pipeline) override;

private:
    const OGLTexture &getTexture(RenderGraphResource resource) const;

    std::vector<RenderGraphAttachment> framebufferColorAttachments;
    RenderGraphAttachment framebufferDepthStencilAttachment;
    RenderGraphAttachment framebufferDepthAttachment;
    RenderGraphAttachment framebufferStencilAttachment;

    RenderGraphResource boundPipeline;

    std::shared_ptr<OGLFramebuffer> framebuffer = nullptr;
    std::shared_ptr<OGLVertexArrayObject> vertexArray = nullptr;

    std::shared_ptr<OGLTexture> backBufferColor = nullptr;
    std::shared_ptr<OGLTexture> backBufferDepthStencil = nullptr;

    const GraphResources &resources;
    RenderGraphStatistics &stats;
};

#endif //XENGINE_CONTEXTGL_HPP
