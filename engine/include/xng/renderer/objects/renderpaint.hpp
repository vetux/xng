/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERPAINT_HPP
#define XENGINE_RENDERPAINT_HPP

#include <utility>

#include "xng/assets/color.hpp"
#include "xng/renderer/renderobject.hpp"

namespace xng {
    class RenderPaint final : public RenderObject {
    public:
        enum Type {
            PAINT_POINT,
            PAINT_LINE,
            PAINT_RECTANGLE,
            PAINT_TEXTURE
        };

        /**
         * Paint Point
         *
         * @param id
         * @param paintStream
         * @param meshStream
         * @param position
         * @param size
         * @param color
         */
        RenderPaint(const Id id,
                    BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                    MeshStreamer &meshStream,
                    Vec2f position,
                    float size,
                    const ColorRGBA &color)
            : RenderObject(OBJECT_PAINT, id),
              type(PAINT_POINT),
              paintStream(paintStream),
              slot(paintStream.create()),
              meshStream(meshStream),
              dstRect(std::move(position), {}),
              color(color) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
            meshHandle = meshStream.getNormalizedQuad();
            modelMatrix = MatrixMath::scale(Vec3f(size, size, 1))
                          * MatrixMath::translate(Vec3f(position.x, position.y, 0));
        }

        /**
         * Paint Line
         *
         * @param id
         * @param paintStream
         * @param meshStream
         * @param _start
         * @param _end
         * @param color
         * @param center
         * @param rotation
         */
        RenderPaint(const Id id,
                    BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                    MeshStreamer &meshStream,
                    Vec2f _start,
                    Vec2f _end,
                    const ColorRGBA &color,
                    Vec2f center = {},
                    const float rotation = 0)
            : RenderObject(OBJECT_PAINT, id),
              type(PAINT_LINE),
              paintStream(paintStream),
              slot(paintStream.create()),
              meshStream(meshStream),
              dstRect(std::move(_start), std::move(_end)),
              center(std::move(center)),
              rotation(rotation),
              color(color) {
            Mesh mesh;
            mesh.primitive = Mesh::TRIANGLES;
            mesh.positions.emplace_back(dstRect.position.x, dstRect.position.y, 1);
            mesh.positions.emplace_back(dstRect.dimensions.x, dstRect.dimensions.y, 1);
            mesh.positions.emplace_back(dstRect.dimensions.x, dstRect.dimensions.y, 1);
            meshHandle = meshStream.create(mesh);
            modelMatrix = MatrixMath::identity();

            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
        }

        /**
         * Paint rectangle (For non-filled rectangles use lines)
         *
         * @param id
         * @param paintStream
         * @param meshStream
         * @param _dstRect
         * @param color
         * @param _center
         * @param rotation
         */
        RenderPaint(const Id id,
                    BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                    MeshStreamer &meshStream,
                    Rectf _dstRect,
                    const ColorRGBA &color,
                    Vec2f _center = {},
                    const float rotation = 0)
            : RenderObject(OBJECT_PAINT, id),
              type(PAINT_RECTANGLE),
              paintStream(paintStream),
              slot(paintStream.create()),
              meshStream(meshStream),
              dstRect(std::move(_dstRect)),
              center(std::move(_center)),
              rotation(rotation),
              color(color) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
            meshHandle = meshStream.getNormalizedQuad();
            modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                          * MatrixMath::rotate(Vec3f(0, 0, rotation))
                          * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        /**
         * Paint texture
         *
         * @param id
         * @param paintStream
         * @param meshStream
         * @param _srcRect
         * @param _dstRect
         * @param _texture
         * @param filter
         * @param wrap
         * @param mix
         * @param alpha_mix
         * @param mixColor
         * @param center
         * @param rotation
         */
        RenderPaint(const Id id,
                    BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                    MeshStreamer &meshStream,
                    Rectf _srcRect,
                    Rectf _dstRect,
                    RenderObjectHandle<RenderTexture> _texture,
                    const FilteringMethod filter,
                    const WrappingMethod wrap,
                    const float mix = 0,
                    const float alpha_mix = 0,
                    const ColorRGBA &mixColor = {},
                    const Vec2f &center = {},
                    const float rotation = 0)
            : RenderObject(OBJECT_PAINT, id),
              type(PAINT_TEXTURE),
              paintStream(paintStream),
              slot(paintStream.create()),
              meshStream(meshStream),
              srcRect(std::move(_srcRect)),
              dstRect(std::move(_dstRect)),
              center(center),
              rotation(rotation),
              color(mixColor),
              texture(std::move(_texture)),
              filter(filter),
              wrap(wrap),
              mix(mix),
              alpha_mix(alpha_mix) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paint.colorMixFactor = mix;
            paint.alphaMixFactor = alpha_mix;

