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

#ifndef XENGINE_READFILE_HPP
#define XENGINE_READFILE_HPP

#include <cstdio>
#include <fstream>

namespace xng {
    /**
     * Fast reading of a whole file into memory using cstdio
     *
     * @param path
     * @return
     */
    inline std::vector<char> readFile(const std::string &path, const size_t bufferStepSize = 100) {
        std::ifstream ifs(path, std::ios_base::in | std::ios::binary);
        return std::vector<char>((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    }

    /**
     * Fast reading of a whole file into memory using cstdio
     *
     * @param path
     * @return
     */
    inline std::string readFileString(const std::string &path, const size_t bufferStepSize = 100) {
        FILE *fp;
        fp = fopen(path.c_str(), "r");

        std::vector<char> fbuf(bufferStepSize);

        auto r = fread(fbuf.data(), 1, fbuf.size(), fp);

        std::string ret;
        ret.insert(ret.end(),
                   fbuf.begin(),
                   fbuf.begin() + static_cast<std::iterator<char *, std::vector<char>>::difference_type>(r));
        while (r == bufferStepSize) {
            r = fread(fbuf.data(), 1, fbuf.size(), fp);
            ret.insert(ret.end(),
                       fbuf.begin(),
                       fbuf.begin() + static_cast<std::iterator<char *, std::vector<char>>::difference_type>(r));
        }

        fclose(fp);

        return ret;
    }

}

#endif //XENGINE_READFILE_HPP
