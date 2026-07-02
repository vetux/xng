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

#ifndef XENGINE_TEXTLAYOUT_HPP
#define XENGINE_TEXTLAYOUT_HPP

#include <utility>

#include "xng/math/vector2.hpp"

namespace xng {
    struct TextLayout {
        struct Character {
            Vec2f position; // The absolute position of the character bitmap
            char32_t character; // The character code point

            Character(Vec2f position, const char32_t character)
                : position(std::move(position)), character(character) {
            }
        };

        TextLayout() = default;

        TextLayout(Vec2i size, const std::vector<Character> &characters)
            : size(std::move(size)),
              characters(characters) {
        }

        Vec2i size; // The total size of the text layout
        std::vector<Character> characters;
    };
}

#endif //XENGINE_TEXTLAYOUT_HPP
