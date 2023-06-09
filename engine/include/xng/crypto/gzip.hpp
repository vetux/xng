/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_GZIP_HPP
#define XENGINE_GZIP_HPP

#include <string>
#include <vector>

namespace xng {
    /**
     * GZip Compression and Decompression
     */
    class GZip {
    public:
        virtual std::vector<char> compress(const char *data, size_t length) = 0;

        virtual std::vector<char> decompress(const char *data, size_t length) = 0;

        virtual std::vector<char> compress(const std::vector<char> &data) = 0;

        virtual std::vector<char> decompress(const std::vector<char> &data) = 0;

        virtual std::string compress(const std::string &data) = 0;

        virtual std::string decompress(const std::string &data) = 0;
    };
}

#endif //XENGINE_GZIP_HPP
