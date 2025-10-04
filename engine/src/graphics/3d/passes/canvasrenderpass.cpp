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

#include <utility>

#include "xng/graphics/3d/passes/canvasrenderpass.hpp"

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
        alignas(4) float customTexture = 0;
        // TODO: Handle shader buffer padding automatically in the render graph runtime
        // Array stride in the ssbo must be 16 for std140 therefore we add this padding float
    };

    static_assert(sizeof(ShaderBufferFormat) % 16 == 0);

    CanvasRenderPass::CanvasRenderPass(std::shared_ptr<RenderConfiguration> config,
                                       std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(config)), registry(std::move(registry)) {
        screenSpaceLayer.name = LAYER_CANVASES_SCREEN;
        worldSpaceLayer.name = LAYER_CANVASES_WORLD;
        screenSpaceLayer.containsTransparency = true;
        worldSpaceLayer.containsTransparency = true;
    }

    bool CanvasRenderPass::shouldRebuild(const Vec2i &backBufferSize) {
        canvases = config->getCanvases();

        updateGlyphTextures();
        updateTextures();

        meshBuffer.update(canvases);

        for (auto &canvas: canvases) {
            for (auto &paintCommand: canvas.getPaintCommands()) {
                if (paintCommand.type == Paint::PAINT_TEXT) {
                    auto &txt = std::get<PaintText>(paintCommand.data);
                    auto key = RenderText(txt.text.text, txt.color, txt.text.fontUri, txt.text.fontPixelSize,
                                          txt.text.parameters);
                    if (textCache.find(key) == textCache.end()) {
                        return true;
                    }
                }
            }
        }

        return atlas.shouldRebuild()
               || backBufferSize != layerSize
               || vertexBufferSize != meshBuffer.getVertexBufferSize()
               || indexBufferSize != meshBuffer.getIndexBufferSize();
    }

    void CanvasRenderPass::create(RenderGraphBuilder &builder) {
        RenderGraphPipeline pipeline;
        pipeline.shaders.emplace_back(createVertexShader());
        pipeline.shaders.emplace_back(createFragmentShader());
        pipeline.enableBlending = true;
        pipeline.enableDepthTest = false;
        pipeline.depthTestWrite = false;
        pipeline.primitive = TRIANGLES;
        pipeline.alphaBlendSourceMode = RenderGraphPipeline::ONE;
        pipeline.alphaBlendDestinationMode = RenderGraphPipeline::ONE_MINUS_SRC_ALPHA;
        trianglePipeline = builder.createPipeline(pipeline);

        pipeline.primitive = LINES;
        linePipeline = builder.createPipeline(pipeline);

        pipeline.primitive = POINTS;
        pointPipeline = builder.createPipeline(pipeline);

        layerSize = builder.getBackBufferSize();

        RenderGraphTexture texture;
        texture.size = layerSize;
        texture.format = RGBA;
        screenSpaceLayer.color = builder.createTexture(texture);
        worldSpaceLayer.color = builder.createTexture(texture);

        texture.format = DEPTH;
        worldSpaceLayer.depth = builder.createTexture(texture);

        auto layers = registry->getOrCreate<CompositingLayers>();
        layers.layers.emplace_back(worldSpaceLayer);
        layers.layers.emplace_back(screenSpaceLayer);
        registry->set<CompositingLayers>(layers);

        atlas.onCreate(builder);

        vertexBufferSize = meshBuffer.getVertexBufferSize();
        vertexBuffer = builder.createVertexBuffer(vertexBufferSize);

        indexBufferSize = meshBuffer.getIndexBufferSize();
        indexBuffer = builder.createIndexBuffer(indexBufferSize);

        shaderBuffer = builder.createShaderBuffer(sizeof(ShaderBufferFormat));

        auto pass = builder.addPass("CanvasRenderPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        updateTextCache(pass, builder);

        atlas.declareReadWrite(builder, pass);

        builder.readWrite(pass, vertexBuffer);
        builder.readWrite(pass, indexBuffer);
        builder.readWrite(pass, shaderBuffer);

        builder.write(pass, screenSpaceLayer.color);
        builder.write(pass, worldSpaceLayer.color);
        builder.write(pass, worldSpaceLayer.depth);
    }

    void CanvasRenderPass::recreate(RenderGraphBuilder &builder) {
        trianglePipeline = builder.inheritResource(trianglePipeline);
        linePipeline = builder.inheritResource(linePipeline);
        pointPipeline = builder.inheritResource(pointPipeline);
        shaderBuffer = builder.inheritResource(shaderBuffer);

        if (builder.getBackBufferSize() != layerSize) {
            layerSize = builder.getBackBufferSize();

            RenderGraphTexture texture;
            texture.size = layerSize;
            texture.format = RGBA;
            screenSpaceLayer.color = builder.createTexture(texture);
            worldSpaceLayer.color = builder.createTexture(texture);

            texture.format = DEPTH;
            worldSpaceLayer.depth = builder.createTexture(texture);
        } else {
            screenSpaceLayer.color = builder.inheritResource(screenSpaceLayer.color);
            worldSpaceLayer.color = builder.inheritResource(worldSpaceLayer.color);
            worldSpaceLayer.depth = builder.inheritResource(worldSpaceLayer.depth);
        }

        auto layers = registry->check<CompositingLayers>() ? registry->get<CompositingLayers>() : CompositingLayers();
        layers.layers.emplace_back(worldSpaceLayer);
        layers.layers.emplace_back(screenSpaceLayer);
        registry->set<CompositingLayers>(layers);

        atlas.onRecreate(builder);

        if (vertexBufferSize < meshBuffer.getVertexBufferSize()) {
            vertexBufferCopy = builder.inheritResource(vertexBuffer);
            vertexBuffer = builder.createVertexBuffer(meshBuffer.getVertexBufferSize());
        } else {
            vertexBuffer = builder.inheritResource(vertexBuffer);
        }

        if (indexBufferSize < meshBuffer.getIndexBufferSize()) {
            indexBufferCopy = builder.inheritResource(indexBuffer);
            indexBuffer = builder.createIndexBuffer(meshBuffer.getIndexBufferSize());
        } else {
            indexBuffer = builder.inheritResource(indexBuffer);
        }

        auto pass = builder.addPass("CanvasRenderPass", [this](RenderGraphContext &ctx) {
            runPass(ctx);
        });

        updateTextCache(pass, builder);

        atlas.declareReadWrite(builder, pass);

        builder.readWrite(pass, vertexBuffer);
        builder.readWrite(pass, indexBuffer);
        builder.readWrite(pass, shaderBuffer);

        builder.write(pass, screenSpaceLayer.color);
        builder.write(pass, worldSpaceLayer.color);
        builder.write(pass, worldSpaceLayer.depth);
    }

    void CanvasRenderPass::runPass(RenderGraphContext &ctx) {
        if (vertexBufferCopy) {
            ctx.copyBuffer(vertexBuffer, vertexBufferCopy, 0, 0, vertexBufferSize);
            vertexBufferCopy = {};
            vertexBufferSize = meshBuffer.getVertexBufferSize();
        }

        if (indexBufferCopy) {
            ctx.copyBuffer(indexBuffer, indexBufferCopy, 0, 0, indexBufferSize);
            indexBufferCopy = {};
            indexBufferSize = meshBuffer.getIndexBufferSize();
        }

        meshBuffer.upload(vertexBuffer, indexBuffer, ctx);

        // Screen space canvases are always the same size as the back buffer,
        for (auto &canvas: canvases) {
            if (canvas.isWorldSpace())
                continue;
            renderCanvas(screenSpaceLayer.color, canvas, ctx);
        }

        ctx.clearTextureColor(worldSpaceLayer.color, ColorRGBA::black(1, 0));
        ctx.clearTextureDepthStencil(worldSpaceLayer.depth, 1, 0);

        //TODO: Implement World Space canvas rendering
    }

    void CanvasRenderPass::updateGlyphTextures() {
        std::unordered_set<std::pair<Uri, Vec2i>, GlyphTextureHash> usedFonts;
        for (auto &canvas: canvases) {
            for (auto &paintCommand: canvas.getPaintCommands()) {
                if (paintCommand.type != Paint::PAINT_TEXT)
                    continue;
                auto &text = std::get<PaintText>(paintCommand.data);
                auto key = std::pair{text.text.fontUri, text.text.fontPixelSize};
                usedFonts.insert(key);
                auto &glyphTextureHandles = glyphTextures[key];
                for (auto &glyph: text.text.glyphs) {
                    if (glyphTextureHandles.find(glyph.character->value) == glyphTextureHandles.end()) {
                        glyphTextureHandles[glyph.character->value] = atlas.add(glyph.character->image);
                    }
                }
            }
        }
        std::unordered_set<std::pair<Uri, Vec2i>, GlyphTextureHash> delFonts;
        for (auto &fontTextures: glyphTextures) {
            if (usedFonts.find(fontTextures.first) == usedFonts.end()) {
                delFonts.insert(fontTextures.first);
                for (auto &glyphTextureHandles: fontTextures.second) {
                    atlas.remove(glyphTextureHandles.second);
                }
            }
        }
        for (auto &fontTextures: delFonts) {
            glyphTextures.erase(fontTextures);
        }
    }

    void CanvasRenderPass::updateTextures() {
        std::set<Uri> usedTextures;
        for (auto &canvas: canvases) {
            for (auto &paintCommand: canvas.getPaintCommands()) {
                if (paintCommand.type != Paint::PAINT_IMAGE)
                    continue;
                auto &img = std::get<PaintImage>(paintCommand.data);
                usedTextures.insert(img.image.getUri());
                if (textureAtlasHandles.find(img.image.getUri()) == textureAtlasHandles.end()) {
                    textureAtlasHandles[img.image.getUri()] = atlas.add(img.image.get());
                }
            }
        }

        std::set<Uri> delTextures;
        for (auto &textureHandles: textureAtlasHandles) {
            if (usedTextures.find(textureHandles.first) == usedTextures.end()) {
                delTextures.insert(textureHandles.first);
                atlas.remove(textureHandles.second);
            }
        }

        for (auto &textureHandles: delTextures) {
            textureAtlasHandles.erase(textureHandles);
        }
    }

    void CanvasRenderPass::updateTextCache(RenderGraphBuilder::PassHandle pass, RenderGraphBuilder &builder) {
        std::unordered_set<RenderText, RenderTextHash> usedTexts;
        for (auto &canvas: canvases) {
            for (auto &paintCommand: canvas.getPaintCommands()) {
                if (paintCommand.type != Paint::PAINT_TEXT)
                    continue;
                auto &txt = std::get<PaintText>(paintCommand.data);
                auto key = RenderText{
                    txt.text.text, txt.color, txt.text.fontUri, txt.text.fontPixelSize, txt.text.parameters
                };
                usedTexts.insert(key);
                if (textCache.find(key) == textCache.end()) {
                    RenderGraphTexture texture;
                    texture.format = RGBA;
                    texture.size = txt.text.size;
                    textCache[key] = builder.createTexture(texture);
                    textCachePending.insert(key);
                } else {
                    textCache.at(key) = builder.inheritResource(textCache.at(key));
                }
                builder.readWrite(pass, textCache.at(key));
            }
        }

        std::unordered_set<RenderText, RenderTextHash> delTexts;
        for (auto &textCacheEntry: textCache) {
            if (usedTexts.find(textCacheEntry.first) == usedTexts.end()) {
                delTexts.insert(textCacheEntry.first);
            }
        }

        for (auto &del: delTexts) {
            textCache.erase(del);
        }
    }

    void CanvasRenderPass::renderTextCache(RenderGraphContext &ctx,
                                           std::vector<RenderGraphResource> atlasTextureBindings) {
        for (auto &canvas: canvases) {
            for (auto &paintCommand: canvas.getPaintCommands()) {
                if (paintCommand.type != Paint::PAINT_TEXT)
                    continue;
                auto &txt = std::get<PaintText>(paintCommand.data);
                auto key = RenderText{
                    txt.text.text, txt.color, txt.text.fontUri, txt.text.fontPixelSize, txt.text.parameters
                };
                if (textCachePending.find(key) == textCachePending.end())
                    continue;
                textCachePending.erase(key);

                ctx.beginRenderPass({RenderGraphAttachment(textCache.at(key), ColorRGBA::black(0, 0))}, {});
                ctx.setViewport({}, txt.text.size);
                ctx.bindPipeline(trianglePipeline);
                ctx.bindVertexBuffer(vertexBuffer);
                ctx.bindIndexBuffer(indexBuffer);
                ctx.bindShaderBuffer("vars", shaderBuffer);
                ctx.bindTexture("atlasTextures", atlasTextureBindings);

                auto &text = std::get<PaintText>(paintCommand.data);

                Camera camera;
                camera.type = ORTHOGRAPHIC;
                camera.top = text.text.size.y;
                camera.left = 0;
                camera.right = text.text.size.x;
                camera.bottom = 0;

                auto viewProjection = camera.projection()
                                      * Camera::view(Transform(Vec3f(0, 0, 1), Vec3f(), Vec3f()));

                auto &glyphTextureHandles = glyphTextures.at({text.text.fontUri, text.text.fontPixelSize});
                for (auto &glyph: text.text.glyphs) {
                    auto model = MatrixMath::translate({
                        glyph.position.x,
                        glyph.position.y,
                        0
                    });

                    ShaderBufferFormat data;
                    data.mvp = viewProjection * model;

                    auto color = text.color.divide();
                    data.color[0] = color.x;
                    data.color[1] = color.y;
                    data.color[2] = color.z;
                    data.color[3] = color.w;

                    auto atlasTex = glyphTextureHandles.at(glyph.character->value);

                    data.colorMixFactor = 0;
                    data.alphaMixFactor = 0;
                    data.colorFactor = 1;
                    data.texAtlasLevel = atlasTex.level;
                    data.texAtlasIndex = static_cast<int>(atlasTex.index);
                    data.texFilter = 0;

                    data.uvOffset_uvScale[0] = 0;
                    data.uvOffset_uvScale[1] = 0;

                    data.uvOffset_uvScale[2] = 1;
                    data.uvOffset_uvScale[3] = 1;

                    auto atlasScale = (atlasTex.size.convert<float>() /
                                       TextureAtlas::getResolutionLevelSize(
                                           atlasTex.level).convert<float>());
                    data.atlasScale_texSize[0] = atlasScale.x;
                    data.atlasScale_texSize[1] = atlasScale.y;
                    data.atlasScale_texSize[2] = static_cast<float>(atlasTex.size.x);
                    data.atlasScale_texSize[3] = static_cast<float>(atlasTex.size.y);

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    auto plane = meshBuffer.getPlane(glyph.character->image.getResolution().convert<float>());
                    ctx.drawIndexed(plane.drawCall, plane.baseVertex);
                }
                ctx.endRenderPass();
            }
        }
    }

    void CanvasRenderPass::renderCanvas(RenderGraphResource target,
                                        const Canvas &canvas,
                                        RenderGraphContext &ctx) {
        auto &atlasTextures = atlas.getAtlasTextures(ctx);

        std::vector<RenderGraphResource> textures;
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
            auto res = static_cast<TextureAtlasResolution>(i);
            textures.emplace_back(atlasTextures.at(res));
        }

        renderTextCache(ctx, textures);

        ctx.beginRenderPass({RenderGraphAttachment(target)}, {});
        ctx.setViewport({}, {layerSize.x, layerSize.y});
        ctx.clearColorAttachment(0, canvas.getBackgroundColor());
        ctx.bindPipeline(trianglePipeline);
        ctx.bindVertexBuffer(vertexBuffer);
        ctx.bindIndexBuffer(indexBuffer);
        ctx.bindShaderBuffer("vars", shaderBuffer);
        ctx.bindTexture("atlasTextures", textures);

        Primitive currentPrimitive = TRIANGLES;
        for (auto &paintCommand: canvas.getPaintCommands()) {
            switch (paintCommand.type) {
                case Paint::PAINT_POINT: {
                    if (currentPrimitive != POINTS) {
                        currentPrimitive = POINTS;
                        ctx.bindPipeline(pointPipeline);
                        ctx.bindVertexBuffer(vertexBuffer);
                        ctx.bindIndexBuffer(indexBuffer);
                        ctx.bindShaderBuffer("vars", shaderBuffer);
                        ctx.bindTexture("atlasTextures", textures);
                    }
                    auto &point = std::get<PaintPoint>(paintCommand.data);

                    auto model = MatrixMath::translate({
                        point.position.x,
                        point.position.y,
                        0
                    });

                    ShaderBufferFormat data;
                    data.mvp = canvas.getViewProjectionMatrix() * model;

                    auto color = point.color.divide();
                    data.color[0] = color.x;
                    data.color[1] = color.y;
                    data.color[2] = color.z;
                    data.color[3] = color.w;

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    auto &pointDraw = meshBuffer.getPoint(point.position);
                    ctx.drawIndexed(pointDraw.drawCall, pointDraw.baseVertex);
                    break;
                }
                case Paint::PAINT_LINE: {
                    if (currentPrimitive != LINES) {
                        currentPrimitive = LINES;
                        ctx.bindPipeline(linePipeline);
                        ctx.bindVertexBuffer(vertexBuffer);
                        ctx.bindIndexBuffer(indexBuffer);
                        ctx.bindShaderBuffer("vars", shaderBuffer);
                        ctx.bindTexture("atlasTextures", textures);
                    }
                    auto &line = std::get<PaintLine>(paintCommand.data);
                    Mat4f rotMat = MatrixMath::identity();

                    if (line.rotation != 0) {
                        rotMat = MatrixMath::translate({
                                     line.center.x,
                                     line.center.y,
                                     0
                                 })
                                 * MatrixMath::rotate(Vec3f(0, 0, line.rotation))
                                 * MatrixMath::translate({
                                     -line.center.x,
                                     -line.center.y,
                                     0
                                 });
                    }

                    auto model = MatrixMath::identity() * rotMat;

                    ShaderBufferFormat data;
                    data.mvp = canvas.getViewProjectionMatrix() * model;

                    auto color = line.color.divide();
                    data.color[0] = color.x;
                    data.color[1] = color.y;
                    data.color[2] = color.z;
                    data.color[3] = color.w;

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    auto &draw = meshBuffer.getLine(line.start, line.end);
                    ctx.drawIndexed(draw.drawCall, draw.baseVertex);
                    break;
                }
                case Paint::PAINT_RECTANGLE: {
                    if (currentPrimitive != TRIANGLES) {
                        currentPrimitive = TRIANGLES;
                        ctx.bindPipeline(trianglePipeline);
                        ctx.bindVertexBuffer(vertexBuffer);
                        ctx.bindIndexBuffer(indexBuffer);
                        ctx.bindShaderBuffer("vars", shaderBuffer);
                        ctx.bindTexture("atlasTextures", textures);
                    }
                    auto &square = std::get<PaintRectangle>(paintCommand.data);
                    Mat4f rotMat = MatrixMath::identity();

                    if (square.rotation != 0) {
                        rotMat = MatrixMath::translate({
                                     square.center.x,
                                     square.center.y,
                                     0
                                 })
                                 * MatrixMath::rotate(Vec3f(0, 0, square.rotation))
                                 * MatrixMath::translate({
                                     -square.center.x,
                                     -square.center.y,
                                     0
                                 });
                    }

                    auto model = MatrixMath::translate({
                                     square.dstRect.position.x,
                                     square.dstRect.position.y,
                                     0
                                 }) * rotMat;

                    ShaderBufferFormat data;
                    data.mvp = canvas.getViewProjectionMatrix() * model;

                    auto color = square.color.divide();
                    data.color[0] = color.x;
                    data.color[1] = color.y;
                    data.color[2] = color.z;
                    data.color[3] = color.w;

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    MeshBuffer2D::MeshDrawData draw;
                    if (square.fill) {
                        draw = meshBuffer.getPlane(square.dstRect.dimensions);
                    } else {
                        draw = meshBuffer.getSquare(square.dstRect.dimensions);
                    }

                    ctx.drawIndexed(draw.drawCall, draw.baseVertex);

                    break;
                }
                case Paint::PAINT_IMAGE: {
                    if (currentPrimitive != TRIANGLES) {
                        currentPrimitive = TRIANGLES;
                        ctx.bindPipeline(trianglePipeline);
                        ctx.bindVertexBuffer(vertexBuffer);
                        ctx.bindIndexBuffer(indexBuffer);
                        ctx.bindShaderBuffer("vars", shaderBuffer);
                        ctx.bindTexture("atlasTextures", textures);
                    }
                    auto &img = std::get<PaintImage>(paintCommand.data);
                    Mat4f rotMat = MatrixMath::identity();

                    if (img.rotation != 0) {
                        rotMat = MatrixMath::translate({
                                     img.center.x,
                                     img.center.y,
                                     0
                                 })
                                 * MatrixMath::rotate(Vec3f(0, 0, img.rotation))
                                 * MatrixMath::translate({
                                     -img.center.x,
                                     -img.center.y,
                                     0
                                 });
                    }

                    auto model = MatrixMath::translate({
                                     img.dstRect.position.x,
                                     img.dstRect.position.y,
                                     0
                                 }) * rotMat;

                    ShaderBufferFormat data;
                    data.mvp = canvas.getViewProjectionMatrix() * model;

                    auto color = img.mixColor.divide();
                    data.color[0] = color.x;
                    data.color[1] = color.y;
                    data.color[2] = color.z;
                    data.color[3] = color.w;

                    auto atlasTex = textureAtlasHandles.at(img.image.getUri());

                    data.colorMixFactor = img.mix;
                    data.alphaMixFactor = img.alphaMix;
                    data.colorFactor = 0;
                    data.texAtlasLevel = atlasTex.level;
                    data.texAtlasIndex = static_cast<int>(atlasTex.index);
                    data.texFilter = img.filter;

                    auto uvOffset = img.srcRect.position / atlasTex.size.convert<float>();
                    data.uvOffset_uvScale[0] = uvOffset.x;
                    data.uvOffset_uvScale[1] = uvOffset.y;

                    auto uvScale = (img.srcRect.dimensions / atlasTex.size.convert<float>());
                    data.uvOffset_uvScale[2] = uvScale.x;
                    data.uvOffset_uvScale[3] = uvScale.y;

                    auto atlasScale = (atlasTex.size.convert<float>() /
                                       TextureAtlas::getResolutionLevelSize(
                                           atlasTex.level).convert<float>());
                    data.atlasScale_texSize[0] = atlasScale.x;
                    data.atlasScale_texSize[1] = atlasScale.y;
                    data.atlasScale_texSize[2] = static_cast<float>(atlasTex.size.x);
                    data.atlasScale_texSize[3] = static_cast<float>(atlasTex.size.y);

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    auto draw = meshBuffer.getPlane(img.dstRect.dimensions);

                    ctx.drawIndexed(draw.drawCall, draw.baseVertex);
                    break;
                }
                case Paint::PAINT_TEXT: {
                    if (currentPrimitive != TRIANGLES) {
                        currentPrimitive = TRIANGLES;
                        ctx.bindPipeline(trianglePipeline);
                        ctx.bindVertexBuffer(vertexBuffer);
                        ctx.bindIndexBuffer(indexBuffer);
                        ctx.bindShaderBuffer("vars", shaderBuffer);
                    }
                    auto &txt = std::get<PaintText>(paintCommand.data);

                    auto key = RenderText(txt.text.text, txt.color, txt.text.fontUri, txt.text.fontPixelSize,
                                          txt.text.parameters);

                    ctx.bindTexture("customTexture", textCache.at(key));

                    Mat4f rotMat = MatrixMath::identity();

                    auto model = MatrixMath::translate({
                                     txt.position.x,
                                     txt.position.y,
                                     0
                                 }) * rotMat;

                    ShaderBufferFormat data;
                    data.mvp = canvas.getViewProjectionMatrix() * model;

                    data.customTexture = 1;
                    data.colorMixFactor = 0;
                    data.alphaMixFactor = 0;
                    data.colorFactor = 0;
                    data.texFilter = 0;

                    ctx.uploadBuffer(shaderBuffer,
                                     reinterpret_cast<const uint8_t *>(&data),
                                     sizeof(ShaderBufferFormat),
                                     0);

                    auto draw = meshBuffer.getPlane(txt.text.size.convert<float>());

                    ctx.drawIndexed(draw.drawCall, draw.baseVertex);
                }
            }
        }
        ctx.endRenderPass();
    }
}
