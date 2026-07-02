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

#ifndef XENGINE_RENDERPAINTRECT_HPP
#define XENGINE_RENDERPAINTRECT_HPP

#include <utility>

#include "xng/assets/color.hpp"
#include "xng/math/matrixmath.hpp"
#include "xng/renderer/objects/renderpaint.hpp"

namespace xng {
    class RenderPaintRect final : public RenderPaint {
    public:
        /**
         * Paint rectangle (For non-filled rectangles use lines)
         *
         * @param id
         * @param paintStream
         * @param _dstRect
         * @param color
         * @param _center
         * @param rotation
         */
        RenderPaintRect(const Id id,
                    BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                    Rectf _dstRect,
                    const ColorRGBA &color,
                    Vec2f _center = {},
                    const float rotation = 0)
            : RenderPaint(OBJECT_PAINT_RECTANGLE, id),
              paintStream(paintStream),
              slot(paintStream.create()),
              dstRect(std::move(_dstRect)),
              center(std::move(_center)),
              rotation(rotation),
              color(color) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
            modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                          * MatrixMath::rotate(Vec3f(0, 0, rotation))
                          * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        ~RenderPaintRect() override {
            paintStream.destroy(slot);
        }

        [[nodiscard]] BufferStreamer<ShaderCanvasPaint::CPU>::Slot getSlot() const {
            return slot;
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

        bool isUploadComplete() override {
            return paintStream.isUploadComplete(slot);
        }

        void flush() override {
            paintStream.flush(slot);
        }

    private:
        BufferStreamer<ShaderCanvasPaint::CPU> &paintStream;
        BufferStreamer<ShaderCanvasPaint::CPU>::Slot slot;

        Mat4f modelMatrix;

        Rectf dstRect{};

        Vec2f center{};
        float rotation{};

        ColorRGBA color{};
    };
}
#endif //XENGINE_RENDERPAINTRECT_HPP