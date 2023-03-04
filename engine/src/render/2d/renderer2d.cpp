/**
 *  This file is part of xEngine, a C++ game engine library.
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
    vec4 color;
    vec4 colorMixFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale;
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
    vec4 color;
    vec4 colorMixFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passes[MAX_DRAW];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.passes[drawID].texAtlasLevel_texAtlasIndex.y >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.passes[drawID].uvOffset_uvScale.zw;
        uv = uv + vars.passes[drawID].uvOffset_uvScale.xy;
        uv = uv * vars.passes[drawID].atlasScale.xy;
        color = texture(atlasTextures[vars.passes[drawID].texAtlasLevel_texAtlasIndex.x],
                        vec3(uv.x, uv.y, vars.passes[drawID].texAtlasLevel_texAtlasIndex.y));
        color = mix(color, vars.passes[drawID].color, vars.passes[drawID].colorMixFactor.x);
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

#pragma pack(push, 1)
    // If anything other than 4 component vectors are used in uniform buffer data the memory layout becomes unpredictable.
    struct PassData {
        float color[4];
        float colorMixFactor[4];
        int texAtlasLevel_texAtlasIndex[4]{-1, -1};
        Mat4f mvp;
        float uvOffset_uvScale[4];
        float atlasScale[4];
    };

    struct ShaderUniformBuffer {
        PassData passes[MAX_DRAW];
    };
#pragma pack(pop)

    static_assert(sizeof(PassData) % 16 == 0);
    static_assert(sizeof(ShaderUniformBuffer) % 16 == 0);

    /**
     * A draw cycle consists of one or more draw batches.
     *
     * Each draw batch contains draw calls which are dispatched using multiDraw.
     *
     * To ensure correct blending consequent draw calls of the same primitive are drawn in the same batch
     * together but each change in primitive causes a new draw batch to be created which incurs the overhead of
     * updating the shader uniform buffer.
     */
    struct DrawBatch {
        std::vector<RenderPass::DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        Primitive primitive = TRIANGLES;
        ShaderUniformBuffer uniformBuffer; // The uniform buffer data for this batch
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
        desc.enableBlending = true;

        trianglePipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = LINES;
        linePipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = POINTS;
        pointPipeline = device.createRenderPipeline(desc, shaderDecompiler);

        for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
            auto res = (TextureAtlasResolution) i;
            TextureArrayBufferDesc atlasDesc;
            atlasDesc.textureDesc.size = TextureAtlas::getResolutionLevelSize(res);
            atlasTextures[res] = std::move(device.createTextureArrayBuffer(atlasDesc));
        }

        RenderPassDesc passDesc;
        passDesc.numberOfColorAttachments = 1;
        passDesc.hasDepthStencilAttachment = false;
        renderPass = device.createRenderPass(passDesc);

        rebindTextureAtlas({});
    }

    Renderer2D::~Renderer2D() = default;

    TextureAtlasHandle Renderer2D::createTexture(const ImageRGBA &texture) {
        auto res = TextureAtlas::getClosestMatchingResolutionLevel(texture.getSize());
        auto free = atlas.getFreeSlotCount(res);
        if (free < 1) {
            auto desc = atlasTextures.at(res)->getDescription();
            desc.textureCount += 1;
            auto buf = std::move(atlasTextures.at(res));
            atlasTextures.at(res) = renderDevice.createTextureArrayBuffer(desc);
            atlasTextures.at(res)->copy(*buf);
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
                                 const ColorRGBA &clearColor,
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
            renderPass->beginRenderPass(target, viewportOffset, viewportSize);
            renderPass->clearColorAttachments(clearColor);
            renderPass->clearDepthAttachments(1);
            renderPass->endRenderPass();
        }

        userTarget = &target;

        polyCounter = 0;
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
        } else if (!passes.empty()) {
            drawCycles = 1;
        }

        for (int i = 0; i < drawCycles; i++) {
            std::vector<RenderPass::DrawCall> drawCalls;
            std::vector<Primitive> primitives;
            std::vector<size_t> baseVertices;
            VertexStream vertexStream;
            std::vector<unsigned int> indices;
            std::vector<PassData> passData(MAX_DRAW);

            size_t indexBufferOffset = 0;

            auto currentPassBase = i * MAX_DRAW;
            for (auto passIndex = 0; passIndex < MAX_DRAW && passIndex + currentPassBase < passes.size(); passIndex++) {
                auto &pass = passes.at(passIndex + currentPassBase);
                switch (pass.type) {
                    case Pass::COLOR_POINT: {
                        primitives.emplace_back(POINTS);
                        baseVertices.emplace_back(vertexStream.getVertices().size());

                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f())
                                                       .addVec2(Vec2f())
                                                       .build());

                        indices.emplace_back(0);

                        drawCalls.emplace_back(RenderPass::DrawCall{
                                .offset = indexBufferOffset * sizeof(unsigned int),
                                .count = 1});

                        indexBufferOffset += 1;

                        polyCounter += 1;

                        auto rotMat = MatrixMath::translate({
                                                                    pass.center.x,
                                                                    pass.center.y,
                                                                    0})
                                      * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                      * MatrixMath::translate({
                                                                      -pass.center.x,
                                                                      -pass.center.y,
                                                                      0});

                        auto model = MatrixMath::translate({
                                                                   pass.srcRect.position.x,
                                                                   pass.srcRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = camera.projection()
                                                  * Camera::view(cameraTransform)
                                                  * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::COLOR_LINE: {
                        primitives.emplace_back(LINES);
                        baseVertices.emplace_back(vertexStream.getVertices().size());

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

                        drawCalls.emplace_back(RenderPass::DrawCall{
                                .offset = indexBufferOffset * sizeof(unsigned int),
                                .count = 2});

                        indexBufferOffset += 2;

                        polyCounter += 1;

                        auto rotMat = MatrixMath::translate({
                                                                    pass.center.x,
                                                                    pass.center.y,
                                                                    0})
                                      * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                      * MatrixMath::translate({
                                                                      -pass.center.x,
                                                                      -pass.center.y,
                                                                      0});

                        auto model = MatrixMath::translate({
                                                                   pass.srcRect.position.x,
                                                                   pass.srcRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = camera.projection()
                                                  * Camera::view(cameraTransform)
                                                  * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::COLOR_PLANE: {
                        baseVertices.emplace_back(vertexStream.getVertices().size());

                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, 0))
                                                       .addVec2(Vec2f())
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x, 0))
                                                       .addVec2(Vec2f())
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(0, pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f())
                                                       .build());
                        vertexStream.addVertex(VertexBuilder()
                                                       .addVec2(Vec2f(pass.dstRect.dimensions.x,
                                                                      pass.dstRect.dimensions.y))
                                                       .addVec2(Vec2f())
                                                       .build());

                        if (pass.fill) {
                            primitives.emplace_back(TRIANGLES);

                            indices.emplace_back(0);
                            indices.emplace_back(1);
                            indices.emplace_back(2);

                            indices.emplace_back(1);
                            indices.emplace_back(2);
                            indices.emplace_back(3);

                            drawCalls.emplace_back(RenderPass::DrawCall{
                                    .offset = indexBufferOffset * sizeof(unsigned int),
                                    .count = 6});

                            indexBufferOffset += 6;

                            polyCounter += 2;
                        } else {
                            primitives.emplace_back(LINES);

                            indices.emplace_back(0);
                            indices.emplace_back(1);

                            indices.emplace_back(1);
                            indices.emplace_back(3);

                            indices.emplace_back(3);
                            indices.emplace_back(2);

                            indices.emplace_back(2);
                            indices.emplace_back(0);

                            drawCalls.emplace_back(RenderPass::DrawCall{
                                    .offset = indexBufferOffset * sizeof(unsigned int),
                                    .count = 8});

                            indexBufferOffset += 8;

                            polyCounter += 4;
                        }

                        auto rotMat = MatrixMath::translate({
                                                                    pass.center.x,
                                                                    pass.center.y,
                                                                    0})
                                      * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                      * MatrixMath::translate({
                                                                      -pass.center.x,
                                                                      -pass.center.y,
                                                                      0});

                        auto model = MatrixMath::translate({
                                                                   pass.dstRect.position.x,
                                                                   pass.dstRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = camera.projection()
                                                  * Camera::view(cameraTransform)
                                                  * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::TEXTURE: {
                        primitives.emplace_back(TRIANGLES);
                        baseVertices.emplace_back(vertexStream.getVertices().size());

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

                        indices.emplace_back(0);
                        indices.emplace_back(1);
                        indices.emplace_back(2);

                        indices.emplace_back(1);
                        indices.emplace_back(2);
                        indices.emplace_back(3);

                        drawCalls.emplace_back(RenderPass::DrawCall{
                                .offset = indexBufferOffset * sizeof(unsigned int),
                                .count = 6});

                        indexBufferOffset += 6;

                        polyCounter += 2;

                        auto rotMat = MatrixMath::translate({
                                                                    pass.center.x,
                                                                    pass.center.y,
                                                                    0})
                                      * MatrixMath::rotate(Vec3f(0, 0, pass.rotation))
                                      * MatrixMath::translate({
                                                                      -pass.center.x,
                                                                      -pass.center.y,
                                                                      0});

                        auto model = MatrixMath::translate({
                                                                   pass.dstRect.position.x,
                                                                   pass.dstRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = camera.projection()
                                                  * Camera::view(cameraTransform)
                                                  * model;

                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        passData[passIndex].colorMixFactor[0] = pass.mix;
                        passData[passIndex].texAtlasLevel_texAtlasIndex[0] = pass.texture.level;
                        passData[passIndex].texAtlasLevel_texAtlasIndex[1] = (int) pass.texture.index;
                        auto uvOffset = pass.srcRect.position / pass.texture.size.convert<float>();
                        passData[passIndex].uvOffset_uvScale[0] = uvOffset.x;
                        passData[passIndex].uvOffset_uvScale[1] = uvOffset.y;
                        auto uvScale = (pass.srcRect.dimensions /
                                        pass.texture.size.convert<float>());
                        passData[passIndex].uvOffset_uvScale[2] = uvScale.x;
                        passData[passIndex].uvOffset_uvScale[3] = uvScale.y;
                        auto atlasScale = (pass.texture.size.convert<float>() /
                                           TextureAtlas::getResolutionLevelSize(
                                                   pass.texture.level).convert<float>());
                        passData[passIndex].atlasScale[0] = atlasScale.x;
                        passData[passIndex].atlasScale[1] = atlasScale.y;
                        break;
                    }
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

            std::vector<DrawBatch> batches;
            DrawBatch currentBatch;
            size_t currentBatchIndex = 0;
            for (auto y = 0; y < drawCalls.size(); y++) {
                auto prim = primitives.at(y);
                if (prim != currentBatch.primitive) {
                    if (!currentBatch.drawCalls.empty())
                        batches.emplace_back(currentBatch);
                    currentBatch = {};
                    currentBatch.primitive = prim;
                    currentBatchIndex = 0;
                }
                currentBatch.drawCalls.emplace_back(drawCalls.at(y));
                currentBatch.baseVertices.emplace_back(baseVertices.at(y));
                currentBatch.uniformBuffer.passes[currentBatchIndex++] = passData[y];
            }
            if (!currentBatch.drawCalls.empty())
                batches.emplace_back(currentBatch);

            renderPass->beginRenderPass(*userTarget, mViewportOffset, mViewportSize);
            renderPass->bindVertexArrayObject(*vao);

            ShaderBufferDesc shaderBufferDesc;
            shaderBufferDesc.size = sizeof(ShaderUniformBuffer);
            auto shaderBuffer = renderDevice.createShaderBuffer(shaderBufferDesc);

            renderPass->bindShaderData({
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

            for (auto &batch: batches) {
                shaderBuffer->upload(batch.uniformBuffer);

                switch (batch.primitive) {
                    case POINTS:
                        renderPass->bindPipeline(*pointPipeline);
                        renderPass->multiDrawIndexed(batch.drawCalls, batch.baseVertices);
                        break;
                    case LINES:
                        renderPass->bindPipeline(*linePipeline);
                        renderPass->multiDrawIndexed(batch.drawCalls, batch.baseVertices);
                        break;
                    case TRIANGLES:
                        renderPass->bindPipeline(*trianglePipeline);
                        renderPass->multiDrawIndexed(batch.drawCalls, batch.baseVertices);
                        break;
                    default:
                        throw std::runtime_error("Unsupported primitive");
                }
            }

            renderPass->endRenderPass();
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

    void Renderer2D::draw(const Vec2f &start, const Vec2f &end, ColorRGBA color, const Vec2f &position, const Vec2f &center, float rotation) {
        passes.emplace_back(Pass(start, end, color,position, center, rotation));
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