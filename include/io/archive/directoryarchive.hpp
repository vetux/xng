/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_DIRECTORYARCHIVE_HPP
#define MANA_DIRECTORYARCHIVE_HPP

#include <fstream>
#include <filesystem>

#include "io/archive.hpp"

namespace xengine {
    /**
     * A directory representing an archive.
     * Only files relative to the specified directory can be accessed by full or relative path,
     */
    class MANA_EXPORT DirectoryArchive : public Archive {
    public:
        std::string directory;

        DirectoryArchive() = default;

        explicit DirectoryArchive(std::string directory) : directory(std::move(directory)) {}

        ~DirectoryArchive() override = default;

        bool exists(const std::string &name) override {
            auto ret = std::filesystem::exists(name);
            if (ret) {
                return ret;
            } else {
                return std::filesystem::exists(directory + name);
            }
        }

        std::unique_ptr<std::istream> open(const std::string &path) override {
            std::string targetPath;

            //Allow full paths which reference files relative to the directory
            if (path.find(directory) == 0 && std::filesystem::exists(path)) {
                targetPath = path;
            } else {
                //Allow relative paths without leading slash
                targetPath = directory + (path.find('/') == 0 ? "" : "/") + path;
            }

            auto ret = std::make_unique<std::fstream>(targetPath);
            if (!*ret) {
                throw std::runtime_error("Failed to open file " + targetPath);
            }
            return ret;
        }
    };
}

#endif //MANA_DIRECTORYARCHIVE_HPP
