/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "xng/asset/camera.hpp"
#include "xng/render/2d/renderer2d.hpp"

#include <utility>

#include "xng/math/matrixmath.hpp"
#include "xng/asset/shadersource.hpp"

#include "xng/geometry/vertexstream.hpp"

static const char *SHADER_VERT_TEXTURE = R"###(#version 460

#define MAX_DRAW 1000 // Maximum amount of draws per multi draw

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;
layout (location = 2) flat out uint drawID;

struct PassData {
    mat4 mvp;
    vec4 color;
    float colorMixFactor;
    int texAtlasLevel;
    int texIndex;
    vec2 uvOffset;
    vec2 uvScale;
    vec2 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passes[MAX_DRAW];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    fPosition = (vars.passes[gl_DrawID].mvp) * vec4(position, 0, 1);
    fUv = uv;
    drawID = gl_DrawID;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG_TEXTURE = R"###(#version 460

#define MAX_DRAW 1000 // Maximum amount of draws per multi draw

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;
layout (location = 2) flat in uint drawID;

layout (location = 0) out vec4 color;

struct PassData {
    mat4 mvp;
    vec4 color;
    float colorMixFactor;
    int texAtlasLevel;
    int texIndex;
    vec2 uvOffset;
    vec2 uvScale;
    vec2 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passes[MAX_DRAW];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.passes[drawID].texIndex >= 0) {
        vec2 uv = fUv * vars.passes[drawID].uvScale;
        uv = uv + vars.passes[drawID].uvOffset;
        uv = uv * vars.passes[drawID].atlasScale;
        color = texture(atlasTextures[vars.passes[drawID].texAtlasLevel], vec3(uv.x, uv.y, vars.passes[drawID].texIndex));
        color = mix(color, vars.passes[drawID].color, vars.passes[drawID].colorMixFactor);
    } else {
        color = vars.passes[drawID].color;
    }
}
)###";

static float distance(float val1, float val2) {
    float abs = val1 - val2;
    if (abs < 0)
        return abs * -1;
    else
        return abs;
}

namespace xng {
    static const int MAX_DRAW = 1000;

    struct PassData {
        Mat4f mvp;
        std::array<float, 4> color;
        float colorMixFactor = 0;
        int texAtlasLevel = -1;
        int texIndex = -1;
        std::array<float, 2> uvOffset;
        std::array<float, 2> uvScale;
        std::array<float, 2> atlasScale;
    };

    struct ShaderUniformBuffer {
        std::array<PassData, MAX_DRAW> passes;
    };

