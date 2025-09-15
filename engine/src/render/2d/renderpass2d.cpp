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

#include "xng/render/2d/renderpass2d.hpp"

#include "vertexshader.hpp"
#include "fragmentshader.hpp"
#include "xng/render/geometry/vertexstream.hpp"

namespace xng {
    static_assert(sizeof(float) == 4);
    static_assert(sizeof(int) == 4);

    struct ShaderBufferFormat {
        alignas(16) float color[4]{};
        alignas(4) float colorMixFactor{};
        alignas(4) float alphaMixFactor{};
        alignas(4) float colorFactor{};
        alignas(4) int texAtlasLevel = -1;
        alignas(4) int texAtlasIndex = -1;
        alignas(4) int texFilter{};
        alignas(16) Mat4f mvp{};
        alignas(16) float uvOffset_uvScale[4]{};
        alignas(16) float atlasScale_texSize[4]{};
        alignas(4) float _padding{}; // TODO: Handle shader buffer padding in the render graph runtime
        // Array stride in the ssbo must be 16 for std140 therefore we add this padding float
    };

    static_assert(sizeof(ShaderBufferFormat) % 16 == 0);

    /**
     * To ensure correct blending consequent draw calls of the same primitive are drawn in the same batch
     * together but each change in primitive causes a new draw batch to be created.
     */
    struct DrawBatch {
        bool clear{};
        ColorRGBA clearColor{};
        std::vector<DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        Primitive primitive = TRIANGLES;
        std::vector<ShaderBufferFormat> uniformBuffers; // The uniform buffer data for this batch
        Vec2i viewportOffset;
        Vec2i viewportSize;
    };

    RenderPass2D::RenderPass2D() {
        for (int res = TEXTURE_ATLAS_8x8; res < TEXTURE_ATLAS_END; res++) {
            atlasTexturesSizes[static_cast<TextureAtlasResolution>(res)] = 0;
        }
    }

    bool RenderPass2D::shouldRebuild() {
        meshBuffer.update(renderer.getRenderBatches());

        if (vertexBufferSize < meshBuffer.getVertexBufferSize()
            || indexBufferSize < meshBuffer.getIndexBufferSize()) {
            return true;
        }
        for (int res = TEXTURE_ATLAS_8x8; res < TEXTURE_ATLAS_END; res++) {
            auto r = static_cast<TextureAtlasResolution>(res);
            if (atlasTexturesSizes.at(r) < renderer.getTextureAtlas().getBufferOccupations().at(r).size()) {
                return true;
            }
        }
        return false;
    }

    void RenderPass2D::setup(RenderGraphBuilder &builder) {
        if (trianglePipeline) {
            // Recompile
            trianglePipeline = builder.inheritResource(trianglePipeline);
            linePipeline = builder.inheritResource(linePipeline);
            pointPipeline = builder.inheritResource(pointPipeline);

            auto &atlas = renderer.getTextureAtlas();
            auto atlasTexturesCopy = atlasTextures;
            atlasTextures.clear();
            for (auto &pair: atlasTexturesCopy) {
                if (atlas.getBufferOccupations().at(pair.first).size() > atlasTexturesSizes.at(pair.first)) {
                    // Buffer Reallocation / Copy
                    if (atlasTexturesSizes.at(pair.first) > 0) {
                        atlasCopyTextures[pair.first] = builder.inheritResource(pair.second);
                    }

                    RenderGraphTexture texture;
                    texture.format = RGBA;
                    texture.size = TextureAtlas::getResolutionLevelSize(pair.first);
                    texture.isArrayTexture = true;
                    texture.arrayLayers = atlas.getBufferOccupations().at(pair.first).size();
                    atlasTextures[pair.first] = builder.createTexture(texture);
                    atlasTexturesSizes[pair.first] = atlas.getBufferOccupations().at(pair.first).size();
                } else {
                    // Inherit old buffer
                    atlasTextures[pair.first] = builder.inheritResource(pair.second);
                }
            }

            if (vertexBufferSize < meshBuffer.getVertexBufferSize()) {
                vertexBufferCopy = builder.inheritResource(vertexBuffer);
                vertexBuffer = builder.createVertexBuffer(meshBuffer.getVertexBufferSize());
            } else {
                vertexBufferCopy = {};
                vertexBuffer = builder.inheritResource(vertexBuffer);
            }

            if (indexBufferSize < meshBuffer.getIndexBufferSize()) {
                indexBufferCopy = builder.inheritResource(indexBuffer);
                indexBuffer = builder.createIndexBuffer(meshBuffer.getIndexBufferSize());
            } else {
                indexBufferCopy = {};
                indexBuffer = builder.inheritResource(indexBuffer);
            }

            shaderBuffer = builder.inheritResource(shaderBuffer);
        } else {
            // Initial setup
            RenderGraphPipeline renderPipeline;
            renderPipeline.shaders.emplace_back(createVertexShader());
            renderPipeline.shaders.emplace_back(createFragmentShader());
            renderPipeline.enableBlending = true;
            renderPipeline.enableDepthTest = false;
            renderPipeline.depthTestWrite = false;
            renderPipeline.primitive = TRIANGLES;
            trianglePipeline = builder.createPipeline(renderPipeline);

            renderPipeline.primitive = LINES;
            linePipeline = builder.createPipeline(renderPipeline);

            renderPipeline.primitive = POINTS;
            pointPipeline = builder.createPipeline(renderPipeline);

            for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
                auto res = static_cast<TextureAtlasResolution>(i);
                RenderGraphTexture texture;
                texture.format = RGBA;
                texture.size = TextureAtlas::getResolutionLevelSize(res);
                texture.isArrayTexture = true;
                atlasTextures[res] = builder.createTexture(texture);
                atlasTexturesSizes[res] = 0;
            }

            vertexBuffer = builder.createVertexBuffer(0);
            indexBuffer = builder.createIndexBuffer(0);

            shaderBuffer = builder.createShaderBuffer(sizeof(ShaderBufferFormat));
        }

