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

#ifndef XENGINE_READFILE_HPP
#define XENGINE_READFILE_HPP

#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

namespace xng {
    /**
     * @param path
     * @return
     */
    inline std::vector<char> readFile(const std::filesystem::path &path) {
        std::ifstream ifs(path, std::ios_base::in | std::ios::binary);
        return {(std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>()};
    }

    /**
     * @param path
     * @return
     */
    inline std::string readFileString(const std::filesystem::path &path) {
        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }

}

#endif //XENGINE_READFILE_HPP