    Renderer2D::Renderer2D(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &shaderDecompiler)
            : renderDevice(device) {
        vertexLayout.emplace_back(VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT));
        vertexLayout.emplace_back(VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT));

        vsTexture = ShaderSource(SHADER_VERT_TEXTURE, "main", VERTEX, GLSL_460, false);
        fsTexture = ShaderSource(SHADER_FRAG_TEXTURE, "main", FRAGMENT, GLSL_460, false);

        vsTexture = vsTexture.preprocess(shaderCompiler);
        fsTexture = fsTexture.preprocess(shaderCompiler);

        auto vsTexSource = vsTexture.compile(shaderCompiler);
        auto fsTexSource = fsTexture.compile(shaderCompiler);

        RenderPipelineDesc desc;

        clearPipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc = {};
        desc.shaders = {
                {ShaderStage::VERTEX,   vsTexSource.getShader()},
                {ShaderStage::FRAGMENT, fsTexSource.getShader()}
        };
        desc.bindings = {
                RenderPipelineBindingType::BIND_SHADER_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
        };

        desc.vertexLayout = vertexLayout;

        texturePipeline = device.createRenderPipeline(desc, shaderDecompiler);

        for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
            auto res = (TextureAtlasResolution) i;
            TextureArrayBufferDesc atlasDesc;
            atlasDesc.textureDesc.size = TextureAtlas::getResolutionLevelSize(res);
            atlasTextures[res] = std::move(device.createTextureArrayBuffer(atlasDesc));
        }

        rebindTextureAtlas({});
    }

    Renderer2D::~Renderer2D() = default;

    TextureAtlasHandle Renderer2D::createTexture(const ImageRGBA &texture) {
        auto res = TextureAtlas::getClosestMatchingResolutionLevel(texture.getSize());
        auto free = atlas.getFreeSlotCount(res);
        if (free < 1) {
            auto desc = atlasTextures.at(res)->getDescription();
            desc.textureCount += 1;
            auto buffer = renderDevice.createTextureArrayBuffer(desc);
            buffer->copy(*atlasTextures.at(res));
            atlasTextures.at(res) = std::move(buffer);
            auto occupations = atlas.getBufferOccupations();
            occupations[res].resize(desc.textureCount, false);
            rebindTextureAtlas(occupations);
        }
        return atlas.add(texture);
    }

    std::vector<TextureAtlasHandle> Renderer2D::createTextures(const std::vector<ImageRGBA> &textures) {
        std::map<TextureAtlasResolution, size_t> resolutionCounts;
        for (auto &img: textures) {
            auto res = TextureAtlas::getClosestMatchingResolutionLevel(img.getSize());
            resolutionCounts[res]++;
        }
        for (auto &pair: resolutionCounts) {
            auto free = atlas.getFreeSlotCount(pair.first);
            if (free < pair.second) {
                auto desc = atlasTextures.at(pair.first)->getDescription();
                desc.textureCount += pair.second - free;
                auto buffer = renderDevice.createTextureArrayBuffer(desc);
                buffer->copy(*atlasTextures.at(pair.first));
                atlasTextures.at(pair.first) = std::move(buffer);
                auto occupations = atlas.getBufferOccupations();
                occupations[pair.first].resize(desc.textureCount, false);
                rebindTextureAtlas(occupations);
            }
        }
        std::vector<TextureAtlasHandle> ret;
        ret.reserve(textures.size());
        for (auto &img: textures) {
            ret.emplace_back(atlas.add(img));
        }
        return ret;
    }

    void Renderer2D::destroyTexture(const TextureAtlasHandle &handle) {
        atlas.remove(handle);
    }

    void Renderer2D::renderBegin(RenderTarget &target,
                                 bool clear,
                                 ColorRGBA clearColor,
                                 const Vec2i &viewportOffset,
                                 const Vec2i &viewportSize,
                                 const Vec2f &cameraPosition,
                                 const Rectf &projection) {
        if (isRendering) {
            throw std::runtime_error("Already Rendering (Nested Renderer2D::renderBegin calls?)");
        }
        isRendering = true;
        mViewportOffset = viewportOffset;
        mViewportSize = viewportSize;

        camera.type = ORTHOGRAPHIC;
        camera.left = projection.position.x;
        camera.right = projection.dimensions.x;
        camera.top = projection.position.y;
        camera.bottom = projection.dimensions.y;
        cameraTransform.setPosition({cameraPosition.x, cameraPosition.y, 1});

        if (clear) {
            clearPipeline->renderBegin(target, viewportOffset, viewportSize);
            clearPipeline->clearColorAttachments(clearColor);
            clearPipeline->clearDepthAttachments(1);
            clearPipeline->renderPresent();
        }

        userTarget = &target;
    }

    void Renderer2D::renderPresent() {
        if (!isRendering) {
            throw std::runtime_error("Not Rendering");
        }
        isRendering = false;

        int drawCycles = 0;
        if (passes.size() > MAX_DRAW) {
            drawCycles = passes.size() / MAX_DRAW;
            if (drawCycles * MAX_DRAW < passes.size()) {
                drawCycles += 1;
            }
        } else {
            drawCycles = 1;
        }

        for (int i = 0; i < drawCycles; i++) {
            std::vector<RenderPipeline::DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            VertexStream vertexStream;
            std::vector<unsigned int> indices;
            ShaderUniformBuffer uniformBuffer;

            size_t baseVertex = 0;
            size_t indexBufferOffset = 0;

            auto currentPassBase = i * MAX_DRAW;
            for (auto passIndex = 0; passIndex < MAX_DRAW && passIndex + currentPassBase < passes.size(); passIndex++) {
                auto &pass = passes.at(passIndex + currentPassBase);
                switch (pass.type) {
                    case Pass::COLOR_POINT:
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f())
                                                       .addVec2(Vec2f())
                                                       .build());
                        indices.emplace_back(0);
                        indices.emplace_back(0);
                        indices.emplace_back(0);
                        drawCalls.emplace_back(RenderPipeline::DrawCall{
                                .offset = indexBufferOffset,
                                .count = 3});
                        baseVertices.emplace_back(baseVertex);
                        baseVertex += 1;
                        indexBufferOffset += 3;
                        uniformBuffer.passes.at(passIndex).mvp = MatrixMath::translate({
                                                                                               pass.center.x,
                                                                                               pass.center.y,
                                                                                               0})
                                                                 * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                                                 * MatrixMath::translate({
                                                                                                 -pass.center.x,
                                                                                                 -pass.center.y,
                                                                                                 0})
                                                                 * MatrixMath::translate({
                                                                                                 pass.dstRect.position.x,
                                                                                                 pass.dstRect.position.y,
                                                                                                 0})
                                                                 * Camera::view(cameraTransform)
                                                                 * camera.projection();
                        uniformBuffer.passes.at(passIndex).color = pass.color.divide().getMemory();
                        break;
                    case Pass::COLOR_LINE:
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(pass.dstRect.position)
                                                       .addVec2(Vec2f())
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(pass.dstRect.dimensions)
                                                       .addVec2(Vec2f())
                                                       .build());
                        indices.emplace_back(0);
                        indices.emplace_back(1);
                        indices.emplace_back(0);
                        drawCalls.emplace_back(RenderPipeline::DrawCall{
                                .offset = indexBufferOffset,
                                .count = 3});
                        baseVertices.emplace_back(baseVertex);
                        baseVertex += 2;
                        indexBufferOffset += 3;
                        uniformBuffer.passes.at(passIndex).mvp = MatrixMath::translate({
                                                                                               pass.center.x,
                                                                                               pass.center.y,
                                                                                               0})
                                                                 * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                                                 * MatrixMath::translate({
                                                                                                 -pass.center.x,
                                                                                                 -pass.center.y,
                                                                                                 0})
                                                                 * Camera::view(cameraTransform)
                                                                 * camera.projection();
                        uniformBuffer.passes.at(passIndex).color = pass.color.divide().getMemory();
                        break;
                    case Pass::COLOR_PLANE:
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, 0))
                                                       .addVec2(Vec2f(0, 0))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x, 0))
                                                       .addVec2(Vec2f(1, 0))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f(0, 1))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x,
                                                                      pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f(1, 1))
                                                       .build());

                        baseVertices.emplace_back(baseVertex);
                        baseVertex += 4;

                        if (pass.fill) {
                            indices.emplace_back(0);
                            indices.emplace_back(1);
                            indices.emplace_back(2);

                            indices.emplace_back(1);
                            indices.emplace_back(2);
                            indices.emplace_back(3);

                            drawCalls.emplace_back(RenderPipeline::DrawCall{
                                    .offset = indexBufferOffset,
                                    .count = 6});
                            indexBufferOffset += 6;
                        } else {
                            indices.emplace_back(0);
                            indices.emplace_back(1);
                            indices.emplace_back(0);

                            indices.emplace_back(1);
                            indices.emplace_back(3);
                            indices.emplace_back(1);

                            indices.emplace_back(3);
                            indices.emplace_back(2);
                            indices.emplace_back(3);

                            indices.emplace_back(0);
                            indices.emplace_back(2);
                            indices.emplace_back(0);

                            drawCalls.emplace_back(RenderPipeline::DrawCall{
                                    .offset = indexBufferOffset,
                                    .count = 12});
                            indexBufferOffset += 12;
                        }

                        uniformBuffer.passes.at(passIndex).mvp = MatrixMath::translate({
                                                                                               pass.center.x,
                                                                                               pass.center.y,
                                                                                               0})
                                                                 * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                                                 * MatrixMath::translate({
                                                                                                 -pass.center.x,
                                                                                                 -pass.center.y,
                                                                                                 0})
                                                                 * MatrixMath::translate({
                                                                                                 pass.dstRect.position.x,
                                                                                                 pass.dstRect.position.y,
                                                                                                 0})
                                                                 * Camera::view(cameraTransform)
                                                                 * camera.projection();
                        uniformBuffer.passes.at(passIndex).color = pass.color.divide().getMemory();

                        break;
                    case Pass::TEXTURE:
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, 0))
                                                       .addVec2(Vec2f(0, 0))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x, 0))
                                                       .addVec2(Vec2f(1, 0))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f(0, 1))
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x,
                                                                      pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f(1, 1))
                                                       .build());

                        baseVertices.emplace_back(baseVertex);
                        baseVertex += 4;

                        indices.emplace_back(0);
                        indices.emplace_back(1);
                        indices.emplace_back(2);

                        indices.emplace_back(1);
                        indices.emplace_back(2);
                        indices.emplace_back(3);

                        drawCalls.emplace_back(RenderPipeline::DrawCall{
                                .offset = indexBufferOffset,
                                .count = 6});
                        indexBufferOffset += 6;

                        uniformBuffer.passes.at(passIndex).mvp = MatrixMath::translate({
                                                                                               pass.center.x,
                                                                                               pass.center.y,
                                                                                               0})
                                                                 * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                                                 * MatrixMath::translate({
                                                                                                 -pass.center.x,
                                                                                                 -pass.center.y,
                                                                                                 0})
                                                                 * MatrixMath::translate({
                                                                                                 pass.dstRect.position.x,
                                                                                                 pass.dstRect.position.y,
                                                                                                 0})
                                                                 * Camera::view(cameraTransform)
                                                                 * camera.projection();
                        uniformBuffer.passes.at(passIndex).color = pass.color.divide().getMemory();
                        uniformBuffer.passes.at(passIndex).colorMixFactor = pass.mix;
                        uniformBuffer.passes.at(passIndex).texAtlasLevel = pass.texture.level;
                        uniformBuffer.passes.at(passIndex).texIndex = (int) pass.texture.index;
                        uniformBuffer.passes.at(passIndex).uvOffset = pass.srcRect.position.getMemory();
                        uniformBuffer.passes.at(passIndex).uvScale = (pass.srcRect.dimensions /
                                                                      pass.texture.size.convert<float>()).getMemory();
                        uniformBuffer.passes.at(passIndex).atlasScale = (pass.texture.size.convert<float>() /
                                                                         TextureAtlas::getResolutionLevelSize(
                                                                                 pass.texture.level).convert<float>()).getMemory();
                        break;
                }
            }

            VertexBufferDesc vertexBufferDesc;
            vertexBufferDesc.size = vertexStream.getVertexBuffer().size();
            auto vertexBuffer = renderDevice.createVertexBuffer(vertexBufferDesc);

            vertexBuffer->upload(0, vertexStream.getVertexBuffer().data(), vertexStream.getVertexBuffer().size());

            IndexBufferDesc indexBufferDesc{};
            indexBufferDesc.size = indices.size() * sizeof(unsigned int);
            auto indexBuffer = renderDevice.createIndexBuffer(indexBufferDesc);

            indexBuffer->upload(0,
                                reinterpret_cast<const uint8_t *>(indices.data()),
                                indices.size() * sizeof(unsigned int));

            VertexArrayObjectDesc vertexArrayObjectDesc;
            vertexArrayObjectDesc.vertexLayout = vertexLayout;
            auto vao = renderDevice.createVertexArrayObject(vertexArrayObjectDesc);

            vao->bindBuffers(*vertexBuffer, *indexBuffer);

            ShaderBufferDesc shaderBufferDesc;
            shaderBufferDesc.size = sizeof(ShaderUniformBuffer);
            auto shaderBuffer = renderDevice.createShaderBuffer(shaderBufferDesc);

            shaderBuffer->upload(uniformBuffer);

            texturePipeline->renderBegin(*userTarget, mViewportOffset, mViewportSize);
            texturePipeline->bindVertexArrayObject(*vao);
            texturePipeline->bindShaderData({
                                                    *shaderBuffer,
                                                    atlas.getBuffer(TEXTURE_ATLAS_8x8),
                                                    atlas.getBuffer(TEXTURE_ATLAS_16x16),
                                                    atlas.getBuffer(TEXTURE_ATLAS_32x32),
                                                    atlas.getBuffer(TEXTURE_ATLAS_64x64),
                                                    atlas.getBuffer(TEXTURE_ATLAS_128x128),
                                                    atlas.getBuffer(TEXTURE_ATLAS_256x256),
                                                    atlas.getBuffer(TEXTURE_ATLAS_512x512),
                                                    atlas.getBuffer(TEXTURE_ATLAS_1024x1024),
                                                    atlas.getBuffer(TEXTURE_ATLAS_2048x2048),
                                                    atlas.getBuffer(TEXTURE_ATLAS_4096x4096),
                                                    atlas.getBuffer(TEXTURE_ATLAS_8192x8192),
                                                    atlas.getBuffer(TEXTURE_ATLAS_16384x16384),
                                            });
            texturePipeline->multiDrawIndexed(drawCalls, baseVertices);
            texturePipeline->renderPresent();
        }

        passes.clear();
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          TextureAtlasHandle &sprite,
                          const Vec2f &center,
                          float rotation,
                          float mix,
                          ColorRGBA mixColor) {
        passes.emplace_back(Pass(srcRect,
                                 dstRect,
                                 sprite,
                                 center,
                                 rotation,
                                 mix,
                                 mixColor));
    }

    void Renderer2D::draw(const Rectf &rectangle, ColorRGBA color, bool fill, const Vec2f &center, float rotation) {
        passes.emplace_back(Pass(rectangle, color, fill, center, rotation));
    }

    void Renderer2D::draw(const Vec2f &start, const Vec2f &end, ColorRGBA color, const Vec2f &center, float rotation) {
        passes.emplace_back(Pass(start, end, color, center, rotation));
    }

    void Renderer2D::draw(const Vec2f &point, ColorRGBA color) {
        passes.emplace_back(Pass(point, color));
    }

    void Renderer2D::rebindTextureAtlas(const std::map<TextureAtlasResolution, std::vector<bool>> &occupations) {
        std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> atlasRef;

        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_8x8,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_8x8).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_16x16,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_16x16).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_32x32,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_32x32).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_64x64,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_64x64).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_128x128, *atlasTextures.at(TEXTURE_ATLAS_128x128).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_256x256, *atlasTextures.at(TEXTURE_ATLAS_256x256).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_512x512, *atlasTextures.at(TEXTURE_ATLAS_512x512).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_1024x1024, *atlasTextures.at(TEXTURE_ATLAS_1024x1024).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_2048x2048, *atlasTextures.at(TEXTURE_ATLAS_2048x2048).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_4096x4096, *atlasTextures.at(TEXTURE_ATLAS_4096x4096).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_8192x8192, *atlasTextures.at(TEXTURE_ATLAS_8192x8192).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_16384x16384, *atlasTextures.at(TEXTURE_ATLAS_16384x16384).get()));

        atlas = TextureAtlas(atlasRef, occupations);
    }
}