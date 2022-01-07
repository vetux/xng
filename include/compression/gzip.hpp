/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_GZIP_HPP
#define MANA_GZIP_HPP

#include <string>
#include <vector>

namespace engine {
    namespace GZip {
        MANA_EXPORT std::vector<char> compress(const char *data, size_t length);

        MANA_EXPORT std::vector<char> decompress(const char *data, size_t length);

        MANA_EXPORT std::vector<char> compress(const std::vector<char> &data);

        MANA_EXPORT std::vector<char> decompress(const std::vector<char> &data);

        MANA_EXPORT std::string compress(const std::string &data);

        MANA_EXPORT std::string decompress(const std::string &data);
    }
}

#endif //MANA_GZIP_HPP
