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

#ifndef XENGINE_RESOURCEBUNDLE_HPP
#define XENGINE_RESOURCEBUNDLE_HPP

#include <map>
#include <string>
#include <memory>
#include <stdexcept>

#include "xng/resource/resource.hpp"

#include "xng/util/downcast.hpp"

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
            return down_cast<const T &>(get(name, T::typeName));
        }

        const Resource &get(const std::string &name, const std::string &typeName) const {
            if (assets.empty())
                throw std::runtime_error("Empty bundle map");

            if (name.empty()) {
                auto r = getAll(typeName);
                if (r.size() == 0) {
                    throw std::runtime_error("No resource of type " + typeName + " found in bundle");
                }
                return r.at(0);
            }

            auto &ret = get(name);
            if (ret.getTypeName() != typeName) {
                throw std::runtime_error("Invalid resource cast, typeName: "
                                         + ret.getTypeName()
                                         + " Requested: "
                                         + typeName);
            }
            return ret;
        }

        const Resource &get(const std::string &name) const {
            if (assets.empty())
                throw std::runtime_error("Empty bundle map");

            if (name.empty()) {
                return *assets.begin()->second;
            }
            auto it = assets.find(name);
            if (it == assets.end()) {
                throw std::runtime_error("Resource not found: " + name);
            }
            return *it->second;
        }

        template<typename T>
        std::vector<std::reference_wrapper<const T>> getAll() const {
            std::vector<std::reference_wrapper<const T>> ret;
            for (auto &pair: assets) {
                if (T::typeName == pair.second->getTypeName()) {
                    ret.emplace_back(down_cast<const T &>(*pair.second));
                }
            }
            return ret;
        }

        std::vector<std::reference_wrapper<const Resource>> getAll(const std::string &typeName) const {
            std::vector<std::reference_wrapper<const Resource>> ret;
            for (auto &pair: assets) {
                if (pair.second->getTypeName() == typeName) {
                    ret.emplace_back(*pair.second);
                }
            }
            return ret;
        }

        void add(const std::string &name, std::unique_ptr<Resource> ptr) {
            assets[name] = std::move(ptr);
        }

        void remove(const std::string &name) {
            assets.erase(name);
        }

        bool has(const std::string &name) {
            return assets.find(name) != assets.end();
        }

        std::map<std::string, std::unique_ptr<Resource> > assets;
    };
}

#endif //XENGINE_RESOURCEBUNDLE_HPP
