/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "gpu/renderdevice.hpp"

#include "shader/shadersource.hpp"
#include "shader/spirvcompiler.hpp"

#include "asset/camera.hpp"

#include "util/hashcombine.hpp"

#include "text/text.hpp"

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
     *
     * Resources which are passed by the user (Texture buffers, shaders etc.) shall not be deallocated until
     * render finish is called.
     */
    class XENGINE_EXPORT Renderer2D {
    public:
        explicit Renderer2D(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &shaderDecompiler);

        ~Renderer2D();

        /**
         * Must be called before calling draw methods.
         *
         * @param target
         * @param clear
         * @param clearColor
         */
        void renderBegin(RenderTarget &target, bool clear = true, ColorRGBA clearColor = ColorRGBA::black());

        void renderBegin(RenderTarget &target,
                         bool clear,
                         ColorRGBA clearColor,
                         Vec2i viewportOffset,
                         Vec2i viewportSize);

        /**
         * Present the recorded drawing commands to the target specified in renderBegin.
         */
        void renderPresent();

        void renderClear(RenderTarget &target,
                         ColorRGBA clearColor,
                         Vec2i viewportOffset,
                         Vec2i viewportSize);

        /**
         * Set the projection bounds.
         * The projection bounds are set to 0,0 and target size when calling renderBegin
         *
         * @param projection The projection bounds to set
         */
        void setProjection(const Rectf &projection);

        void setCameraPosition(const Vec2f &pos);

        /**
         * Draw texture
         *
         * @param srcRect The part of the of texture to sample
         * @param dstRect The part of the screen to display the sampled part into
         * @param texture
         * @param center
         * @param rotation
         * @param flipUv
         * @param mix
         * @param mixColor
         */
        void draw(Rectf srcRect,
                  Rectf dstRect,
                  TextureBuffer &texture,
                  Vec2f center = {},
                  float rotation = 0,
                  Vec2b flipUv = Vec2b(false),
                  float mix = 0,
                  ColorRGB mixColor = ColorRGB());

        /**
         * Draw a texture which is the result of blending between textureA and textureB with the progress indicating blend (0 - 1)
         *
         * textureA -------- | --- textureB
         *                  blendScale
         *
         * @param srcRect
         * @param dstRect
         * @param textureA
         * @param textureB
         * @param blendScale
         * @param center
         * @param rotation
         * @param flipUv
         */
        void draw(Rectf srcRect,
                  Rectf dstRect,
                  TextureBuffer &textureA,
                  TextureBuffer &textureB,
                  float blendScale,
                  Vec2f center = {},
                  float rotation = 0,
                  Vec2b flipUv = Vec2b(false));

        /**
        * Draw Polygon
        *
        * @param poly
        * @param position
        * @param color
        * @param fill
        * @param center
        * @param rotation
        */
        void draw(std::vector<Vec2f> poly,
                  Vec2f position,
                  ColorRGBA color,
                  Vec2f center = {},
                  float rotation = 0);

        /**
         * Draw rectangle
         *
         * @param rectangle
         * @param color
         * @param fill
         * @param center
         * @param rotation
         */
        void draw(Rectf rectangle,
                  ColorRGBA color,
                  bool fill = true,
                  Vec2f center = {},
                  float rotation = 0);

        /**
         * Draw line
         *
         * @param start
         * @param end
         * @param color
         */
        void draw(Vec2f start, Vec2f end, ColorRGBA color);

        /**
         * Draw point
         *
         * @param point
         * @param color
         */
        void draw(Vec2f point, ColorRGBA color = {});

        /**
         * Draw text
         *
         * @param text
         * @param dstRect
         * @param color
         * @param center
         * @param rotation
         */
        void draw(Text &text, Rectf srcRect, Rectf dstRect, ColorRGBA color, Vec2f center = {}, float rotation = 0);

        RenderDevice &getDevice() { return renderDevice; }

    private:
        struct PlaneDescription {
            Vec2f size;
            Vec2f center;
            Rectf uvOffset;
            Vec2f uvSize;
            Vec2b flipUv;

            bool operator==(const PlaneDescription &other) const {
                return size == other.size
                       && center == other.center
                       && uvOffset == other.uvOffset
                       && flipUv == other.flipUv;
            }
        };

        class PlaneDescriptionHashFunction {
        public:
            size_t operator()(const PlaneDescription &p) const {
                size_t ret;
                hash_combine(ret, p.size.x);
                hash_combine(ret, p.size.y);
                hash_combine(ret, p.center.x);
                hash_combine(ret, p.center.y);
                hash_combine(ret, p.uvOffset.position.x);
                hash_combine(ret, p.uvOffset.position.y);
                hash_combine(ret, p.uvSize.x);
                hash_combine(ret, p.uvSize.y);
                hash_combine(ret, p.flipUv.x);
                hash_combine(ret, p.flipUv.y);
                return ret;
            }
        };

        struct SquareDescription {
            Vec2f size;
            Vec2f center;

            bool operator==(const SquareDescription &other) const {
                return size == other.size
                       && center == other.center;
            }
        };

        class SquareDescriptionHashFunction {
        public:
            size_t operator()(const SquareDescription &p) const {
                size_t ret;
                hash_combine(ret, p.size.x);
                hash_combine(ret, p.size.y);
                hash_combine(ret, p.center.x);
                hash_combine(ret, p.center.y);
                return ret;
            }
        };

        struct LineDescription {
            Vec2f start;
            Vec2f end;
            Vec2f center;

            bool operator==(const LineDescription &other) const {
                return start == other.start
                       && end == other.end
                       && center == other.center;
            }
        };

        class LineDescriptionHashFunction {
        public:
            size_t operator()(const LineDescription &p) const {
                size_t ret;
                hash_combine(ret, p.start.x);
                hash_combine(ret, p.start.y);
                hash_combine(ret, p.end.x);
                hash_combine(ret, p.end.y);
                hash_combine(ret, p.center.x);
                hash_combine(ret, p.center.y);
                return ret;
            }
        };

        template<typename T>
        class PolyHashFunction {
        public:
            size_t operator()(const std::vector<Vector2<T>> &p) const {
                size_t ret;
                for (auto &v: p) {
                    hash_combine(ret, v.x);
                    hash_combine(ret, v.y);
                }
                return ret;
            }
        };

        void reallocatePipelines();

        VertexBuffer &getPoly(const std::vector<Vec2f> &poly);

        VertexBuffer &getPlane(const PlaneDescription &desc);

        std::vector<Vec2f> getSquare(const SquareDescription &desc);

        ShaderBuffer &getShaderBuffer();

        RenderDevice &renderDevice;

        ShaderSource vs;
        ShaderSource fsColor;
        ShaderSource fsTexture;
        ShaderSource fsText;
        ShaderSource fsBlend;

        std::unique_ptr<ShaderProgram> colorShader = nullptr;
        std::unique_ptr<ShaderProgram> textureShader = nullptr;
        std::unique_ptr<ShaderProgram> textShader = nullptr;
        std::unique_ptr<ShaderProgram> blendShader = nullptr;

        std::unique_ptr<RenderPipeline> clearPipeline;
        std::unique_ptr<RenderPipeline> colorPipeline;
        std::unique_ptr<RenderPipeline> texturePipeline;
        std::unique_ptr<RenderPipeline> textPipeline;
        std::unique_ptr<RenderPipeline> textureBlendPipeline;

        std::unordered_map<PlaneDescription, std::unique_ptr<VertexBuffer>, PlaneDescriptionHashFunction> allocatedPlanes;
        std::unordered_map<std::vector<Vec2f>, std::unique_ptr<VertexBuffer>, PolyHashFunction<float>> allocatedPolys;

        std::unordered_set<PlaneDescription, PlaneDescriptionHashFunction> usedPlanes;
        std::unordered_set<std::vector<Vec2f>, PolyHashFunction<float>> usedPolys;

        std::vector<std::unique_ptr<ShaderBuffer>> shaderBuffers;

        size_t usedShaderBuffers = 0;

        struct Pass {
            enum Type {
                COLOR,
                COLOR_SOLID,
                TEXTURE,
                TEXTURE_BLEND,
                TEXT
            } type{};

            Vec2f position;
            float rotation = 0;
            ColorRGBA color;
            TextureBuffer *texture = nullptr;
            TextureBuffer *textureB = nullptr;
            float blendScale = 0;
            std::variant<PlaneDescription, std::vector<Vec2f>> geometry;
            Camera camera;
            Transform cameraTransform;
            float mix = 0;
            ColorRGB mixColor;

            Pass() = default;

            Pass(Vec2f position,
                 float rotation,
                 std::vector<Vec2f> poly,
                 ColorRGBA color,
                 Camera camera,
                 Transform cameraTransform)
                    : type(COLOR),
                      position(std::move(position)),
                      rotation(rotation),
                      geometry(poly),
                      color(color),
                      camera(std::move(camera)),
                      cameraTransform(std::move(cameraTransform)) {}

            Pass(Vec2f position,
                 float rotation,
                 PlaneDescription plane,
                 ColorRGBA color,
                 Camera camera,
                 Transform cameraTransform)
                    : type(COLOR_SOLID),
                      position(std::move(position)),
                      rotation(rotation),
                      geometry(plane),
                      color(color),
                      camera(std::move(camera)),
                      cameraTransform(std::move(cameraTransform)) {}

            Pass(Vec2f position,
                 float rotation,
                 PlaneDescription plane,
                 TextureBuffer &texture,
                 Camera camera,
                 Transform cameraTransform,
                 float mix,
                 ColorRGB mixColor)
                    : type(TEXTURE),
                      position(std::move(position)),
                      rotation(rotation),
                      geometry(plane),
                      texture(&texture),
                      camera(std::move(camera)),
                      cameraTransform(std::move(cameraTransform)),
                      mix(mix),
                      mixColor(mixColor) {}

            Pass(Vec2f position,
                 float rotation,
                 PlaneDescription plane,
                 TextureBuffer &textureA,
                 TextureBuffer &textureB,
                 float blendScale,
                 Camera camera,
                 Transform cameraTransform)
                    : type(TEXTURE_BLEND),
                      position(std::move(position)),
                      rotation(rotation),
                      geometry(plane),
                      texture(&textureA),
                      textureB(&textureB),
                      blendScale(blendScale),
                      camera(std::move(camera)),
                      cameraTransform(std::move(cameraTransform)) {}

            Pass(Vec2f position,
                 float rotation,
                 PlaneDescription plane,
                 Text &text,
                 ColorRGBA color,
                 Camera camera,
                 Transform cameraTransform)
                    : type(TEXT),
                      position(std::move(position)),
                      rotation(rotation),
                      geometry(plane),
                      texture(&text.getTexture()),
                      color(color),
                      camera(std::move(camera)),
                      cameraTransform(std::move(cameraTransform)) {}
        };

        std::vector<Pass> passes;

        RenderTarget *userTarget = nullptr;

        Camera camera;
        Transform cameraTransform;

        bool isRendering = false;

        int layer = 0;

        bool clear = false;
        ColorRGBA clearColor = ColorRGBA::black();
        Vec2i viewportOffset = {};
        Vec2i viewportSize = Vec2i(1);
    };
}

#endif //XENGINE_RENDERER2D_HPP
