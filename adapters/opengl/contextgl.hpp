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

#ifndef XENGINE_CONTEXTGL_HPP
#define XENGINE_CONTEXTGL_HPP

#include "../../engine/include/xng/rendergraph/rendergraphcontext.hpp"

#include "compiledpipeline.hpp"

using namespace xng;

class ContextGL : public RenderGraphContext {
public:
    ContextGL() = default;

    ~ContextGL() override = default;

    explicit ContextGL(const std::unordered_map<RenderGraphResource, CompiledPipeline> &pipelines)
        : pipelines(pipelines) {
    }

    void uploadBuffer(RenderGraphResource buffer, const uint8_t *ptr, size_t size) override;

    void uploadTexture(RenderGraphResource texture, const uint8_t *ptr, size_t size, ColorFormat format, size_t index,
                       size_t mipMapLevel, CubeMapFace face) override;

    void bindPipeline(RenderGraphResource pipeline) override;

    void bindVertexBuffer(RenderGraphResource buffer) override;

    void bindIndexBuffer(RenderGraphResource buffer) override;

    void bindRenderTarget(size_t binding,
                          RenderGraphResource texture,
                          size_t index,
                          size_t mipMapLevel,
                          CubeMapFace face) override;

    void bindTextures(const std::unordered_map<std::string, RenderGraphResource> &textures) override;

    void bindShaderBuffers(const std::unordered_map<std::string, RenderGraphResource> &buffers) override;

    void setShaderParameters(const std::unordered_map<std::string, ShaderLiteral> &parameters) override;

    void draw(const std::vector<DrawCall> &calls) override;

    std::vector<uint8_t> downloadShaderBuffer(RenderGraphResource buffer) override;

    Image<ColorRGBA> downloadTexture(RenderGraphResource texture, size_t index, size_t mipMapLevel, CubeMapFace face) override;

    std::unordered_map<ShaderStage::Type, std::string> getShaderSource(RenderGraphResource shader) override;

private:
    std::unordered_map<RenderGraphResource, CompiledPipeline> pipelines;
};

#endif //XENGINE_CONTEXTGL_HPP
