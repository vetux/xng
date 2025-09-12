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

void ContextGL::uploadBuffer(RenderGraphResource buffer, const uint8_t *ptr, size_t size) {
}

void ContextGL::uploadTexture(RenderGraphResource texture, const uint8_t *ptr, size_t size, ColorFormat format, size_t index,
                              size_t mipMapLevel, CubeMapFace face) {
}

void ContextGL::bindPipeline(RenderGraphResource pipeline) {
}

void ContextGL::bindVertexBuffer(RenderGraphResource buffer) {
}

void ContextGL::bindIndexBuffer(RenderGraphResource buffer) {
}

void ContextGL::bindRenderTarget(size_t binding,
                                 RenderGraphResource texture,
                                 size_t index,
                                 size_t mipMapLevel,
                                 CubeMapFace face) {
}

void ContextGL::bindTextures(const std::unordered_map<std::string, RenderGraphResource> &textures) {
}

void ContextGL::bindShaderBuffers(const std::unordered_map<std::string, RenderGraphResource> &buffers) {
}

void ContextGL::setShaderParameters(const std::unordered_map<std::string, ShaderLiteral> &parameters) {
}

void ContextGL::draw(const std::vector<DrawCall> &calls) {
}

std::vector<uint8_t> ContextGL::downloadShaderBuffer(RenderGraphResource buffer) {
    throw std::runtime_error("Not implemented");
}

Image<ColorRGBA> ContextGL::downloadTexture(RenderGraphResource texture, size_t index, size_t mipMapLevel, CubeMapFace face) {
    throw std::runtime_error("Not implemented");
}

std::unordered_map<ShaderStage::Type, std::string> ContextGL::getShaderSource(RenderGraphResource shader) {
    return pipelines.at(shader).sourceCode;
}
