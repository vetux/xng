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

#ifndef MANA_ASSETPATH_HPP
#define MANA_ASSETPATH_HPP

#include <string>

#include <tuple>
#include <utility>

namespace xengine {
    struct MANA_EXPORT AssetPath {
        AssetPath() = default;

        AssetPath(std::string bundle, std::string asset)
                : bundle(std::move(bundle)), asset(std::move(asset)) {}

        std::string bundle; //The path to the bundle
        std::string asset; //The name of the asset in the bundle

        std::size_t hash() const {
            std::hash<std::string> hash;
            return hash(bundle + "%" + asset);
        }

        bool empty() const { return bundle.empty() && asset.empty(); }

        bool operator<(const AssetPath &other) const {
            return std::tie(bundle, asset) < std::tie(other.bundle, other.asset);
        }

        bool operator==(const AssetPath &other) const {
            return std::tie(bundle, asset) == std::tie(other.bundle, other.asset);
        }

        struct MANA_EXPORT Hash {
            std::size_t operator()(const AssetPath &key) const {
                return key.hash();
            }
        };

        template<int S>
        struct MANA_EXPORT HashArray {
            std::size_t operator()(const std::array<AssetPath, S> &value) const {
                std::string hashStr;
                for (auto &item: value)
                    hashStr += item.bundle + "%" + item.asset;
                std::hash<std::string> hash;
                return hash(hashStr);
            }
        };
    };
}
#endif //MANA_ASSETPATH_HPP
