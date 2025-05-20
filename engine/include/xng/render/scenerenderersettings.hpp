/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_SCENERENDERERSETTINGS_HPP
#define XENGINE_SCENERENDERERSETTINGS_HPP

#include "xng/util/genericmap.hpp"

namespace xng {
    struct SceneRendererSetting {
        std::string name;
        std::type_index type;
        std::any defaultValue;

        SceneRendererSetting(std::string name,
                             const std::type_index &type,
                             std::any defaultValue)
                : name(std::move(name)), type(type), defaultValue(std::move(defaultValue)) {}
    };

    class SceneRendererSettings {
    public:
        template<typename T>
        T get(const SceneRendererSetting &setting) const {
            if (std::type_index(typeid(T)) != setting.type) {
                throw std::runtime_error("Requested setting type is invalid");
            }
            if (map.has(setting.name))
                return map.get<T>(setting.name);
            else
                return std::any_cast<T>(setting.defaultValue);
        }

        template<typename T>
        void set(const SceneRendererSetting &setting, const T &value) {
            if (std::type_index(typeid(T)) != setting.type) {
                throw std::runtime_error("Requested setting type is invalid");
            }
            map.set(setting.name, value);
        }

    private:
        GenericMapString map;
    };
}

#endif //XENGINE_SCENERENDERERSETTINGS_HPP
