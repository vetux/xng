/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include <map>
#include <memory>

#include "xng/gpu/texturebuffer.hpp"

#include "xng/asset/image.hpp"

namespace xng {
    class XENGINE_EXPORT Character {
    public:
        char value{};
        ImageRGBA image; //The rasterized character as an image
        Vec2i bearing;   //The bearing of the character in pixels
        int advance{};   //The advance of the character in pixels

        Character() = default;

        Character(char value, ImageRGBA image, Vec2i bearing, int advance)
                : value(value), image(std::move(image)), bearing(bearing), advance(advance) {}

        ~Character() = default;

        Character(const Character &other) = default;

        Character &operator=(const Character &other) = default;

        Character(Character &&other) = default;

        Character &operator=(Character &&other) = default;
    };
}

#endif //XENGINE_CHARACTER_HPP
