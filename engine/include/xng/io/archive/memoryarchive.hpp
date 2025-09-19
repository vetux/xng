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

#ifndef XENGINE_MEMORYARCHIVE_HPP
#define XENGINE_MEMORYARCHIVE_HPP

#include <map>
#include <vector>
#include <sstream>

#include "xng/io/archive.hpp"

namespace xng {
    class XENGINE_EXPORT MemoryArchive : public Archive {
    public:
        MemoryArchive() = default;

        ~MemoryArchive() override = default;

        bool exists(const std::string &name) override {
            return data.find(name) != data.end();
        }

        std::unique_ptr<std::istream> open(const std::string &path) override {
            auto buf = data.at(path);
            const auto ret = new std::stringstream(std::string(buf.begin(), buf.end()));
            std::noskipws(*ret);
            return std::unique_ptr<std::istream>(ret);
        }

        std::unique_ptr<std::iostream> openRW(const std::string &path) override {
            throw std::runtime_error("No RW in memory archive allowed");
        }

        void addData(const std::string &path, const std::vector<uint8_t> &bytes) {
            if (data.find(path) != data.end())
                throw std::runtime_error("Data already exists at " + path);
            data[path] = bytes;
        }

        void removeData(const std::string &path) {
            data.erase(path);
        }

    private:
        std::map<std::string, std::vector<uint8_t>> data;
    };
}
#endif //XENGINE_MEMORYARCHIVE_HPP
