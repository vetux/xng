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

#ifndef XENGINE_GENERICMAP_HPP
#define XENGINE_GENERICMAP_HPP

#include <map>
#include <any>

namespace xng {
    template<typename KeyType>
    class XENGINE_EXPORT GenericMap {
    public:
        GenericMap() = default;

        explicit GenericMap(std::map<KeyType, std::any> values) : values(std::move(values)) {}

        const std::any &getAny(const KeyType &key) const {
            return values.at(key);
        }

        template<typename T>
        const T &get(const KeyType &key, const T &defaultValue) const {
            if (values.find(key) == values.end()) {
                return defaultValue;
            } else {
                return std::any_cast<const T &>(getAny(key));
            }
        }

        template<typename T>
        T get(const KeyType &key) const {
            if (values.find(key) == values.end()) {
                return T();
            } else {
                return std::any_cast<const T &>(getAny(key));
            }
        }

        void set(const KeyType &key, const std::any &value) {
            values[key] = value;
        }

        void erase(const KeyType &key) {
            values.erase(key);
        }

        void clear() {
            values.clear();
        }

        bool has(const KeyType &key) const {
            return values.find(key) != values.end();
        }

        typename std::map<KeyType, std::any>::iterator find(const KeyType &key) {
            return values.find(key);
        }

        typename std::map<KeyType, std::any>::iterator &begin() {
            return values.begin();
        }

        typename std::map<KeyType, std::any>::iterator end() {
            return values.end();
        }

        typename std::map<KeyType, std::any>::iterator find(const KeyType &key) const {
            return values.find(key);
        }

        typename std::map<KeyType, std::any>::iterator begin() const {
            return values.begin();
        }

        typename std::map<KeyType, std::any>::iterator end() const {
            return values.end();
        }

    private:
        std::map<KeyType, std::any> values;
    };

    typedef GenericMap<std::string> GenericMapString;
}

#endif //XENGINE_GENERICMAP_HPP