            const auto uvOffset = srcRect.position / texture->getHandle().size.convert<float>();
            const auto uvScale = (srcRect.dimensions / texture->getHandle().size.convert<float>());
            paint.uvOffset_uvScale = Vec4f(uvOffset.x, uvOffset.y, uvScale.x, uvScale.y);

            paint.texture.level_index = Vec2i(texture->getHandle().level, texture->getHandle().slot);
            paint.texture.minFilter_magFilter_mipFilter_wrap = Vec4i(FILTER_NEAREST,
                                                                     filter,
                                                                     rg::NEAREST,
                                                                     wrap);

            const auto scale = texture->getHandle().getScale();
            paint.texture.scale_texSize = Vec4f(scale.x,
                                                scale.y,
                                                texture->getHandle().size.x,
                                                texture->getHandle().size.y);

            paintStream.upload(slot, paint);

            meshHandle = meshStream.getNormalizedQuad();
            modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                          * MatrixMath::rotate(Vec3f(0, 0, rotation))
                          * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        ~RenderPaint() override {
            paintStream.destroy(slot);
            if (type == PAINT_LINE) {
                meshStream.destroy(meshHandle);
            }
        }

        [[nodiscard]] Type getPaintType() const {
            return type;
        }

        [[nodiscard]] BufferStreamer<ShaderCanvasPaint::CPU>::Slot getSlot() const {
            return slot;
        }

        [[nodiscard]] const Rectf &getSrcRect() const {
            return srcRect;
        }

        [[nodiscard]] const Rectf &getDstRect() const {
            return dstRect;
        }

        [[nodiscard]] const Vec2f &getCenter() const {
            return center;
        }

        [[nodiscard]] float getRotation() const {
            return rotation;
        }

        [[nodiscard]] const ColorRGBA &getColor() const {
            return color;
        }

        [[nodiscard]] RenderObjectHandle<RenderTexture> getTexture() const {
            return texture;
        }

        [[nodiscard]] bool isFilter() const {
            return filter;
        }

        [[nodiscard]] float getMix() const {
            return mix;
        }

        [[nodiscard]] float getAlphaMix() const {
            return alpha_mix;
        }

        bool isUploadComplete() override {
            if (!meshStream.isUploadComplete(meshHandle)) {
                return false;
            }
            if (texture && !texture->isUploadComplete()) {
                return false;
            }
            return paintStream.isUploadComplete(slot);
        }

        void flush() override {
            meshStream.flush(meshHandle);
            if (texture) {
                texture->flush();
            }
            paintStream.flush(slot);
        }

    private:
        Type type;

        BufferStreamer<ShaderCanvasPaint::CPU> &paintStream;
        BufferStreamer<ShaderCanvasPaint::CPU>::Slot slot;

        MeshStreamer &meshStream;
        MeshStreamer::Handle meshHandle{};

        Mat4f modelMatrix;

        Rectf srcRect{};
        Rectf dstRect{};

        Vec2f center{};
        float rotation{};

        ColorRGBA color{};

        RenderObjectHandle<RenderTexture> texture{};
        FilteringMethod filter{};
        WrappingMethod wrap{};
        float mix{};
        float alpha_mix{};
    };
}

#endif //XENGINE_RENDERPAINT_HPP
