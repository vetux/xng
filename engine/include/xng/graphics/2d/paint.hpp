/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_PAINT_HPP
#define XENGINE_PAINT_HPP

#include <variant>

#include "xng/resource/resourcehandle.hpp"
#include "xng/math/rectangle.hpp"
#include "xng/graphics/color.hpp"
#include "xng/graphics/image.hpp"
#include "xng/graphics/text/textlayout.hpp"

namespace xng {
    struct PaintPoint {
        Vec2f position;
        ColorRGBA color;

        PaintPoint(const Vec2f &position, const ColorRGBA &color)
            : position(position),
              color(color) {
        }
    };

    struct PaintLine {
        Vec2f start;
        Vec2f end;
        ColorRGBA color;
        Vec2f center;
        float rotation;

        PaintLine(const Vec2f &start,
                  const Vec2f &end,
                  const ColorRGBA &color,
                  const Vec2f &center = {},
                  float rotation = 0)
            : start(start),
              end(end),
              color(color),
              center(center),
              rotation(rotation) {
        }
    };

    struct PaintRectangle {
        Rectf dstRect;
        ColorRGBA color;
        bool fill;
        Vec2f center;
        float rotation;

        PaintRectangle(const Rectf &dst_rect,
                    const ColorRGBA &color,
                    bool fill,
                    const Vec2f &center = {},
                    float rotation = 0)
            : dstRect(dst_rect),
              color(color),
              fill(fill),
              center(center),
              rotation(rotation) {
        }
    };

    struct PaintImage {
        Rectf srcRect;
        Rectf dstRect;
        ResourceHandle<ImageRGBA> image;
        bool filter;
        float mix;
        float alphaMix;
        ColorRGBA mixColor;
        Vec2f center;
        float rotation;

        PaintImage(const Rectf &src_rect,
                   const Rectf &dst_rect,
                   const ResourceHandle<ImageRGBA> &image,
                   bool filter,
                   float mix = 0,
                   float alpha_mix = 0,
                   const ColorRGBA &mix_color = {},
                   const Vec2f &center = {},
                   float rotation = 0)
            : srcRect(src_rect),
              dstRect(dst_rect),
              image(image),
              filter(filter),
              mix(mix),
              alphaMix(alpha_mix),
              mixColor(mix_color),
              center(center),
              rotation(rotation) {
        }
    };

    struct PaintText {
        Vec2f position;
        TextLayout text;
        ColorRGBA color;

        PaintText(const Vec2f &position, const TextLayout &text, const ColorRGBA &color)
            : position(position),
              text(text),
              color(color) {
        }
    };

    struct Paint {
        enum Type {
            PAINT_POINT = 0,
            PAINT_LINE,
            PAINT_RECTANGLE,
            PAINT_IMAGE,
            PAINT_TEXT,
        } type;

        std::variant<PaintPoint, PaintLine, PaintRectangle, PaintImage, PaintText> data;

        Paint(PaintPoint point)
            : type(PAINT_POINT), data(point) {
        }

        Paint(PaintLine line)
            : type(PAINT_LINE), data(line) {
        }

        Paint(PaintRectangle square)
            : type(PAINT_RECTANGLE), data(square) {
        }

        Paint(PaintImage image)
            : type(PAINT_IMAGE), data(image) {
        }

        Paint(PaintText text)
            : type(PAINT_TEXT), data(text) {
        }
    };
}

#endif //XENGINE_PAINT_HPP
