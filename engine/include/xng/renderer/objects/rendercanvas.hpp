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

#ifndef XENGINE_RENDERCANVAS_HPP
#define XENGINE_RENDERCANVAS_HPP

#include <utility>

#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/camera.hpp"

namespace xng {
    /**
     * Each canvas can be drawn in 2 ways:
     *
     * - In screen space:
     *   Renders the paint directly to the screen.
     *
     * - To texture:
     *   Renders the paint to a texture. (Canvas must fit in one of the defined TextureResolutions)
     */
    class RenderCanvas final : public RenderObject {
    public:
        /**
         * Construct a default local transformation matrix for translating the canvas contents in canvas space.
         *
         * E.g., for a pixel-perfect screen space canvas use left=0, right=screenSize.x, top=0, bottom=screenSize.y
         *
         * @param left
         * @param right
         * @param top
         * @param bottom
         */
        static Mat4f getLocalProjection(const float left, const float right, const float top, const float bottom) {
            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, -1});

            const auto view = Camera::getView(cameraTransform);
            const auto projection = Camera::getOrthographicProjection(left,
                                                                      right,
                                                                      bottom,
                                                                      top,
                                                                      0.001,
                                                                      1);

            return projection * view;
        }

        /**
         * Construct a screen space canvas.
         *
         * @param id
         * @param _localProjection The canvas space projection to apply to the paints
         */
        explicit RenderCanvas(const Id id, Mat4f _localProjection)
            : RenderObject(OBJECT_CANVAS, id),
              localProjection(std::move(_localProjection)) {
        }

        /**
         * Construct a texture canvas.
         *
         * The paints are rendered to the texture using the local projection.
         *
         * @param id
         * @param _localProjection The canvas space projection to apply to the paints
         * @param texture The texture to render the canvas to
         */
        RenderCanvas(const Id id, Mat4f _localProjection, RenderObjectHandle<RenderTexture> texture)
            : RenderObject(OBJECT_CANVAS, id),
              localProjection(std::move(_localProjection)),
              texture(std::move(texture)) {
        }

        ~RenderCanvas() override = default;

        void setPaints(std::vector<RenderObjectHandle<RenderPaint> > _paints) {
            paints = std::move(_paints);
        }

        bool isUploadComplete() override {
            for (auto &paint: paints) {
                if (!paint->isUploadComplete()) {
                    return false;
                }
            }
            return true;
        }

        void flush() override {
            for (auto &paint: paints) {
                paint->flush();
            }
        }

        const Mat4f &getLocalProjection() const {
            return localProjection;
        }

        const std::vector<RenderObjectHandle<RenderPaint> > &getPaints() const {
            return paints;
        }

        const RenderObjectHandle<RenderTexture> &getTexture() const {
            return texture;
        }

    private:
        Mat4f localProjection{};
        std::vector<RenderObjectHandle<RenderPaint> > paints{};
        RenderObjectHandle<RenderTexture> texture{};
    };
}

#endif //XENGINE_RENDERCANVAS_HPP
