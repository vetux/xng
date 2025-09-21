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

#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/renderpassscheduler.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/text/textlayout.hpp"

#include "xng/rendergraph/rendergraphruntime.hpp"
#include "xng/resource/resourcehandle.hpp"

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
         * The recorded commands are presented via the passed runtime.
         *
         * Provided for projects that exclusively use the renderer 2D and don't perform more complex rendering
         * via the canvas api.
         *
         * @param runtime
         */
        explicit Renderer2D(std::shared_ptr<RenderGraphRuntime> runtime);

        ~Renderer2D();

        /**
         * Begin rendering to the screen.
         *
         * Must be called before calling draw methods.
         *
         * @param clearColor If the clear color contains non 0 alpha, the screen is cleared before rendering.
         */
        void renderBegin(const ColorRGBA &clearColor = ColorRGBA::black(1, 0));

        void renderPresent();

        /**
         * Draw texture with another color optionally mixed in.
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
                  const ResourceHandle<ImageRGBA> &texture,
                  const Vec2f &center,
                  float rotation,
                  bool filter,
                  float mixRGB = 0,
                  float mixAlpha = 0,
                  const ColorRGBA &mixColor = {});

        /**
         * Draw text
         *
         * @param position
         * @param text
         * @param color
         * @param center
         * @param rotation
         */
        void draw(const Vec2f &position, const TextLayout &text, const ColorRGBA &color);

        /**
         * Draw a rectangle
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
        void draw(const Vec2f &start, const Vec2f &end, const ColorRGBA &color);

        /**
         * Draw point
         *
         * @param point
         * @param color
         * @param position
         * @param center
         * @param rotation
         */
        void draw(const Vec2f &point, const ColorRGBA &color = {});

    private:
        bool isRendering = false;

        Canvas canvas;

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        std::shared_ptr<RenderGraphRuntime> runtime;
        std::unique_ptr<RenderPassScheduler> scheduler;

        RenderGraphHandle graph{};
    };
}

#endif //XENGINE_RENDERER2D_HPP
