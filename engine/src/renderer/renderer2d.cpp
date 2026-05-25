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

#include "xng/renderer/renderer2d.hpp"

#include <utility>
/*
namespace xng {
    Renderer2D::Renderer2D()
        : canvas({1, 1}){
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::renderBegin(const ColorRGBA &clearColor) {
        if (isRendering) {
            throw std::runtime_error("Already Rendering (Nested Renderer2D::renderBegin calls?)");
        }

        isRendering = true;
        canvas = Canvas({});
    }

    void Renderer2D::renderPresent() {
        if (!isRendering) {
            throw std::runtime_error("Not Rendering");
        }
        isRendering = false;
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          const ResourceHandle<ImageRGBA> &texture,
                          const Vec2f &center,
                          float rotation,
                          bool filter,
                          float mixRGB,
                          float mixAlpha,
                          const ColorRGBA &mixColor) {
        canvas.paint(PaintImage(srcRect,
                                dstRect,
                                texture,
                                filter,
                                mixRGB,
                                mixAlpha,
                                mixColor,
                                center,
                                rotation));
    }

    void Renderer2D::draw(const Vec2f &position,
                          const TextLayout &text,
                          const ColorRGBA &color) {
        canvas.paint(PaintText(position, text, color));
    }

    void Renderer2D::draw(const Rectf &rectangle,
                          const ColorRGBA &color,
                          bool fill,
                          const Vec2f &center,
                          float rotation) {
        canvas.paint(PaintRectangle(rectangle, color, fill, center, rotation));
    }

    void Renderer2D::draw(const Vec2f &start,
                          const Vec2f &end,
                          const ColorRGBA &color) {
        canvas.paint(PaintLine(start, end, color));
    }

    void Renderer2D::draw(const Vec2f &point, const ColorRGBA &color) {
        canvas.paint(PaintPoint(point, color));
    }
}*/
