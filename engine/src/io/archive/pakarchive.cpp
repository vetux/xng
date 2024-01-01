/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/io/archive/pakarchive.hpp"

#include <filesystem>
#include <sstream>
#include <utility>

namespace xng {
    PakArchive::PakArchive(Pak pak, bool verifyHashes)
            : pak(std::move(pak)),
              verifyHashes(verifyHashes) {}

    bool PakArchive::exists(const std::string &path) {
        return pak.exists(path);
    }

    std::unique_ptr<std::istream> PakArchive::open(const std::string &path) {
        std::lock_guard<std::mutex> guard(mutex);
        auto data = pak.get(path, verifyHashes);
        auto ret = std::make_unique<std::stringstream>(std::string(data.begin(), data.end()));
        std::noskipws(*ret);
        return std::move(ret);
    }

    std::unique_ptr<std::iostream> PakArchive::openRW(const std::string &name) {
        throw std::runtime_error("Writing to pak is not supported");
    }
}