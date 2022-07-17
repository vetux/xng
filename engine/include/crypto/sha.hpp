/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_SHA_HPP
#define XENGINE_SHA_HPP

#include <string>
#include <vector>

namespace xng {
    class SHA {
    public:
        virtual ~SHA() = default;

        virtual std::string sha256(const char *data, size_t length) = 0;

        virtual std::string sha256(const std::string &data) = 0;

        virtual std::string sha256(const std::vector<char> &data) = 0;
    };
}

#endif //XENGINE_SHA_HPP
