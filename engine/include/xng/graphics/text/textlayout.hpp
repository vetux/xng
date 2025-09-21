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

#ifndef XENGINE_TEXTLAYOUT_HPP
#define XENGINE_TEXTLAYOUT_HPP

#include <utility>

#include "xng/font/character.hpp"
#include "xng/resource/uri.hpp"

namespace xng {
    class TextLayoutEngine;

    struct TextLayout {
        struct Glyph {
            Vec2f position; // The absolute position of the character image
            std::reference_wrapper<const Character> character;

            Glyph(Vec2f position, const Character &character)
                : position(std::move(position)), character(character) {
            }
        };

        TextLayout(const Vec2i &size,
                   const std::vector<Glyph> &glyphs,
                   const Uri &font_uri,
                   const Vec2i &font_pixel_size)
            : size(size),
              glyphs(glyphs),
              fontUri(font_uri),
              fontPixelSize(font_pixel_size) {
        }

        Vec2i size; // The total size of the text layout
        std::vector<Glyph> glyphs;

        Uri fontUri; // The font used to render the glyphs
        Vec2i fontPixelSize; // The size of the glyphs in pixels
    };
}

#endif //XENGINE_TEXTLAYOUT_HPP
