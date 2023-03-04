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

#ifndef XENGINE_RENDERER2D_HPP
#define XENGINE_RENDERER2D_HPP

#include <set>
#include <unordered_set>
#include <utility>
#include <variant>

#include "xng/gpu/renderdevice.hpp"

#include "xng/asset/shadersource.hpp"
#include "xng/shader/spirvcompiler.hpp"

#include "xng/asset/camera.hpp"

#include "xng/util/hashcombine.hpp"

#include "xng/render/textureatlas.hpp"

#include "xng/geometry/vertexstream.hpp"

namespace xng {
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
     */
    class XENGINE_EXPORT Renderer2D {
    public:
        explicit Renderer2D(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &shaderDecompiler);

        ~Renderer2D();

        TextureAtlasHandle createTexture(const ImageRGBA &texture);

        std::vector<TextureAtlasHandle> createTextures(const std::vector<ImageRGBA> &textures);

        void destroyTexture(const TextureAtlasHandle &handle);

        void renderClear(RenderTarget &target,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize) {
            renderBegin(target, true, clearColor, std::move(viewportOffset), std::move(viewportSize));
            renderPresent();
        }

        void renderClear(RenderTarget &target, const ColorRGBA &clearColor) {
            renderClear(target, clearColor, {}, target.getDescription().size);
        }

        /**
         *
         * @param target
         * @param clear
         * @param clearColor
         * @param viewportOffset
         * @param viewportSize
         * @param projection
         * @param cameraPosition
         */
        void renderBegin(RenderTarget &target,
                         bool clear,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize,
                         const Vec2f &cameraPosition,
                         const Rectf &projection);

        void renderBegin(RenderTarget &target,
                         bool clear,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize,
                         const Vec2f &cameraPosition = {}) {
            renderBegin(target,
                        clear,
                        clearColor,
                        viewportOffset,
                        viewportSize,
                        cameraPosition,
                        Rectf({}, target.getDescription().size.convert<float>()));
        }

        /**
         * Must be called before calling draw methods.
         *
         * @param target
         * @param clear
         * @param clearColor
         */
        void renderBegin(RenderTarget &target, bool clear = true, ColorRGBA clearColor = ColorRGBA::black()) {
            renderBegin(target, clear, clearColor, {}, target.getDescription().size);
        }

        /**
         * Present the recorded drawing commands to the target specified in renderBegin.
         */
        void renderPresent();

        /**
         * Draw texture
         *
         * @param srcRect The part of the of texture to sample
         * @param dstRect The part of the screen to display the sampled part into
         * @param sprite
         * @param center
         * @param rotation
         * @param mix
         * @param mixColor
         */
        void draw(const Rectf &srcRect,
                  const Rectf &dstRect,
                  TextureAtlasHandle &sprite,
                  const Vec2f &center = {},
                  float rotation = 0,
                  float mix = 0,
                  ColorRGBA mixColor = ColorRGBA());

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
                  ColorRGBA color,
                  bool fill = true,
                  const Vec2f &center = {},
                  float rotation = 0);

        /**
         * Draw line
         *
         * @param start
         * @param end
         * @param position
         * @param center
         * @param rotation
         * @param color
         */
        void draw(const Vec2f &start,
                  const Vec2f &end,
                  ColorRGBA color,
                  const Vec2f &position = {},
                  const Vec2f &center = {},
                  float rotation = 0);

        /**
         * Draw point
         *
         * @param point
         * @param color
         */
        void draw(const Vec2f &point, ColorRGBA color = {});

        RenderDevice &getDevice() { return renderDevice; }

    private:
        void rebindTextureAtlas(const std::map<TextureAtlasResolution, std::vector<bool>> &occupations);

        RenderDevice &renderDevice;

        ShaderSource vsTexture;
        ShaderSource fsTexture;

        std::unique_ptr<RenderPipeline> trianglePipeline;
        std::unique_ptr<RenderPipeline> linePipeline;
        std::unique_ptr<RenderPipeline> pointPipeline;

        std::unique_ptr<RenderPass> renderPass;

        std::map<TextureAtlasResolution, std::unique_ptr<TextureArrayBuffer>> atlasTextures;
        TextureAtlas atlas;

        struct Pass {
            enum Type {
                COLOR_POINT,
                COLOR_LINE,
                COLOR_PLANE,
                TEXTURE,
            } type{};

            Rectf srcRect;
            Rectf dstRect; // If line dstRect.position contains the start and dstRect.dimensions the end of the line.

            Vec2f center;
            float rotation = 0;

            bool fill = false;

            TextureAtlasHandle texture;

            float mix = 0;
            ColorRGBA color;

            Pass() = default;

            Pass(Vec2f point,
                 ColorRGBA color)
                    : type(COLOR_POINT),
                      dstRect(std::move(point), {}),
                      color(color) {}

            Pass(Vec2f start,
                 Vec2f end,
                 ColorRGBA color,
                 Vec2f position,
                 Vec2f center,
                 float rotation)
                    : type(COLOR_LINE),
                      srcRect(std::move(position), {}),
                      dstRect(std::move(start), std::move(end)),
                      color(color),
                      center(std::move(center)),
                      rotation(rotation) {}

            Pass(Rectf dstRect,
                 ColorRGBA color,
                 bool fill,
                 Vec2f center,
                 float rotation)
                    : type(COLOR_PLANE),
                      dstRect(std::move(dstRect)),
                      center(std::move(center)),
                      rotation(rotation),
                      color(color),
                      fill(fill) {}

            Pass(Rectf srcRect,
                 Rectf dstRect,
                 TextureAtlasHandle &texture,
                 Vec2f center,
                 float rotation,
                 float mix,
                 ColorRGBA color)
                    : type(TEXTURE),
                      srcRect(std::move(srcRect)),
                      dstRect(std::move(dstRect)),
                      center(std::move(center)),
                      rotation(rotation),
                      texture(std::move(texture)),
                      mix(mix),
                      color(color) {}
        };

        std::vector<Pass> passes;

        RenderTarget *userTarget = nullptr;

        Camera camera;
        Transform cameraTransform;

        bool isRendering = false;

        Vec2i mViewportOffset = {};
        Vec2i mViewportSize = Vec2i(1);

        std::vector<VertexAttribute> vertexLayout;
    };
}

#endif //XENGINE_RENDERER2D_HPP
