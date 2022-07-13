/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "ecs/entitycontainer.hpp"

#include "ecs/components.hpp"

namespace xng {
    void EntityContainer::serializeEntity(const EntityHandle &entity, Message &message) const {
        message = Message(Message::DICTIONARY);
        auto it = entityNamesReverse.find(entity);
        if (it != entityNamesReverse.end()) {
            message["name"] = it->second;
        }
        auto clist = std::vector<Message>();
        if (components.check<TransformComponent>(entity)) {
            Message component;
            components.lookup<TransformComponent>(entity) >> component;
            component["type"] = "transform";
            clist.emplace_back(component);
        }
        message["components"] = clist;
    }

    void EntityContainer::deserializeEntity(const Message &message) {
        EntityHandle entity(0);
        if (message.has("name")) {
            entity = create(message.at("name"));
        } else {
            entity = create();
        }
        for (auto &c: message.value("components", std::vector<Message>())) {
            deserializeComponent(entity, c);
        }
    }

    void EntityContainer::deserializeComponent(const EntityHandle &entity, const Message &message) {
        auto type = message.value("type", std::string(""));
        if (type == "transform") {
            TransformComponent component;
            component << message;
            components.create(entity, component);
        }
    }
}