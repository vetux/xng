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

#ifndef XENGINE_RESOURCEBUNDLE_HPP
#define XENGINE_RESOURCEBUNDLE_HPP

#include <map>
#include <string>
#include <memory>
#include <typeindex>
#include <stdexcept>

#include "resource.hpp"

namespace xng {
    class XENGINE_EXPORT ResourceBundle {
    public:
        ResourceBundle() = default;

        ~ResourceBundle() {
            assets.clear();
        }

        ResourceBundle(const ResourceBundle &other) {
            *this = other;
        }

        ResourceBundle &operator=(const ResourceBundle &other) {
            for (auto &pair: other.assets) {
                assets[pair.first] = pair.second->clone();
            }

            return *this;
        }

        ResourceBundle(ResourceBundle &&other) = default;

        ResourceBundle &operator=(ResourceBundle &&other) = default;

        template<typename T>
        const T &get(const std::string &name = "") const {
            return dynamic_cast<const T &>(get(name));
        }

        const Resource &get(const std::string &name = "") const {
            if (assets.empty())
                throw std::runtime_error("Empty bundle map");

            if (name.empty()) {
                return *assets.begin()->second;
            } else {
                return *assets.at(name);
            }
        }

        void add(const std::string &name, std::unique_ptr<Resource> ptr) {
            assets[name] = std::move(ptr);
        }

        void remove(const std::string &name) {
            assets.erase(name);
        }

        std::map<std::string, std::unique_ptr<Resource>> assets;
    };
}

#endif //XENGINE_RESOURCEBUNDLE_HPP
