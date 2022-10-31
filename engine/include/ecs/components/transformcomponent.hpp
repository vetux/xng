/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_TRANSFORMCOMPONENT_HPP
#define XENGINE_TRANSFORMCOMPONENT_HPP

#include <string>

#include "math/transform.hpp"
#include "ecs/entityscene.hpp"
#include "ecs/component.hpp"
#include "io/messageable.hpp"

namespace xng {
    struct AssetScene;

    struct XENGINE_EXPORT TransformComponent : public Component {
        static Transform walkHierarchy(const TransformComponent &component, EntityScene &entityManager);

        bool enabled = true;
        Transform transform;
        std::string parent; //The name of the parent transform entity

        Messageable &operator<<(const Message &message) override {
            enabled = message.value("enabled", true);
            transform << message.value("transform");
            parent = message.value("parent", std::string());
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["enabled"] = enabled;
            transform >> message["transform"];
            message["parent"] = parent;
            return message;
        }

        std::type_index getType() const override {
            return typeid(TransformComponent);
        }
    };
}

#endif //XENGINE_TRANSFORMCOMPONENT_HPP