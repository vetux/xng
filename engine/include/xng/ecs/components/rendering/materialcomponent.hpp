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

#ifndef XENGINE_MATERIALCOMPONENT_HPP
#define XENGINE_MATERIALCOMPONENT_HPP

#include "xng/graphics/scene/material.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT MaterialComponent final : Component {
        XNG_COMPONENT_TYPENAME(MaterialComponent)

        /**
         *  The materials for each index where index 0 is the root mesh
         *  and index - 1 are indices of subMeshes in the root mesh.
         */
        std::map<size_t, ResourceHandle<Material>> materials;

        bool operator==(const MaterialComponent &other) const {
            return enabled == other.enabled
                   && materials == other.materials;
        }

        Messageable &operator<<(const Message &message) override {
            Message dict;
            message.value("materials", dict);
            if (dict.getType() == Message::DICTIONARY) {
                for (auto &pair: dict.asDictionary()) {
                    ResourceHandle < Material > material;
                    material << pair.second;
                    size_t key = std::stoul(pair.first);
                    materials[key] = material;
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            Message dict = Message(Message::DICTIONARY);
            for (auto &pair: materials) {
                std::string key = std::to_string(pair.first);
                Message value;
                pair.second >> value;
                dict[key.c_str()] = value;
            }
            message["materials"] = dict;
            return message;
        }
    };
}

#endif //XENGINE_MATERIALCOMPONENT_HPP
