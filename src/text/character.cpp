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

#include "text/character.hpp"

namespace xengine {
    Recti Character::getMetrics(const std::string &str, const std::map<char, Character> &chars) {
        Vec2i origin(0); //The origin of the text
        Vec2i size(0); //The size of the text
        for (auto c : str) {
            //Add advance (The only factor for size x increment)
            size.x += chars.at(c).advance;

            auto min = origin.y - chars.at(c).bearing.y;
            if (min < 0) {
                origin.y += min * -1;
            }

            int height = origin.y + chars.at(c).image.getHeight() - chars.at(c).bearing.y;
            if (size.y < height) {
                size.y = height;
            }
        }
        return {origin, size};
    }
}