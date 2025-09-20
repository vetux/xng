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

#ifndef XENGINE_RENDERER2D_HPP
#define XENGINE_RENDERER2D_HPP

#include <vector>

#include "text/text.hpp"

#include "xng/graphics/2d/texture2d.hpp"
#include "xng/graphics/2d/renderbatch2d.hpp"

#include "xng/graphics/3d/renderpassscheduler.hpp"
#include "xng/graphics/3d/passes/renderpass2d.hpp"

#include "xng/rendergraph/rendergraphruntime.hpp"

namespace xng {
    /**
     * This is a SDL inspired 2d renderer.
     * It uses a similar interface to SDL in a object oriented manner.
     *
     * The following coordinate system is used:
     *      -Y
     *       |
     * -X - -|- - +X
     *       |
     *      +Y
     *
     * with boundary of 0,0 at the top left ond targetsize.x,targetsize.y at the bottom right.
     *
     * The Renderer2D ignores depth information stored in the render target.
     * The order in which methods are invoked controls which elements are drawn below / above.
     *
     * For example when drawing a rectangle texture first and then drawing a triangle texture afterwards
     * the triangle will be drawn on top of the rectangle.
     *
     * Drawing operations support blending between draw objects and previous target contents.
     */
    class XENGINE_EXPORT Renderer2D {
    public:
        /**
         * The renderer does not present recorded batches.
         */
        Renderer2D() = default;

        /**
         * The recorded batches are presented via the passed runtime.
         *
         * Provided for projects that exclusively use the renderer 2D and don't perform more complex rendering.
         *
         * @param runtime
         */
        explicit Renderer2D(std::shared_ptr<RenderGraphRuntime> runtime);

        ~Renderer2D();

        Texture2D createTexture(const ImageRGBA &texture);

        std::vector<Texture2D> createTextures(const std::vector<ImageRGBA> &textures);

        void destroyTexture(const Texture2D &texture);

        /**
         * Begin rendering to the screen.
         *
         * Must be called before calling draw methods.
         *
         * @param clear
         * @param clearColor
         * @param viewportOffset
         * @param viewportSize
         * @param projection
         * @param cameraPosition
         */
        void renderBegin(bool clear,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize,
                         const Vec2f &cameraPosition,
                         const Rectf &projection);

        /**
         * Begin rendering to a texture
         *
         * @param target
         * @param clear
         * @param clearColor
         * @param viewportOffset
         * @param viewportSize
         * @param cameraPosition
         * @param projection
         */
        void renderBegin(const Texture2D &target,
                         bool clear,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize,
                         const Vec2f &cameraPosition,
                         const Rectf &projection);

        void renderBegin(const bool clear,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize,
                         const Vec2f &cameraPosition,
                         const Vec2i &frameBufferSize) {
            renderBegin(clear,
                        clearColor,
                        viewportOffset,
                        viewportSize,
                        cameraPosition,
                        Rectf({}, frameBufferSize.convert<float>()));
        }

        void renderBegin(const Vec2i &frameBufferSize, const ColorRGBA clearColor = ColorRGBA::black(1, 0)) {
            renderBegin(true,
                        clearColor,
                        {},
                        frameBufferSize,
                        {},
                        Rectf({}, frameBufferSize.convert<float>()));
        }

        void renderPresent();

        /**
         * Draw texture where each fragment color = textureColor * colorFactor.
         *
         * The colorFactor usually is white so the texture color is unaffected,
         * for rendering text where the image is a grayscale image the color factor can be used to tint the text.
         *
         * @param srcRect The part of the of texture to sample
         * @param dstRect The part of the screen to display the sampled part into
         * @param texture
         * @param center
         * @param rotation
         * @param filter
         * @param colorFactor
         */
        void draw(const Rectf &srcRect,
                  const Rectf &dstRect,
                  const Texture2D &texture,
                  const Vec2f &center,
                  float rotation,
                  TextureFiltering filter,
                  ColorRGBA colorFactor = ColorRGBA::white());

        void draw(const Vec2f &position,
                  const Texture2D &texture,
                  const TextureFiltering filter = NEAREST,
                  const Vec2f &center = {},
                  const float rotation = {}) {
            const auto size = texture.getSize().convert<float>();
            draw(Rectf({}, size),
                 Rectf(position, size),
                 texture,
                 center,
                 rotation,
                 filter);
        }

        /**
         * Draw texture with a color mixed in.
         *
         * @param srcRect The part of the of texture to sample
         * @param dstRect The part of the screen to display the sampled part into
         * @param texture
         * @param center
         * @param rotation
         * @param filter
         * @param mixRGB
         * @param mixAlpha How much of mixColor.rgb to mix in with the texture color (0 - 1)
         * @param mixColor How much of mixColor.a to mix in with the texture alpha (0 - 1)
         */
        void draw(const Rectf &srcRect,
                  const Rectf &dstRect,
                  const Texture2D &texture,
                  const Vec2f &center,
                  float rotation,
                  TextureFiltering filter,
                  float mixRGB,
                  float mixAlpha,
                  const ColorRGBA &mixColor);

        void draw(const Vec2f &position,
                  const Text &text,
                  const ColorRGBA &color,
                  const Vec2f &center = {},
                  float rotation = 0) {
            auto tex = text.getTexture();
            draw(Rectf({}, tex.getSize().convert<float>()),
                 Rectf(position, tex.getSize().convert<float>()),
                 tex,
                 center,
                 rotation,
                 NEAREST,
                 1,
                 0,
                 color);
        }

        /**
         * Draw rectangle
         *
         * @param rectangle
         * @param color
         * @param fill
         * @param center
         * @param rotation
         */
        void draw(const Rectf &rectangle,
                  const ColorRGBA &color,
                  bool fill = true,
                  const Vec2f &center = {},
                  float rotation = 0);

        /**
         * Draw line
         *
         * @param start
         * @param end
         * @param color
         * @param position
         * @param center
         * @param rotation
         */
        void draw(const Vec2f &start,
                  const Vec2f &end,
                  const ColorRGBA &color,
                  const Vec2f &position = {},
                  const Vec2f &center = {},
                  float rotation = 0);

        /**
         * Draw point
         *
         * @param point
         * @param color
         * @param position
         * @param center
         * @param rotation
         */
        void draw(const Vec2f &point,
                  const ColorRGBA &color = {},
                  const Vec2f &position = {},
                  const Vec2f &center = {},
                  float rotation = 0);

        const std::vector<RenderBatch2D> &getRenderBatches() const {
            return renderBatches;
        }

        void clearBatches() {
            renderBatches.clear();
        }

    private:
        bool isRendering = false;
        RenderBatch2D batch;
        std::vector<RenderBatch2D> renderBatches;

        size_t textureHandleCounter = 0;
        std::vector<size_t> unusedTextureHandles;

        std::shared_ptr<RenderConfiguration> config = nullptr;
        std::shared_ptr<SharedResourceRegistry> registry = nullptr;

        std::shared_ptr<RenderGraphRuntime> runtime = nullptr;
        std::shared_ptr<RenderPass2D> renderPass = nullptr;
        std::unique_ptr<RenderPassScheduler> scheduler = nullptr;

        RenderGraphHandle graph{};
    };
}

#endif //XENGINE_RENDERER2D_HPP
