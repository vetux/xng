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

#include <fstream>
#include <filesystem>

#include "xng/io/archive/directoryarchive.hpp"

namespace xng {
    DirectoryArchive::DirectoryArchive(std::filesystem::path directory, bool readOnly)
            : directory(std::move(directory)),
              readOnly(readOnly) {}

    bool DirectoryArchive::exists(const std::string &name) {
        auto ret = std::filesystem::exists(name);
        if (ret) {
            return ret;
        } else {
            auto dir = directory;
            dir.append(name);
            return std::filesystem::exists(dir);
        }
    }

    std::unique_ptr<std::istream> DirectoryArchive::open(const std::string &path) {
        auto targetPath = getAbsolutePath(path);

        auto ret = std::make_unique<std::ifstream>(targetPath.string(), std::ios_base::in | std::ios_base::binary);

        if (!*ret) {
            throw std::runtime_error("Failed to open file " + targetPath.string());
        }

        return std::move(ret);
    }

    std::unique_ptr<std::iostream> DirectoryArchive::openRW(const std::string &path) {
        if (readOnly)
            throw std::runtime_error("Attempted to open RW on read only directory archive.");

        auto targetPath = getAbsolutePath(path);

        auto ret = std::make_unique<std::fstream>(targetPath.string(), std::ios_base::in | std::ios_base::binary);

        if (!*ret) {
            throw std::runtime_error("Failed to open file " + targetPath.string());
        }

        return std::move(ret);
    }

    std::filesystem::path DirectoryArchive::getAbsolutePath(const std::string &path) {
        if (path.find(directory.string()) == 0 && std::filesystem::exists(path)) {
            //Allow absolute paths which reference files relative to the directory
            return path;
        } else if (path.find('/') != 0) {
            //Allow relative paths without leading slash
            return directory.string() + "/" + path;
        } else {
            return directory.string() + path;
        }
    }
}