/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/util/hashcombine.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * Components of an uri can contain any sequence of ascii values only the sequence "://" is reserved.
     *
     * The scheme and asset specification is optional.
     *
     * To specify an asset the file must have an extension.
     *
     * SCHEME :// FILE / ASSET
     *
     * For example:
     * memory://shaders/shader.spirv
     * file://mesh/cube.obj
     * /mydir/myfile.fbx/myAssetDir/cube
     * /mydir/myimage.png
     */
    class XENGINE_EXPORT Uri : public Messageable {
    public:
        Uri() = default;

        explicit Uri(const std::string &value) {
            auto schemeIndex = value.find("://");

            auto pathStr = value;
            if (schemeIndex != std::string::npos) {
                scheme = value.substr(0, schemeIndex);
                pathStr = value.substr(schemeIndex + 3);
            }

            auto path = std::filesystem::path(pathStr);
            if (path.has_parent_path()){
                auto parent = path;
                bool foundExtension = false;
                while(parent.has_parent_path() && parent.has_relative_path()){
                    parent = parent.parent_path();
                    if (parent.has_extension()){
                        asset = pathStr.substr(parent.string().size() + 1);
                        foundExtension = true;
                        break;
                    }
                }
                if (foundExtension){
                    file = parent.string();
                } else {
                    file = pathStr;
                }
            } else {
                file = pathStr;
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

        Messageable &operator<<(const Message &message) override {
            *this = Uri(message.getType() == Message::STRING ? message.asString() : "");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = toString();
            return message;
        }

        const std::string &getScheme() const { return scheme; }

        const std::string &getFile() const { return file; }

        const std::string &getAsset() const { return asset; }

        std::string toString(bool includeScheme = true) const {
            return (scheme.empty() || !includeScheme ? "" : scheme + "://") + file +  (asset.empty() ? "" : "$" + asset);
        }

        bool empty() const {
            return file.empty();
        }

        bool operator==(const Uri &other) const {
            return file == other.file
                   && asset == other.asset
                   && scheme == other.scheme;
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

namespace std {
    template<>
    struct hash<xng::Uri> {
        std::size_t operator()(const xng::Uri &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.getScheme());
            xng::hash_combine(ret, k.getFile());
            xng::hash_combine(ret, k.getAsset());
            return ret;
        }
    };
}

#endif //XENGINE_URI_HPP
