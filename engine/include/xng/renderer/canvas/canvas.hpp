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

#ifndef XENGINE_CANVAS_HPP
#define XENGINE_CANVAS_HPP

#include <utility>

#include "paint.hpp"
#include "xng/renderer/camera.hpp"
#include "xng/math/transform.hpp"

namespace xng {
    class Canvas {
    public:
        explicit Canvas(Vec2i _size)
            : canvasSize(std::move(_size)) {
            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, -1});

            const auto view = Camera::getView(cameraTransform);
            const auto projection = Camera::getOrthographicProjection(0,
                                                                      static_cast<float>(canvasSize.x),
                                                                      0,
                                                                      static_cast<float>(canvasSize.y),
                                                                      0.001,
                                                                      2);

            viewProjectionMatrix = projection * view;
        }

        void setProjection(const float left, const float right, const float bottom, const float top) {
            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, -1});

            const auto view = Camera::getView(cameraTransform);
            const auto projection = Camera::getOrthographicProjection(left,
                                                                      right,
                                                                      bottom,
                                                                      top,
                                                                      0.001,
                                                                      1);

            viewProjectionMatrix = projection * view;
        }

        void setWorldTransform(const Transform &canvasTransform) { worldTransform = canvasTransform; }

        void setWorldSpace(const bool isWorldSpace) { this->worldSpace = isWorldSpace; }

        void setBackgroundColor(const ColorRGBA &color) { this->backgroundColor = color; }

        void paint(const Paint &command) { paintCommands.emplace_back(command); }

        const Vec2i &getCanvasSize() const { return canvasSize; }

        const Mat4f &getViewProjectionMatrix() const { return viewProjectionMatrix; }

        const Transform &getWorldTransform() const { return worldTransform; }

        bool isWorldSpace() const { return worldSpace; }

        const ColorRGBA &getBackgroundColor() const { return backgroundColor; }

        const std::vector<Paint> &getPaintCommands() const { return paintCommands; }

    private:
        Vec2i canvasSize;
        Mat4f viewProjectionMatrix;

        Transform worldTransform;
        bool worldSpace = false;

        ColorRGBA backgroundColor = ColorRGBA::black();

        std::vector<Paint> paintCommands{};
    };
}

#endif //XENGINE_CANVAS_HPP
