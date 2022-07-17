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

#ifndef XENGINE_PAKARCHIVE_HPP
#define XENGINE_PAKARCHIVE_HPP

#include <mutex>

#include "io/archive.hpp"
#include "io/pak.hpp"

namespace xng {
    class XENGINE_EXPORT PakArchive : public Archive {
    public:
        PakArchive() = default;

        PakArchive(std::vector<std::reference_wrapper<std::istream>> streams,
                   bool verifyHashes,
                   GZip &gzip,
                   SHA &sha);

        PakArchive(std::vector<std::reference_wrapper<std::istream>> streams,
                   bool verifyHashes,
                   GZip &gzip,
                   SHA &sha,
                   AES &aes,
                   AES::Key key,
                   AES::InitializationVector iv);

        ~PakArchive() override = default;

        bool exists(const std::string &path) override;

        std::unique_ptr<std::istream> open(const std::string &path) override;

        std::unique_ptr<std::iostream> openRW(const std::string &name) override;

    private:
        std::mutex mutex;
        Pak pak;
        bool verifyHashes;
    };
}

#endif //XENGINE_PAKARCHIVE_HPP
