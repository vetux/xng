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

#ifndef XENGINE_DIRECTORYARCHIVE_HPP
#define XENGINE_DIRECTORYARCHIVE_HPP

#include "xng/io/archive.hpp"

namespace xng {
    /**
     * A directory representing an archive.
     * Only files relative to the specified directory can be accessed by full or relative path,
     */
    class XENGINE_EXPORT DirectoryArchive : public Archive {
    public:
        DirectoryArchive() = default;

        explicit DirectoryArchive(std::filesystem::path directory, bool readOnly = true);

        ~DirectoryArchive() override = default;

        bool exists(const std::string &name) override;

        std::unique_ptr<std::istream> open(const std::string &path) override;

        std::unique_ptr<std::iostream> openRW(const std::string &path) override;

    private:
        std::filesystem::path getAbsolutePath(const std::string &path);

        std::filesystem::path directory;
        bool readOnly = true;
    };
}

#endif //XENGINE_DIRECTORYARCHIVE_HPP
