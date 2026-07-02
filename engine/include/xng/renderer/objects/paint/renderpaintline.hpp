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

#ifndef XENGINE_RENDERPAINTLINE_HPP
#define XENGINE_RENDERPAINTLINE_HPP

#include <utility>

#include "xng/assets/color.hpp"
#include "xng/math/matrixmath.hpp"
#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/stream/meshstreamer.hpp"

namespace xng {
    class RenderPaintLine final : public RenderPaint {
    public:
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
        RenderPaintLine(const Id id,
                        BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                        MeshStreamer &meshStream,
                        Vec2f _start,
                        Vec2f _end,
                        const ColorRGBA &color,
                        Vec2f center = {},
                        const float rotation = 0)
            : RenderPaint(OBJECT_PAINT_LINE, id),
              paintStream(paintStream),
              slot(paintStream.create()),
              meshStream(meshStream),
              start(std::move(_start)),
              end(std::move(_end)),
              center(std::move(center)),
              rotation(rotation),
              color(color) {
            Mesh mesh;
            mesh.primitive = Mesh::TRIANGLES;
            mesh.positions.emplace_back(start.x, start.y, 1);
            mesh.positions.emplace_back(end.x, end.y, 1);
            mesh.positions.emplace_back(end.x, end.y, 1);
            meshHandle = meshStream.create(mesh);
            modelMatrix = MatrixMath::identity();

            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paintStream.upload(slot, paint);
        }

        ~RenderPaintLine() override {
            paintStream.destroy(slot);
            meshStream.destroy(meshHandle);
        }

        [[nodiscard]] Type getPaintType() const {
            return type;
        }

        [[nodiscard]] BufferStreamer<ShaderCanvasPaint::CPU>::Slot getSlot() const {
            return slot;
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
            if (!meshStream.isUploadComplete(meshHandle)) {
                return false;
            }
            return paintStream.isUploadComplete(slot);
        }

        void flush() override {
            meshStream.flush(meshHandle);
            paintStream.flush(slot);
        }

    private:
        BufferStreamer<ShaderCanvasPaint::CPU> &paintStream;
        BufferStreamer<ShaderCanvasPaint::CPU>::Slot slot;

        MeshStreamer &meshStream;
        MeshStreamer::Handle meshHandle{};

        Mat4f modelMatrix;

        Vec2f start;
        Vec2f end;

        Vec2f center{};
        float rotation{};

        ColorRGBA color{};
    };
}
#endif //XENGINE_RENDERPAINTLINE_HPP
