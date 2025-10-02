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

#ifndef XENGINE_TEXTLAYOUT_HPP
#define XENGINE_TEXTLAYOUT_HPP

#include <utility>

#include "textlayoutparameters.hpp"
#include "xng/font/character.hpp"
#include "xng/resource/uri.hpp"

namespace xng {
    class TextLayoutEngine;

    struct TextLayout {
        struct Glyph {
            Vec2f position; // The absolute position of the character image
            std::shared_ptr<const Character> character;

            Glyph(Vec2f position, std::shared_ptr<const Character> character)
                : position(std::move(position)), character(std::move(character)) {
            }
        };

        TextLayout(const Vec2i &size,
                   const std::vector<Glyph> &glyphs,
                   const Uri &font_uri,
                   const Vec2i &font_pixel_size,
                   const std::string &text,
                   const TextLayoutParameters &parameters)
            : size(size),
              glyphs(glyphs),
              fontUri(font_uri),
              fontPixelSize(font_pixel_size),
              text(text),
              parameters(parameters) {
        }

        Vec2i size; // The total size of the text layout
        std::vector<Glyph> glyphs;

        Uri fontUri; // The font used to render the glyphs
        Vec2i fontPixelSize; // The size of the glyphs in pixels
        std::string text;
        TextLayoutParameters parameters; // The parameters used to render the glyphs
    };
}

#endif //XENGINE_TEXTLAYOUT_HPP
