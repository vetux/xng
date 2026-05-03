/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_CHARACTER_HPP
#define XENGINE_CHARACTER_HPP

#include <memory>
#include <utility>

#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT Character {
    public:
        char value{};

        Vec2i bearing{}; //The bearing of the character in pixels
        int advance{}; //The horizontal advance of the character in pixels

        Vec2i bitmapSize{}; // The width / height of the bitmap
        std::vector<uint8_t> bitmap{}; // The character bitmap, 1 byte grayscale per pixel, packed row ascending

        Character() = default;

        Character(const char value,
                  Vec2i bearing,
                  const int advance,
                  Vec2i bitmapSize,
                  std::vector<uint8_t> bitmap)
            : value(value),
              bearing(std::move(bearing)),
              advance(advance),
              bitmapSize(std::move(bitmapSize)),
              bitmap(std::move(bitmap)) {
        }

        ~Character() = default;

        Character(const Character &other) = default;

        Character &operator=(const Character &other) = default;

        Character(Character &&other) = default;

        Character &operator=(Character &&other) = default;
    };
}

#endif //XENGINE_CHARACTER_HPP
