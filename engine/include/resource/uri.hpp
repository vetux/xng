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

#ifndef XENGINE_URI_HPP
#define XENGINE_URI_HPP

#include <string>
#include <tuple>
#include <utility>
#include <filesystem>
#include <cassert>

#include "algo/hashcombine.hpp"

namespace xng {
    /**
     * Components of an uri can contain any sequence of ascii values only the sequence "://" and the '$' character are reserved.
     *
     * The scheme and asset specification is optional.
     *
     * SCHEME :// FILE $ ASSET
     *
     * Eg.
     * memory://shaders/shader.spirv
     * file://mesh/cube.obj
     * /mydir/myfile.fbx$cube
     * /mydir/myimage.png
     */
    class XENGINE_EXPORT Uri {
    public:
        Uri() = default;

        explicit Uri(const std::string &value) {
            auto schemeIndex = value.find("://");
            auto assetIndex = value.find('$');

            if (schemeIndex != std::string::npos) {
                scheme = value.substr(0, schemeIndex);
            }

            if (assetIndex != std::string::npos) {
                if (schemeIndex != std::string::npos) {
                    file = value.substr(schemeIndex + 1, assetIndex - (schemeIndex + 1));
                } else {
                    file = value.substr(0, assetIndex);
                }
                if (assetIndex + 1 < value.size()) {
                    asset = value.substr(assetIndex + 1);
                }
            } else {
                if (schemeIndex != std::string::npos) {
                    file = value.substr(schemeIndex + 1);
                } else {
                    file = value;
                }
            }
        }

        Uri(std::string file, std::string asset)
                : file(std::move(file)), asset(std::move(asset)) {}

        Uri(std::string scheme, std::string file, std::string asset)
                : scheme(std::move(scheme)), file(std::move(file)), asset(std::move(asset)) {}

        Uri(const Uri &other) = default;

        Uri(Uri &&other) = default;

        Uri &operator=(const Uri &other) = default;

        Uri &operator=(Uri &&other) = default;

        bool operator()() const {
            return !empty();
        }

        const std::string &getScheme() const { return scheme; }

        const std::string &getFile() const { return file; }

        const std::string &getAsset() const { return asset; }

        std::string toString() const {
            return (scheme.empty() ? "" : scheme) + file + ":" + asset;
        }

        bool empty() const {
            return file.empty();
        }

        bool operator==(const Uri &other) const {
            return std::tie(file, asset, scheme) < std::tie(other.file, other.asset, other.scheme);
        }

        bool operator<(const Uri &other) const {
            return std::tie(file, asset, scheme) < std::tie(other.file, other.asset, other.scheme);
        }

    private:
        std::string scheme;
        std::string file;
        std::string asset;
    };
}

using namespace xng;
namespace std {
    template<>
    struct hash<Uri> {
        std::size_t operator()(const Uri &k) const {
            size_t ret = 0;
            hash_combine(ret, k.getScheme());
            hash_combine(ret, k.getFile());
            hash_combine(ret, k.getAsset());
            return ret;
        }
    };
}

#endif //XENGINE_URI_HPP
