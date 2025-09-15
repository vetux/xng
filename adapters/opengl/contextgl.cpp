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

#include "contextgl.hpp"

void ContextGL::uploadBuffer(RenderGraphResource target, const uint8_t *buffer, size_t bufferSize,
    size_t targetOffset) {
}

void ContextGL::uploadTexture(RenderGraphResource texture, const uint8_t *buffer, size_t bufferSize,
    ColorFormat bufferFormat, size_t index, CubeMapFace face, size_t mipMapLevel) {
}

void ContextGL::copyBuffer(RenderGraphResource target, RenderGraphResource source, size_t targetOffset,
    size_t sourceOffset, size_t count) {
}

void ContextGL::copyTexture(RenderGraphResource target, RenderGraphResource source) {
}

void ContextGL::copyTexture(RenderGraphResource target, RenderGraphResource source, Vec3i srcOffset, Vec3i dstOffset,
    Vec3i size, size_t srcMipMapLevel, size_t dstMipMapLevel) {
}

void ContextGL::beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
    const RenderGraphAttachment &depthAttachment, const RenderGraphAttachment &stencilAttachment) {
}

void ContextGL::beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
    const RenderGraphAttachment &depthStencilAttachment) {
}

void ContextGL::bindPipeline(RenderGraphResource pipeline) {
}

void ContextGL::bindVertexBuffer(RenderGraphResource buffer) {
}

void ContextGL::bindIndexBuffer(RenderGraphResource buffer) {
}

void ContextGL::bindTextures(const std::vector<std::vector<RenderGraphResource>> &textureArrays) {
}

void ContextGL::bindShaderBuffers(const std::unordered_map<std::string, RenderGraphResource> &buffers) {
}

void ContextGL::setShaderParameters(const std::unordered_map<std::string, ShaderLiteral> &parameters) {
}

void ContextGL::clearColorAttachment(size_t binding, ColorRGBA clearColor) {
}

void ContextGL::clearDepthAttachment(float depth) {
}

void ContextGL::setViewport(Vec2i viewportOffset, Vec2i viewportSize) {
}

void ContextGL::drawArray(const DrawCall &drawCall) {
}

void ContextGL::drawIndexed(const DrawCall &drawCall, size_t indexOffset) {
}

void ContextGL::endRenderPass() {
}

std::vector<uint8_t> ContextGL::downloadShaderBuffer(RenderGraphResource buffer) {
    throw std::runtime_error("DownloadShaderBuffer not implemented");
}

Image<ColorRGBA> ContextGL::downloadTexture(RenderGraphResource texture, size_t index, size_t mipMapLevel,
    CubeMapFace face) {
    throw std::runtime_error("DownloadTexture not implemented");
}

std::unordered_map<ShaderStage::Type, std::string> ContextGL::getShaderSource(RenderGraphResource shader) {
    return pipelines.at(shader).sourceCode;
}
