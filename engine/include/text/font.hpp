/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_FONT_HPP
#define XENGINE_FONT_HPP

#include <istream>
#include <memory>

#include "math/vector2.hpp"

#include "text/character.hpp"

namespace xng {
    class XENGINE_EXPORT Font {
    public:
        /**
         * Load the font from the stream.
         *
         * @param stream The stream pointing to the ttf font data.
         * @return
         */
        static std::unique_ptr<Font> createFont(std::istream &stream);

        virtual ~Font() = default;

        /**
         * Set the requested pixel size.
         *
         * @param size
         */
        virtual void setPixelSize(Vec2i size) = 0;

        /**
         *  Rasterize the given ascii character.
         *
         * @param c
         * @return
         */
        virtual Character renderAscii(char c) = 0;

        /**
         * Rasterize all ascii characters and return the character mapping.
         *
         * @return
         */
        virtual std::map<char, Character> renderAscii() = 0;

        //TODO: Unicode character rasterization
    };
}

#endif //XENGINE_FONT_HPP
