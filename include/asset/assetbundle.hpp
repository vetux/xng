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

#ifndef XENGINE_ASSETBUNDLE_HPP
#define XENGINE_ASSETBUNDLE_HPP

#include <map>
#include <string>
#include <memory>
#include <typeindex>

#include "asset/asset.hpp"

namespace xengine {
    class XENGINE_EXPORT AssetBundle {
    public:
        ~AssetBundle() {
            assets.clear();
        }

        AssetBundle() = default;

        AssetBundle(const AssetBundle &other) {
            *this = other;
        }

        AssetBundle &operator=(const AssetBundle &other) {
            for (auto &pair: other.assets) {
                auto index = pair.first;
                for (auto &assetMap: pair.second)
                    for (auto &asset: assetMap.second)
                        assets[index][assetMap.first].emplace_back(asset->clone());
            }

            return *this;
        }

        AssetBundle(AssetBundle &&other) = default;

        AssetBundle &operator=(AssetBundle &&other) = default;

        template<typename T>
        const T &get(const std::string &name = "") const {
            if (assets.empty())
                throw std::runtime_error("Empty bundle map");

            auto index = std::type_index(typeid(T));

            if (name.empty()) {
                return dynamic_cast<const T &>(
                        dynamic_cast<Asset<T> &>(*assets.at(index).begin()->second.at(0)).instance
                );
            } else {
                return dynamic_cast<const T &>(
                        dynamic_cast<Asset<T> &>(*assets.at(index).at(name).at(0)).instance
                );
            }
        }

        template<typename T>
        void add(const std::string &name, const T &asset) {
            auto index = std::type_index(typeid(T));
            assets[index][name].emplace_back(std::move(std::make_unique<Asset<T>>(asset)));
        }

        template<typename T>
        void remove(const std::string &name) {
            auto index = std::type_index(typeid(T));
            assets.at(index).at(name).clear();
        }

        std::map<std::type_index, std::map<std::string, std::vector<std::unique_ptr<AssetBase>>>> assets;
    };
}

#endif //XENGINE_ASSETBUNDLE_HPP
