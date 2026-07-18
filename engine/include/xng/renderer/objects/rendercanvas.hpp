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


namespace xng {
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

        Canvas(const Mat4f &localProjection,
               RenderObjectHandle<RenderTexture> texture,
               std::vector<Paint> paints)
            : localProjection(localProjection),
              texture(std::move(texture)),
              paints(std::move(paints)) {
        }

    private:
        Mat4f localProjection;
        RenderObjectHandle<RenderTexture> texture;
        std::vector<Paint> paints;
    };
}

#endif //XENGINE_RENDERCANVAS_HPP