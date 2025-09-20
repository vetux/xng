/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/graphics/2d/renderer2d.hpp"

#include <utility>

#include "xng/math/matrixmath.hpp"
#include "../../../include/xng/graphics/camera.hpp"

namespace xng {
    Renderer2D::Renderer2D(std::shared_ptr<RenderPassScheduler> scheduler)
        : renderPassScheduler(std::move(scheduler)),
          renderPass(std::make_shared<RenderPass2D>()) {
        graph = renderPassScheduler->addGraph(renderPass);
    }

    Renderer2D::~Renderer2D() {
        if (renderPassScheduler) {
            renderPassScheduler->destroy(graph);
        }
    }

    Texture2D Renderer2D::createTexture(const ImageRGBA &texture) {
        Texture2D::Handle handle;
        if (unusedTextureHandles.empty()) {
            handle = textureHandleCounter++;
        } else {
            handle = unusedTextureHandles.back();
            unusedTextureHandles.pop_back();
        }
        batch.textureAllocations[handle] = texture;
        return {handle, texture.getResolution()};
    }

    std::vector<Texture2D> Renderer2D::createTextures(const std::vector<ImageRGBA> &textures) {
        std::vector<Texture2D> ret;
        ret.reserve(textures.size());
        for (auto &img: textures) {
            ret.emplace_back(createTexture(img));
        }
        return ret;
    }

    void Renderer2D::destroyTexture(const Texture2D &texture) {
        batch.textureDeallocations.emplace_back(texture.getHandle());
        unusedTextureHandles.emplace_back(texture.getHandle());
    }

    void Renderer2D::renderBegin(const Texture2D &target,
                                 const bool clear,
                                 const ColorRGBA &clearColor,
                                 const Vec2i &viewportOffset,
                                 const Vec2i &viewportSize,
                                 const Vec2f &cameraPosition,
                                 const Rectf &projection) {
        if (isRendering) {
            throw std::runtime_error("Already Rendering (Nested Renderer2D::renderBegin calls?)");
        }

        isRendering = true;

        batch.mViewportOffset = viewportOffset;
        batch.mViewportSize = viewportSize;
        batch.mClear = clear;
        batch.mClearColor = clearColor;

        batch.camera.type = ORTHOGRAPHIC;
        batch.camera.left = projection.position.x;
        batch.camera.right = projection.dimensions.x;
        batch.camera.top = projection.position.y;
        batch.camera.bottom = projection.dimensions.y;

        batch.cameraTransform.setPosition({cameraPosition.x, cameraPosition.y, 1});

        batch.viewProjectionMatrix = batch.camera.projection() * Camera::view(batch.cameraTransform);

        batch.renderToScreen = false;
        batch.renderTarget = target.getHandle();
    }

    void Renderer2D::renderBegin(const bool clear,
                                 const ColorRGBA &clearColor,
                                 const Vec2i &viewportOffset,
                                 const Vec2i &viewportSize,
                                 const Vec2f &cameraPosition,
                                 const Rectf &projection) {
        if (isRendering) {
            throw std::runtime_error("Already Rendering (Nested Renderer2D::renderBegin calls?)");
        }

        isRendering = true;

        batch.mViewportOffset = viewportOffset;
        batch.mViewportSize = viewportSize;
        batch.mClear = clear;
        batch.mClearColor = clearColor;

        batch.camera.type = ORTHOGRAPHIC;
        batch.camera.left = projection.position.x;
        batch.camera.right = projection.dimensions.x;
        batch.camera.top = projection.position.y;
        batch.camera.bottom = projection.dimensions.y;

        batch.cameraTransform.setPosition({cameraPosition.x, cameraPosition.y, 1});

        batch.viewProjectionMatrix = batch.camera.projection() * Camera::view(batch.cameraTransform);
    }

    void Renderer2D::renderPresent() {
        if (!isRendering) {
            throw std::runtime_error("Not Rendering");
        }
        isRendering = false;
        renderBatches.emplace_back(batch);
        batch = {};

        if (renderPassScheduler) {
            renderPass->setBatches(renderBatches);
            renderPassScheduler->execute(graph);
            clearBatches();
        }
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          const Texture2D &texture,
                          const Vec2f &center,
                          float rotation,
                          TextureFiltering filter,
                          float mixRGB,
                          float mixAlpha,
                          const ColorRGBA &mixColor) {
        batch.drawCommands.emplace_back(srcRect,
                                        dstRect,
                                        texture.getHandle(),
                                        center,
                                        rotation,
                                        filter,
                                        mixRGB,
                                        mixAlpha,
                                        mixColor);
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          const Texture2D &texture,
                          const Vec2f &center,
                          float rotation,
                          TextureFiltering filter,
                          ColorRGBA colorFactor) {
        batch.drawCommands.emplace_back(srcRect,
                                        dstRect,
                                        texture.getHandle(),
                                        center,
                                        rotation,
                                        filter,
                                        colorFactor);
    }

    void Renderer2D::draw(const Rectf &rectangle, const ColorRGBA &color, bool fill, const Vec2f &center,
                          float rotation) {
        batch.drawCommands.emplace_back(rectangle, color, fill, center, rotation);
    }

    void Renderer2D::draw(const Vec2f &start,
                          const Vec2f &end,
                          const ColorRGBA &color,
                          const Vec2f &position,
                          const Vec2f &center,
                          float rotation) {
        batch.drawCommands.emplace_back(start, end, color, position, center, rotation);
    }

    void Renderer2D::draw(const Vec2f &point,
                          const ColorRGBA &color,
                          const Vec2f &position,
                          const Vec2f &center,
                          float rotation) {
        batch.drawCommands.emplace_back(point, color, position, center, rotation);
    }
}
