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

#ifndef XENGINE_RENDERPAINTPOINT_HPP
#define XENGINE_RENDERPAINTPOINT_HPP

#include <utility>

#include "xng/assets/color.hpp"
#include "xng/math/matrixmath.hpp"
#include "xng/renderer/objects/renderpaint.hpp"

namespace xng {
    class RenderPaintPoint final : public RenderPaint {
    public:
        /**
         * Paint Point
         *
         * @param id
         * @param paintStream
         * @param _position
         * @param size
         * @param color
         */
        RenderPaintPoint(const Id id,
                         BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                         Vec2f _position,
                         const float size,
                         const ColorRGBA &color)
            : RenderPaint(OBJECT_PAINT_POINT, id),
              paintStream(paintStream),
              slot(paintStream.create()),
              position(std::move(_position)),
              color(color) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
            modelMatrix = MatrixMath::scale(Vec3f(size, size, 1))
                          * MatrixMath::translate(Vec3f(position.x, position.y, 0));
        }

        ~RenderPaintPoint() override {
            paintStream.destroy(slot);
        }

        [[nodiscard]] BufferStreamer<ShaderCanvasPaint::CPU>::Slot getSlot() const {
            return slot;
        }

        [[nodiscard]] const Vec2f &getPosition() const {
            return position;
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

        Vec2f position;

        ColorRGBA color{};
    };
}
#endif //XENGINE_RENDERPAINTPOINT_HPP
