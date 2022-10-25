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

#ifndef XENGINE_PAKBUILDER_HPP
#define XENGINE_PAKBUILDER_HPP

#include "io/pak.hpp"

namespace xng {
    class XENGINE_EXPORT PakBuilder {
    public:
        PakBuilder() = default;

        void addEntry(const std::string &name, const std::vector<char> &buffer);

        std::vector<std::vector<char>> build(size_t chunkSize,
                                             bool compressData,
                                             bool encryptData,
                                             SHA &sha,
                                             GZip &zip,
                                             AES &aes,
                                             const AES::Key &key,
                                             const AES::InitializationVector &iv);

    private:
        std::map<std::string, std::vector<char>> entries;
    };
}

#endif //XENGINE_PAKBUILDER_HPP
