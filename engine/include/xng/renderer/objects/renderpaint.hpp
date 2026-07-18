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


namespace xng {
    class RenderPaint {
    public:
        static Mat4f getTransform(const Rectf &dstRect, const float rotation) {
            return MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                   * MatrixMath::rotate(Vec3f(0, 0, rotation))
                   * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        /**
         * Paint Line
         *
         * @param allocator
         * @param start
         * @param end
         * @param color
         * @param center
         * @param rotation
         */
        RenderPaint(RenderAllocator &allocator,
              const Vec2f &start,
              const Vec2f &end,
              const ColorRGBA &color,
              const Vec2f &center = {},
              const float rotation = 0) {
            transform = allocator.createTransform(MatrixMath::rotate(Vec3f(0, 0, rotation))
                                                  * MatrixMath::translate(Vec3f(center.x, center.y, 0)));
            material = allocator.createMaterial(color,
                                                0,
                                                0,
                                                0,
                                                0,
                                                false,
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {});
            Mesh m;
            m.primitive = Mesh::TRIANGLES;
            m.positions.emplace_back(start.x, start.y, 1);
            m.positions.emplace_back(end.x, end.y, 1);
            m.positions.emplace_back(end.x, end.y, 1);
            mesh = allocator.createMesh(m, {});
        }

        /**
         * Paint Point
         *
         * @param allocator
         * @param unitQuadMesh
         * @param position
         * @param size
         * @param color
         */
        RenderPaint(RenderAllocator &allocator,
              RenderObjectHandle<RenderMesh> unitQuadMesh,
              const Vec2f &position,
              const float size,
              const ColorRGBA &color) {
            transform = allocator.createTransform(MatrixMath::scale(Vec3f(size, size, 1))
                                                  * MatrixMath::translate(Vec3f(position.x, position.y, 0)));
            material = allocator.createMaterial(color,
                                                0,
                                                0,
                                                0,
                                                0,
                                                false,
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {});
            mesh = std::move(unitQuadMesh);
        }

        /**
         * Paint Rect
         *
         * @param allocator
         * @param unitQuadMesh
         * @param dstRect
         * @param color
         * @param center
         * @param rotation
         */
        RenderPaint(RenderAllocator &allocator,
              RenderObjectHandle<RenderMesh> unitQuadMesh,
              const Rectf &dstRect,
              const ColorRGBA &color,
              const Vec2f &center = {},
              const float rotation = 0) {
            transform = allocator.createTransform(
                MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                * MatrixMath::rotate(Vec3f(0, 0, rotation))
                * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1)));
            material = allocator.createMaterial(color,
                                                0,
                                                0,
                                                0,
                                                0,
                                                false,
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {});
            mesh = std::move(unitQuadMesh);
        }

        /**
         * Paint Texture
         *
         * @param allocator
         * @param unitQuadMesh
         * @param dstRect
         * @param texture
         * @param samplingProperties
         * @param center
         * @param rotation
         */
        RenderPaint(RenderAllocator &allocator,
              RenderObjectHandle<RenderMesh> unitQuadMesh,
              const Rectf &dstRect,
              const RenderObjectHandle<RenderTexture> &texture,
              const SamplingProperties &samplingProperties,
              const Vec2f &center = {},
              const float rotation = 0) {
            transform = allocator.createTransform(
                MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                * MatrixMath::rotate(Vec3f(0, 0, rotation))
                * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1)));
            material = allocator.createMaterial({},
                                                0,
                                                0,
                                                0,
                                                0,
                                                false,
                                                texture,
                                                samplingProperties,
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {},
                                                {});
            mesh = std::move(unitQuadMesh);
        }

        [[nodiscard]] RenderObjectHandle<RenderTransform> getTransform() const {
            return transform;
        }

        [[nodiscard]] RenderObjectHandle<RenderMaterial> getMaterial() const {
            return material;
        }

        [[nodiscard]] RenderObjectHandle<RenderMesh> getMesh() const {
            return mesh;
        }

    private:
        RenderObjectHandle<RenderTransform> transform;
        RenderObjectHandle<RenderMaterial> material;
        RenderObjectHandle<RenderMesh> mesh;
    };
}

#endif //XENGINE_RENDERPAINT_HPP