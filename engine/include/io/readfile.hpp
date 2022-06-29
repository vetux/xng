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

#ifndef XENGINE_READFILE_HPP
#define XENGINE_READFILE_HPP

#include <cstdio>

namespace xng {
    /**
     * Fast reading of a whole file into memory using cstdio
     *
     * @param path
     * @return
     */
    inline std::vector<char> readFile(const std::string &path) {
        FILE *fp;
        fp = fopen(path.c_str(), "r");

        const size_t step = 100;
        std::vector<char> fbuf(step);

        auto r = fread(fbuf.data(), 1, fbuf.size(), fp);

        std::vector<char> ret;
        ret.insert(ret.end(), fbuf.begin(), fbuf.begin() + static_cast<long>(r));
        while (r == step) {
            r = fread(fbuf.data(), 1, fbuf.size(), fp);
            ret.insert(ret.end(), fbuf.begin(), fbuf.begin() + r);
        }

        fclose(fp);

        return ret;
    }

}

#endif //XENGINE_READFILE_HPP
