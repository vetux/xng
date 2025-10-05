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
     * Components of an uri can contain any sequence of ascii values only the sequence "://" and colon ":" is reserved.
     *
     * The scheme and asset specification is optional.
     *
     * SCHEME :// FILE : ASSET
     *
     * For example:
     * https://www.domain.com/assets/cube.fbx/
     * memory://shaders/shader.spirv
     * file://mesh/cube.obj
     * /mydir.extension/myfile.fbx:cube.001
     * /mydir.extension/myfile.fbx
     * /mydir/myimage.png
     * /mydir/myfilewithoutextension:cube
     */
    class XENGINE_EXPORT Uri : public Messageable {
    public:
        Uri() = default;

        explicit Uri(const std::string &value) {
            auto schemeIndex = value.find("://");

            auto path = value;
            if (schemeIndex != std::string::npos) {
                scheme = value.substr(0, schemeIndex);
                path = value.substr(schemeIndex + 3);
            }

            auto assetIndex = path.find(':');
            if (assetIndex != std::string::npos) {
                asset = path.substr(assetIndex + 1);
                file = path.substr(0, assetIndex);
            } else {
                file = path;
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

        std::string getExtension() const { return std::filesystem::path(getFile()).extension().string(); }

        std::string toString(bool includeScheme = true) const {
            return (scheme.empty() || !includeScheme ? "" : scheme + "://") + file + (asset.empty() ? "" : ":" + asset);
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
