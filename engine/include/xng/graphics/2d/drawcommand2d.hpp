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

#ifndef XENGINE_DRAWCOMMAND2D_HPP
#define XENGINE_DRAWCOMMAND2D_HPP

#include "xng/math/rectangle.hpp"

#include "xng/graphics/color.hpp"
#include "xng/graphics/2d/texture2d.hpp"

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

namespace xng {
    struct DrawCommand2D {
        enum Type {
            COLOR_POINT,
            COLOR_LINE,
            COLOR_PLANE,
            TEXTURE,
        } type{};

        Rectf srcRect{};
        Rectf dstRect{}; // If line dstRect.position contains the start and dstRect.dimensions the end of the line.

        Vec2f center{};
        float rotation = 0;

        bool fill = false;

        Texture2D::Handle texture{};

        TextureFiltering filter{};

        float mix = 0;
        float alphaMix = 0;
        ColorRGBA color{};

        bool colorFactor = false;

        DrawCommand2D() = default;

        DrawCommand2D(Vec2f point,
             const ColorRGBA color,
             Vec2f position,
             Vec2f center,
             const float rotation)
            : type(COLOR_POINT),
              srcRect(std::move(position), {}),
              dstRect(std::move(point), {}),
              color(color),
              center(std::move(center)),
              rotation(rotation) {
        }

        DrawCommand2D(Vec2f start,
             Vec2f end,
             const ColorRGBA color,
             Vec2f position,
             Vec2f center,
             const float rotation)
            : type(COLOR_LINE),
              srcRect(std::move(position), {}),
              dstRect(std::move(start), std::move(end)),
              color(color),
              center(std::move(center)),
              rotation(rotation) {
        }

        DrawCommand2D(Rectf dstRect,
             const ColorRGBA color,
             const bool fill,
             Vec2f center,
             const float rotation)
            : type(COLOR_PLANE),
              dstRect(std::move(dstRect)),
              center(std::move(center)),
              rotation(rotation),
              color(color),
              fill(fill) {
        }

        DrawCommand2D(Rectf srcRect,
             Rectf dstRect,
             const Texture2D::Handle texture,
             Vec2f center,
             const float rotation,
             const TextureFiltering filter,
             const float mix,
             const float alphaMix,
             ColorRGBA color)
            : type(TEXTURE),
              srcRect(std::move(srcRect)),
              dstRect(std::move(dstRect)),
              center(std::move(center)),
              rotation(rotation),
              filter(filter),
              texture(texture),
              mix(mix),
              alphaMix(alphaMix),
              color(color) {
        }

        DrawCommand2D(Rectf srcRect,
             Rectf dstRect,
             const Texture2D::Handle texture,
             Vec2f center,
             const float rotation,
             const TextureFiltering filter,
             const ColorRGBA color)
            : type(TEXTURE),
              srcRect(std::move(srcRect)),
              dstRect(std::move(dstRect)),
              center(std::move(center)),
              rotation(rotation),
              filter(filter),
              texture(texture),
              color(color),
              colorFactor(true) {
        }
    };
}

#endif //XENGINE_DRAWCOMMAND2D_HPP
