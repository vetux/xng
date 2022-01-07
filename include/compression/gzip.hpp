/**
 *  XEngine - C++ game engine library
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

#ifndef XENGINE_GZIP_HPP
#define XENGINE_GZIP_HPP

#include <string>
#include <vector>

namespace xengine {
    namespace GZip {
        XENGINE_EXPORT std::vector<char> compress(const char *data, size_t length);

        XENGINE_EXPORT std::vector<char> decompress(const char *data, size_t length);

        XENGINE_EXPORT std::vector<char> compress(const std::vector<char> &data);

        XENGINE_EXPORT std::vector<char> decompress(const std::vector<char> &data);

        XENGINE_EXPORT std::string compress(const std::string &data);

        XENGINE_EXPORT std::string decompress(const std::string &data);
    }
}

#endif //XENGINE_GZIP_HPP
