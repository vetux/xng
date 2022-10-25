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

#include "ecs/entityscene.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"

namespace xng {
    void EntityScene::serializeEntity(const EntityHandle &entity, Message &message) const {
        message = Message(Message::DICTIONARY);
        auto it = entityNamesReverse.find(entity);
        if (it != entityNamesReverse.end()) {
            message["name"] = it->second;
        }
        auto cmap = std::map<std::string, Message>();
        for (auto &pair: componentPools) {
            if (pair.second->check(entity)) {
                auto serializer = ComponentRegistry::instance().getSerializer(pair.first);
                Message msg;
                serializer(*this, entity, msg);
                cmap[ComponentRegistry::instance().getNameFromType(pair.first)] = msg;
            }
        }

        message["components"] = cmap;
    }

    void EntityScene::deserializeEntity(const Message &message) {
        EntityHandle entity;
        if (message.has("name")) {
            entity = create(message.at("name"));
        } else {
            entity = create();
        }
        for (auto &c: message.value("components", std::map<std::string, Message>())) {
            auto deserializer = ComponentRegistry::instance().getDeserializer(ComponentRegistry::instance().getTypeFromName(c.first));
            deserializer(*this, entity, c.second);
        }
    }

    Entity EntityScene::createEntity() {
        return {create(), *this};
    }

    Entity EntityScene::createEntity(const std::string &name) {
        return {create(name), *this};
    }

    void EntityScene::destroyEntity(const Entity &entity) {
        destroy(entity.getHandle());
    }

    Entity EntityScene::getEntity(const std::string &name) {
        return {getEntityByName(name), *this};
    }

    std::unique_ptr<Resource> EntityScene::clone() {
        return std::make_unique<EntityScene>(*this);
    }

    std::type_index EntityScene::getTypeIndex() {
        return typeid(EntityScene);
    }

    EntityScene::EntityScene(const EntityScene &other) {
        componentPools.clear();
        for (auto &p: other.componentPools) {
            componentPools[p.first] = p.second->clone();
        }
        entities = other.entities;
        entityNames = other.entityNames;
        entityNamesReverse = other.entityNamesReverse;
        name = other.name;
        idCounter = other.idCounter;
        idStore = other.idStore;
    }

    EntityScene &EntityScene::operator=(const EntityScene &other) {
        componentPools.clear();
        for (auto &p: other.componentPools) {
            componentPools[p.first] = p.second->clone();
        }
        entities = other.entities;
        entityNames = other.entityNames;
        entityNamesReverse = other.entityNamesReverse;
        name = other.name;
        idCounter = other.idCounter;
        idStore = other.idStore;
        return *this;
    }
}