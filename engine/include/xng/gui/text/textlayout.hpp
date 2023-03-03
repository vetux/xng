/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_TEXTPROPERTIES_HPP
#define XENGINE_TEXTPROPERTIES_HPP

#include "textalignment.hpp"

namespace xng  {
    struct TextLayout {
        /**
         * The distance between the top of the texture and the origin of the first line or the distance between the origins of each consequtive line.
         */
        int lineHeight = 50;

        /**
         * If larger than zero the value in pixels at which a newline is rendered if the widths of characters in the current line exceed it.
         * The character is then drawn on a new line
         */
        int lineWidth = 0;

        /**
         * Additional spacing between lines
         */
        int lineSpacing = 10;

        /**
         * The horizontal alignment of lines within a rendered text eg.
         *
         * ALIGN_LEFT:
         * TestTestTest123
         * TestTest123
         * Test123
         *
         * ALIGN_CENTER:
         * TestTestTest123
         *   TestTest123
         *     Test123
         *
         * ALIGN_RIGHT:
         * TestTestTest123
         *     TestTest123
         *         Test123
         */
        TextAlignment alignment = TEXT_ALIGN_LEFT;
    };
}
#endif //XENGINE_TEXTPROPERTIES_HPP
