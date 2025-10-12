/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_CANVAS_HPP
#define XENGINE_CANVAS_HPP

#include <utility>

#include "xng/graphics/paint.hpp"
#include "xng/graphics/camera.hpp"
#include "xng/math/transform.hpp"

namespace xng {
    class Canvas {
    public:
        explicit Canvas(Vec2i canvasSize)
            : canvasSize(std::move(canvasSize)) {
            Camera camera;
            camera.type = ORTHOGRAPHIC;
            camera.left = 0;
            camera.top = 0;
            camera.right = static_cast<float>(canvasSize.x);
            camera.bottom = static_cast<float>(canvasSize.y);

            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, 1});

            viewProjectionMatrix = camera.projection() * Camera::view(cameraTransform);
        }

        void setProjection(float left, float right, float bottom, float top) {
            Camera camera;
            camera.type = ORTHOGRAPHIC;
            camera.left = left;
            camera.right = right;
            camera.bottom = bottom;
            camera.top = top;

            Transform cameraTransform;
            cameraTransform.setPosition({0, 0, 1});

            viewProjectionMatrix = camera.projection() * Camera::view(cameraTransform);
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

        std::vector<Paint> paintCommands;
    };
}

#endif //XENGINE_CANVAS_HPP