        screenTexture = builder.getScreenTexture();

        auto pass = builder.addPass("Renderer2D", [this](RenderGraphContext &ctx) {
            static bool printDebug = true;
            if (printDebug) {
                auto src = ctx.getShaderSource(trianglePipeline);
                std::cout << src[Shader::VERTEX] << std::endl << src[Shader::FRAGMENT] << std::endl;
                printDebug = false;
            }
            // Handle inherited resource buffers
            for (auto &v: atlasCopyTextures) {
                const auto src = v.second;
                const auto target = atlasTextures.at(v.first);
                ctx.copyTexture(target, src);
            }
            atlasCopyTextures.clear();

            if (vertexBufferCopy) {
                ctx.copyBuffer(vertexBuffer, vertexBufferCopy, 0, 0, vertexBufferSize);
                vertexBufferSize = meshBuffer.getVertexBufferSize();
                vertexBufferCopy = {};
            }
            if (indexBufferCopy) {
                ctx.copyBuffer(indexBuffer, indexBufferCopy, 0, 0, indexBufferSize);
                indexBufferSize = meshBuffer.getIndexBufferSize();
                indexBufferCopy = {};
            }

            meshBuffer.upload(vertexBuffer, indexBuffer, ctx);

            // TODO: Redesign RenderPass2D implementation
            std::vector<Primitive> primitives;
            std::vector<size_t> baseVertices;
            std::vector<DrawCall> drawCalls;
            std::vector<ShaderBufferFormat> passData;
            std::vector<size_t> batchIndices;
            for (auto i = 0; i < renderer.getRenderBatches().size(); i++) {
                auto &batch = renderer.getRenderBatches().at(i);

                // Handle texture allocation / Deallocation
                for (auto &alloc: batch.textureAllocations) {
                    renderer.getTextureAtlas().upload(ctx, alloc.first, atlasTextures, alloc.second);
                }
                for (auto &dalloc: batch.textureDeallocations) {
                    renderer.getTextureAtlas().remove(dalloc.getHandle());
                }

                // Create draw batches to handle interleaved primitives
                for (auto y = 0; y < batch.drawCommands.size(); y++) {
                    batchIndices.emplace_back(i);
                    auto &pass = batch.drawCommands.at(y);
                    switch (pass.type) {
                        case DrawCommand2D::COLOR_POINT: {
                            auto point = meshBuffer.getPoint(pass.dstRect.position);

                            primitives.emplace_back(point.primitive);
                            baseVertices.emplace_back(point.baseVertex);
                            drawCalls.emplace_back(point.drawCall);

                            auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                            auto model = MatrixMath::translate({
                                             pass.srcRect.position.x,
                                             pass.srcRect.position.y,
                                             0
                                         })
                                         * rotMat;

                            ShaderBufferFormat buffer;

                            buffer.mvp = batch.viewProjectionMatrix * model;

                            auto color = pass.color.divide();
                            buffer.color[0] = color.x;
                            buffer.color[1] = color.y;
                            buffer.color[2] = color.z;
                            buffer.color[3] = color.w;

                            passData.emplace_back(buffer);

                            break;
                        }
                        case DrawCommand2D::COLOR_LINE: {
                            auto line = meshBuffer.getLine(pass.dstRect.position, pass.dstRect.dimensions);

                            primitives.emplace_back(line.primitive);
                            baseVertices.emplace_back(line.baseVertex);
                            drawCalls.emplace_back(line.drawCall);

                            auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                            auto model = MatrixMath::translate({
                                             pass.srcRect.position.x,
                                             pass.srcRect.position.y,
                                             0
                                         })
                                         * rotMat;

                            ShaderBufferFormat buffer;

                            buffer.mvp = batch.viewProjectionMatrix * model;
                            auto color = pass.color.divide();
                            buffer.color[0] = color.x;
                            buffer.color[1] = color.y;
                            buffer.color[2] = color.z;
                            buffer.color[3] = color.w;

                            passData.emplace_back(buffer);
                            break;
                        }
                        case DrawCommand2D::COLOR_PLANE: {
                            if (pass.fill) {
                                auto plane = meshBuffer.getPlane(pass.dstRect.dimensions);

                                primitives.emplace_back(plane.primitive);
                                baseVertices.emplace_back(plane.baseVertex);
                                drawCalls.emplace_back(plane.drawCall);
                            } else {
                                auto square = meshBuffer.getSquare(pass.dstRect.dimensions);

                                primitives.emplace_back(square.primitive);
                                baseVertices.emplace_back(square.baseVertex);
                                drawCalls.emplace_back(square.drawCall);
                            }

                            auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                            auto model = MatrixMath::translate({
                                             pass.dstRect.position.x,
                                             pass.dstRect.position.y,
                                             0
                                         })
                                         * rotMat;

                            ShaderBufferFormat buffer;

                            buffer.mvp = batch.viewProjectionMatrix * model;
                            auto color = pass.color.divide();
                            buffer.color[0] = color.x;
                            buffer.color[1] = color.y;
                            buffer.color[2] = color.z;
                            buffer.color[3] = color.w;

                            passData.emplace_back(buffer);
                            break;
                        }
                        case DrawCommand2D::TEXTURE: {
                            auto plane = meshBuffer.getPlane(pass.dstRect.dimensions);

                            primitives.emplace_back(plane.primitive);
                            baseVertices.emplace_back(plane.baseVertex);
                            drawCalls.emplace_back(plane.drawCall);

                            auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                            auto model = MatrixMath::translate({
                                             pass.dstRect.position.x,
                                             pass.dstRect.position.y,
                                             0
                                         })
                                         * rotMat;

                            ShaderBufferFormat buffer;

                            buffer.mvp = batch.viewProjectionMatrix * model;

                            auto color = pass.color.divide();
                            buffer.color[0] = color.x;
                            buffer.color[1] = color.y;
                            buffer.color[2] = color.z;
                            buffer.color[3] = color.w;

                            buffer.colorMixFactor = pass.mix;
                            buffer.alphaMixFactor = pass.alphaMix;
                            buffer.colorFactor = pass.colorFactor;
                            buffer.texAtlasLevel = pass.texture.level;
                            buffer.texAtlasIndex = (int) pass.texture.index;
                            buffer.texFilter = pass.filter == LINEAR ? 1 : 0;
                            auto uvOffset = pass.srcRect.position / pass.texture.size.convert<float>();
                            buffer.uvOffset_uvScale[0] = uvOffset.x;
                            buffer.uvOffset_uvScale[1] = uvOffset.y;
                            auto uvScale = (pass.srcRect.dimensions /
                                            pass.texture.size.convert<float>());
                            buffer.uvOffset_uvScale[2] = uvScale.x;
                            buffer.uvOffset_uvScale[3] = uvScale.y;
                            auto atlasScale = (pass.texture.size.convert<float>() /
                                               TextureAtlas::getResolutionLevelSize(
                                                   pass.texture.level).convert<float>());
                            buffer.atlasScale_texSize[0] = atlasScale.x;
                            buffer.atlasScale_texSize[1] = atlasScale.y;
                            buffer.atlasScale_texSize[2] = static_cast<float>(pass.texture.size.x);
                            buffer.atlasScale_texSize[3] = static_cast<float>(pass.texture.size.y);

                            passData.emplace_back(buffer);

                            break;
                        }
                    }
                }
            }

            // Hacky way to properly clear between primitive draw batches
            size_t renderBatchIndex = 0;
            bool gotRenderBatchIndex = false;

            std::vector<DrawBatch> batches;
            DrawBatch currentBatch;
            for (auto y = 0; y < drawCalls.size(); y++) {
                auto prim = primitives.at(y);
                if (prim != currentBatch.primitive) {
                    if (!currentBatch.drawCalls.empty())
                        batches.emplace_back(currentBatch);
                    currentBatch = {};
                    currentBatch.primitive = prim;
                }
                currentBatch.drawCalls.emplace_back(drawCalls.at(y));
                currentBatch.baseVertices.emplace_back(baseVertices.at(y));
                currentBatch.uniformBuffers.emplace_back(passData.at(y));
                if (!gotRenderBatchIndex
                    || batchIndices.at(y) > renderBatchIndex) {
                    currentBatch.clear = renderer.getRenderBatches().at(batchIndices.at(y)).mClear;
                    currentBatch.clearColor = renderer.getRenderBatches().at(batchIndices.at(y)).mClearColor;
                    renderBatchIndex = batchIndices.at(y);
                    gotRenderBatchIndex = true;
                }
                currentBatch.viewportOffset = renderer.getRenderBatches().at(batchIndices.at(y)).mViewportOffset;
                currentBatch.viewportSize = renderer.getRenderBatches().at(batchIndices.at(y)).mViewportSize;
            }
            if (!currentBatch.drawCalls.empty())
                batches.emplace_back(currentBatch);

            // Render the draw batches
            ctx.beginRenderPass({RenderGraphAttachment(screenTexture)}, {});

            std::vector<RenderGraphResource> textures;
            for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
                textures.emplace_back(atlasTextures.at(static_cast<TextureAtlasResolution>(i)));
            }

            for (auto &batch: batches) {
                ctx.setViewport(batch.viewportOffset, batch.viewportSize);
                if (batch.clear) {
                    ctx.clearColorAttachment(0, batch.clearColor);
                }
                switch (batch.primitive) {
                    case POINTS:
                        ctx.bindPipeline(pointPipeline);
                        break;
                    case LINES:
                        ctx.bindPipeline(linePipeline);
                        break;
                    case TRIANGLES:
                        ctx.bindPipeline(trianglePipeline);
                        break;
                    default:
                        throw std::runtime_error("Unsupported primitive");
                }

                ctx.bindVertexBuffer(vertexBuffer);
                ctx.bindIndexBuffer(indexBuffer);
                ctx.bindShaderBuffers({{"vars", shaderBuffer}});
                ctx.bindTextures({{"atlasTextures", textures}});

                for (auto y = 0; y < batch.drawCalls.size(); y++) {
                    auto buf = batch.uniformBuffers.at(y);
                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&buf),
                                     sizeof(ShaderBufferFormat),
                                     0);
                    ctx.drawIndexed(batch.drawCalls.at(y), batch.baseVertices.at(y));
                }
            }

            ctx.endRenderPass();

            renderer.clearBatches();
        });
        builder.read(pass, pointPipeline);
        builder.read(pass, linePipeline);
        builder.read(pass, trianglePipeline);
        builder.readWrite(pass, vertexBuffer);
        builder.readWrite(pass, indexBuffer);
        builder.readWrite(pass, shaderBuffer);
        for (auto &tex: atlasTextures) {
            builder.readWrite(pass, tex.second);
        }
        for (auto &tex: atlasCopyTextures) {
            builder.read(pass, tex.second);
        }
        if (vertexBufferCopy) {
            builder.read(pass, vertexBufferCopy);
        }
        if (indexBufferCopy) {
            builder.read(pass, indexBufferCopy);
        }
        builder.write(pass, screenTexture);
    }

    Mat4f RenderPass2D::getRotationMatrix(float rotation, Vec2f center) {
        auto pair = std::make_pair(rotation, center);
        usedRotationMatrices.insert(pair);
        auto it = rotationMatrices.find(pair);
        if (it != rotationMatrices.end()) {
            return it->second;
        } else {
            rotationMatrices[pair] = MatrixMath::translate({
                                         center.x,
                                         center.y,
                                         0
                                     })
                                     * MatrixMath::rotate(Vec3f(0, 0, rotation))
                                     * MatrixMath::translate({
                                         -center.x,
                                         -center.y,
                                         0
                                     });
            return rotationMatrices.at(pair);
        }
    }
}
