/**
 *  xEngine - C++ Game Engine Library
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

#include "xng/shader/shadersource.hpp"
#include "xng/shader/shadercompiler.hpp"

#include "xng/render/camera.hpp"

#include "xng/util/hashcombine.hpp"

#include "xng/render/atlas/textureatlas.hpp"

#include "xng/render/geometry/vertexstream.hpp"

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
        explicit Renderer2D(RenderDevice &device, ShaderCompiler &shaderCompiler, ShaderDecompiler &shaderDecompiler);

        ~Renderer2D();

        TextureAtlasHandle createTexture(const ImageRGBA &texture);

        std::vector<TextureAtlasHandle> createTextures(const std::vector<ImageRGBA> &textures);

        void destroyTexture(const TextureAtlasHandle &handle);

        void renderClear(RenderTarget &target,
                         const ColorRGBA &clearColor,
                         const Vec2i &viewportOffset,
                         const Vec2i &viewportSize) {
            renderBegin(target, true, clearColor, viewportOffset, viewportSize, {});
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
                         const Vec2f &cameraPosition) {
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
        void renderBegin(RenderTarget &target, ColorRGBA clearColor = ColorRGBA::black(1, 0)) {
            renderBegin(target, true, clearColor, {}, target.getDescription().size, {});
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
         * @param filter
         * @param mix
         * @param mixAlpha
         * @param mixColor
         */
        void draw(const Rectf &srcRect,
                  const Rectf &dstRect,
                  TextureAtlasHandle &sprite,
                  const Vec2f &center,
                  float rotation,
                  TextureFiltering filter,
                  float mix,
                  float mixAlpha,
                  ColorRGBA mixColor);

        /**
         * Draw texture where each fragment color = textureColor * colorFactor
         *
         * @param srcRect The part of the of texture to sample
         * @param dstRect The part of the screen to display the sampled part into
         * @param sprite
         * @param center
         * @param rotation
         * @param filter
         * @param mix
         * @param mixAlpha
         * @param mixColor
         */
        void draw(const Rectf &srcRect,
                  const Rectf &dstRect,
                  TextureAtlasHandle &sprite,
                  const Vec2f &center,
                  float rotation,
                  TextureFiltering filter,
                  ColorRGBA colorFactor = ColorRGBA::white());

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
        void draw(const Vec2f &point,
                  ColorRGBA color = {},
                  const Vec2f &position = {},
                  const Vec2f &center = {},
                  float rotation = 0);

        RenderDevice &getDevice() { return renderDevice; }

    private:
        void updateAtlasRef();

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

            TextureFiltering filter;

            float mix = 0;
            float alphaMix = 0;
            ColorRGBA color;

            bool colorFactor = false;

            Pass() = default;

            Pass(Vec2f point,
                 ColorRGBA color,
                 Vec2f position,
                 Vec2f center,
                 float rotation)
                    : type(COLOR_POINT),
                      srcRect(std::move(position), {}),
                      dstRect(std::move(point), {}),
                      color(color),
                      center(std::move(center)),
                      rotation(rotation) {}

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
                 TextureFiltering filter,
                 float mix,
                 float alphaMix,
                 ColorRGBA color)
                    : type(TEXTURE),
                      srcRect(std::move(srcRect)),
                      dstRect(std::move(dstRect)),
                      center(std::move(center)),
                      rotation(rotation),
                      filter(filter),
                      texture(std::move(texture)),
                      mix(mix),
                      alphaMix(alphaMix),
                      color(color) {}

            Pass(Rectf srcRect,
                 Rectf dstRect,
                 TextureAtlasHandle &texture,
                 Vec2f center,
                 float rotation,
                 TextureFiltering filter,
                 ColorRGBA color)
                    : type(TEXTURE),
                      srcRect(std::move(srcRect)),
                      dstRect(std::move(dstRect)),
                      center(std::move(center)),
                      rotation(rotation),
                      filter(filter),
                      texture(std::move(texture)),
                      color(color),
                      colorFactor(true) {}
        };

        struct MeshDrawData {
            Primitive primitive;
            DrawCall drawCall;
            size_t baseVertex;
        };

        struct BufferRange {
            size_t start;
            size_t size;
        };

        void present();

        void presentMultiDraw();

        MeshDrawData getPlane(const Vec2f &size);

        MeshDrawData getSquare(const Vec2f &size);

        MeshDrawData getLine(const Vec2f &start, const Vec2f &end);

        MeshDrawData getPoint(const Vec2f &point);

        void destroyPlane(const Vec2f &size);

        void destroySquare(const Vec2f &size);

        void destroyLine(const Vec2f &start, const Vec2f &end);

        void destroyPoint(const Vec2f &point);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateVertexData(size_t size);

        void deallocateVertexData(size_t offset);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateIndexData(size_t size);

        void deallocateIndexData(size_t offset);

        void mergeFreeVertexBufferRanges();

        void mergeFreeIndexBufferRanges();

        void updateVertexArrayObject();

        Mat4f getRotationMatrix(float rotation, Vec2f center);

        class RotationPairHash {
        public:
            std::size_t operator()(const std::pair<float, Vec2f> &k) const {
                size_t ret = 0;
                hash_combine(ret, k.first);
                hash_combine(ret, k.second);
                return ret;
            }
        };

        class LinePairHash {
        public:
            std::size_t operator()(const std::pair<Vec2f, Vec2f> &k) const {
                size_t ret = 0;
                hash_combine(ret, k.first);
                hash_combine(ret, k.second);
                return ret;
            }
        };

        RenderDevice &renderDevice;

        ShaderSource vsTexture;
        ShaderSource fsTexture;

        ShaderSource vsTextureMultiDraw;
        ShaderSource fsTextureMultiDraw;

        std::unique_ptr<RenderPipeline> trianglePipeline;
        std::unique_ptr<RenderPipeline> linePipeline;
        std::unique_ptr<RenderPipeline> pointPipeline;

        std::unique_ptr<RenderPipeline> trianglePipelineMultiDraw;
        std::unique_ptr<RenderPipeline> linePipelineMultiDraw;
        std::unique_ptr<RenderPipeline> pointPipelineMultiDraw;

        std::unique_ptr<RenderPass> renderPass;

        std::unique_ptr<CommandBuffer> commandBuffer;

        std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> atlasRef;
        std::map<TextureAtlasResolution, std::unique_ptr<TextureArrayBuffer>> atlasTextures;
        TextureAtlas atlas;

        std::unordered_map<Vec2f, MeshDrawData> planeMeshes;
        std::unordered_map<Vec2f, MeshDrawData> squareMeshes;
        std::unordered_map<std::pair<Vec2f, Vec2f>, MeshDrawData, LinePairHash> lineMeshes;
        std::unordered_map<Vec2f, MeshDrawData> pointMeshes;

        std::map<size_t, size_t> freeVertexBufferRanges; // start and size of free ranges of vertices with layout vertexLayout in the vertex buffer
        std::map<size_t, size_t> freeIndexBufferRanges; // start and size of free ranges of bytes in the index buffer

        std::map<size_t, size_t> allocatedVertexRanges;
        std::map<size_t, size_t> allocatedIndexRanges;

        std::unordered_set<Vec2f> usedPlanes;
        std::unordered_set<Vec2f> usedSquares;
        std::unordered_set<std::pair<Vec2f, Vec2f>, LinePairHash> usedLines;
        std::unordered_set<Vec2f> usedPoints;

        std::unique_ptr<IndexBuffer> indexBuffer;
        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<VertexArrayObject> vertexArrayObject;

        std::unordered_map<std::pair<float, Vec2f>, Mat4f, RotationPairHash> rotationMatrices;
        std::unordered_set<std::pair<float, Vec2f>, RotationPairHash> usedRotationMatrices;

        std::vector<Pass> passes;

        RenderTarget *mTarget = nullptr;

        Camera camera;
        Transform cameraTransform;

        Mat4f viewProjectionMatrix;

        bool isRendering = false;

        bool vaoChange = false;

        Vec2i mViewportOffset = {};
        Vec2i mViewportSize = Vec2i(1);

        bool mClear = false;

        ColorRGBA mClearColor;

        VertexLayout vertexLayout;
    };
}

#endif //XENGINE_RENDERER2D_HPP
