/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_WRITEFILE_HPP
#define XENGINE_WRITEFILE_HPP

#include <fstream>
#include <vector>

namespace xng {
    template<typename T>
    inline void writeFile(const std::filesystem::path &path, const std::vector<T> &data) {
        std::ofstream ofs(path, std::ios_base::out | std::ios::binary);
        ofs.write(reinterpret_cast<const char *>(data.data()), sizeof(T) * data.size());
    }

    inline void writeFile(const std::filesystem::path &path, const std::string &data) {
        std::ofstream ofs(path);
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    }
}

#endif //XENGINE_WRITEFILE_HPP
