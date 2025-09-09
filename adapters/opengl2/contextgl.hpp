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

#include "xng/render/graph2/fgcontext.hpp"

#include "compiledpipeline.hpp"

using namespace xng;

class ContextGL : public FGContext {
public:
    ContextGL() = default;

    ~ContextGL() override = default;

    explicit ContextGL(const std::unordered_map<FGResource, CompiledPipeline> &pipelines)
        : pipelines(pipelines) {
    }

    void uploadBuffer(FGResource buffer, const uint8_t *ptr, size_t size) override;

    void uploadTexture(FGResource texture, const uint8_t *ptr, size_t size, FGColorFormat format, size_t index,
                       size_t mipMapLevel, FGCubeMapFace face) override;

    void bindVertexBuffer(FGResource buffer) override;

    void bindIndexBuffer(FGResource buffer) override;

    void bindRenderTarget(size_t binding,
                          FGResource texture,
                          size_t index,
                          size_t mipMapLevel,
                          FGCubeMapFace face) override;

    void bindTextures(const std::unordered_map<std::string, FGResource> &textures) override;

    void bindShaderBuffers(const std::unordered_map<std::string, FGResource> &buffers) override;

    void setShaderParameters(const std::unordered_map<std::string, FGShaderLiteral> &parameters) override;

    void bindShaders(const std::vector<FGResource> &shaders) override;

    void draw(const std::vector<FGDrawCall> &calls) override;

    std::vector<uint8_t> downloadShaderBuffer(FGResource buffer) override;

    Image<ColorRGBA> downloadTexture(FGResource texture, size_t index, size_t mipMapLevel, FGCubeMapFace face) override;

    std::unordered_map<FGShaderSource::ShaderStage, std::string> getShaderSource(FGResource shader) override;

private:
    std::unordered_map<FGResource, CompiledPipeline> pipelines;
};

#endif //XENGINE_CONTEXTGL_HPP
