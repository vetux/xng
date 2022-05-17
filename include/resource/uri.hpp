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

#ifndef XENGINE_URI_HPP
#define XENGINE_URI_HPP

#include <string>
#include <tuple>
#include <utility>

namespace xengine {
    /**
     * Eg. /mydir/myfile.fbx:cube
     * or /mydir/myimage.png
     */
    class Uri {
    public:
        Uri() = default;

        explicit Uri(const std::string &value) {
            auto it = value.find(':');
            if (it != std::string::npos) {
                file = value.substr(0, it);
                asset = value.substr(it + 1);
            } else {
                file = value;
            }
        }

        Uri(std::string file, std::string asset)
                : file(std::move(file)), asset(std::move(asset)) {}

        Uri(const Uri &other) = default;

        Uri(Uri &&other) = default;

        Uri &operator=(const Uri &other) = default;

        Uri &operator=(Uri &&other) = default;

        bool operator()() const{
            return !empty();
        }
        /**
         * eg. /mydir/myfile.fbx
         * @return
         */
        const std::string &getFile() const { return file; }

        /**
         * eg. cube
         * @return
         */
        const std::string &getAsset() const { return asset; }

        std::string toString() const {
            return file + ":" + asset;
        }

        bool empty() const {
            return file.empty();
        }

        bool operator==(const Uri &other) const {
            return std::tie(file, asset) < std::tie(other.file, other.asset);
        }

        bool operator<(const Uri &other) const {
            return std::tie(file, asset) < std::tie(other.file, other.asset);
        }

    private:
        std::string file;
        std::string asset;
    };
}
#endif //XENGINE_URI_HPP
