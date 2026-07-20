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

#include "xng/renderer/renderobject.hpp"

#include "xng/renderer/pipeline/renderpipeline.hpp"
#include "xng/renderer/camera.hpp"

namespace xng {
    class RenderCanvas final : public RenderObject {
    public:
        static Mat4f getLocalProjection(const Vec2f &screenSize) {
            return getLocalProjection(0, screenSize.x, 0, screenSize.y);
        }

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
        static Mat4f getLocalProjection(const float left,
                                        const float right,
                                        const float top,
                                        const float bottom) {
            const auto projection = Camera::getOrthographicProjection(left,
                                                                      right,
                                                                      bottom,
                                                                      top,
                                                                      0.001,
                                                                      1);

            return projection;
        }

        RenderCanvas() = default;

        explicit RenderCanvas(std::shared_ptr<RenderPipeline> pipeline)
            : pipeline(std::move(pipeline)) {
        }

        RenderCanvas(std::shared_ptr<RenderPipeline> pipeline,
                     RenderObjectHandle<RenderTexture> texture)
            : pipeline(std::move(pipeline)),
              texture(std::move(texture)) {
        }

        void setLocalProjection(const Mat4f &localProjection) {
            if (pipeline == nullptr) {
                throw std::runtime_error("Uninitialized RenderCanvas");
            }
            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, -1});
            const auto view = Camera::getView(cameraTransform);
            pipeline->setCamera(Vec3f(0, 0, -1), view, localProjection);
        }

        const std::shared_ptr<RenderPipeline> &getPipeline() const {
            if (pipeline == nullptr) {
                throw std::runtime_error("Uninitialized RenderCanvas");
            }
            return pipeline;
        }

    private:
        std::shared_ptr<RenderPipeline> pipeline = nullptr;
        RenderObjectHandle<RenderTexture> texture{};
    };
}

#endif //XENGINE_RENDERCANVAS_HPP
