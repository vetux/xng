/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERER2D_HPP
#define XENGINE_RENDERER2D_HPP

#include <set>

#include "platform/graphics/renderer.hpp"
#include "platform/graphics/renderdevice.hpp"

#include "platform/text/character.hpp"
#include "platform/text/font.hpp"

#include "asset/camera.hpp"

namespace xengine {
    /**
     * This is a SDL inspired 2d renderer.
     * It uses a similar interface to SDL in a object oriented manner.
     *
     * The 2d renderer uses the following coordinate system:
     *      -Y
     *       |
     * -X - -|- - +X
     *       |
     *      +Y
     *
     * with default boundary of 0,0 at the top left ond targetsize.x,targetsize.y at the bottom right.
     *
     * The Renderer2D ignores depth information stored in the render target.
     * The order in which methods are invoked controls which elements are drawn below / above.
     *
     * For example when drawing a rectangle texture first and then drawing a triangle texture afterwards
     * the triangle will be drawn on top of the rectangle.
     *
     * Drawing operations support blending between draw objects and previous target contents.
     *
     * Resources which are passed by the user (Texture buffers, shaders etc.) shall not be deallocated until
     * render finish is called.
     */
    class XENGINE_EXPORT Renderer2D {
    public:
        explicit Renderer2D(RenderDevice &device);

        ~Renderer2D();

        void renderBegin(RenderTarget &target, bool clear);

        void renderBegin(RenderTarget &target,
                         bool clear,
                         Vec2i viewportOffset,
                         Vec2i viewportSize,
                         ColorRGBA clearColor = ColorRGBA(0, 0, 0, 0));

        /**
         * Set the projection bounds.
         * The projection bounds are set to 0,0 and target size when calling renderBegin
         *
         * @param projection The projection bounds to set
         */
        void setProjection(const Rectf &projection);

        void draw(Rectf srcRect,
                  Rectf dstRect,
                  TextureBuffer &texture,
                  ShaderProgram &shader,
                  Vec2f center = {},
                  float rotation = 0);

        void draw(Rectf srcRect,
                  Rectf dstRect,
                  TextureBuffer &texture,
                  Vec2f center = {},
                  float rotation = 0);

        void draw(Rectf dstRect, TextureBuffer &texture, Vec2f center = {}, float rotation = 0);

        void draw(Rectf rectangle,
                  ColorRGBA color,
                  bool fill = true,
                  Vec2f center = {},
                  float rotation = 0);

        void draw(Vec2f start, Vec2f end, ColorRGBA color, Vec2f center = {}, float rotation = 0);

        void draw(Vec2f point, ColorRGBA color = {});

        /**
         * @param position The origin of the text
         * @param text
         * @param color
         */
        void draw(Vec2f position,
                  const std::string &text,
                  ColorRGBA color,
                  std::map<char, Character> &characters,
                  std::map<char, std::unique_ptr<TextureBuffer>> &textures);

        void renderPresent();

    private:
        RenderDevice &renderDevice;

        ShaderSource vs;
        ShaderSource fs;
        ShaderSource fsText;

        std::unique_ptr<ShaderProgram> defaultShader = nullptr;
        std::unique_ptr<ShaderProgram> defaultTextShader = nullptr;

        std::set<std::unique_ptr<MeshBuffer>> allocatedMeshes;

        Vec2i screenSize;

        Camera camera;
    };
}

#endif //XENGINE_RENDERER2D_HPP
