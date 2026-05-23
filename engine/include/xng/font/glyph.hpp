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

#ifndef XENGINE_GLYPH_HPP
#define XENGINE_GLYPH_HPP

#include <memory>
#include <utility>

#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT Glyph {
    public:
        struct Metrics {
            Vec2i bearing{}; //The bearing of the character in pixels
            int advance{}; //The horizontal advance of the character in pixels
            Vec2i bitmapSize{}; // The width / height of the bitmap

            Metrics() = default;

            Metrics(Vec2i bearing, const int advance, Vec2i bitmapSize)
                : bearing(std::move(bearing)), advance(advance), bitmapSize(std::move(bitmapSize)) {
            }
        };

        char32_t character{};
        Metrics metrics{};
        std::vector<uint8_t> bitmap{}; // The character bitmap, 1 byte grayscale per pixel, packed row ascending

        Glyph() = default;

        Glyph(const char32_t character,
              Vec2i bearing,
              const int advance,
              Vec2i bitmapSize,
              std::vector<uint8_t> bitmap)
            : character(character),
              metrics(std::move(bearing), advance, std::move(bitmapSize)),
              bitmap(std::move(bitmap)) {
        }

        ~Glyph() = default;

        Glyph(const Glyph &other) = default;

        Glyph &operator=(const Glyph &other) = default;

        Glyph(Glyph &&other) = default;

        Glyph &operator=(Glyph &&other) = default;
    };
}

#endif //XENGINE_GLYPH_HPP
