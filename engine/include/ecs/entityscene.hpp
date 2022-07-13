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

#ifndef XENGINE_ENTITYSCENE_HPP
#define XENGINE_ENTITYSCENE_HPP

#include <set>
#include <limits>

#include "ecs/componentcontainer.hpp"

#include "io/messageable.hpp"

namespace xng {
    class Entity;

    class XENGINE_EXPORT EntityScene : public Messageable {
    public:
        EntityScene() = default;

        ~EntityScene() = default;

        EntityScene(const EntityScene &other) = default;

        EntityScene(EntityScene &&other) noexcept = default;

        EntityScene &operator=(const EntityScene &other) = default;

        EntityScene &operator=(EntityScene &&other) noexcept = default;

        /**
         * Optional name mapping.
         * Set the name of the given entity to the passed value.
         * The entity can then be accessed by name by calling getByName().
         *
         * @param entity
         * @param name
         */
        void setName(const EntityHandle &entity, const std::string &name) {
            if (entityNames.find(name) != entityNames.end())
                throw std::runtime_error("Entity with name " + name + " already exists");
            entityNames.insert(std::make_pair(name, entity));
            entityNamesReverse[entity] = name;
        }

        void clearName(const EntityHandle &entity) {
            if (entityNamesReverse.find(entity) == entityNamesReverse.end())
                throw std::runtime_error("Entity does not have a name mapping");
            auto name = entityNamesReverse.at(entity);
            entityNames.erase(name);
            entityNamesReverse.erase(entity);
        }

        const std::string &getName(const EntityHandle &entity) const {
            return entityNamesReverse.at(entity);
        }

        EntityHandle getByName(const std::string &name) const {
            return entityNames.at(name);
        }

        EntityHandle create() {
            if (idStore.empty()) {
                if (idCounter == std::numeric_limits<int>::max())
                    throw std::runtime_error("Cannot create entity, id overflow");
                auto ret = EntityHandle(idCounter++);
                entities.insert(ret);
                return ret;
            } else {
                auto it = idStore.begin();
                EntityHandle ret(*it);
                idStore.erase(it);
                entities.insert(ret);
                return ret;
            }
        }

        EntityHandle create(const std::string &name) {
            auto ret = create();
            setName(ret, name);
            return ret;
        }

        void destroy(const EntityHandle &entity) {
            components.destroy(entity);
            idStore.insert(entity.id);
            entities.erase(entity);
            entityNames.erase(entityNamesReverse.at(entity));
            entityNamesReverse.erase(entity);
        }

        void clear() {
            components.clear();
            idStore.clear();
            idCounter = 0;
            entities.clear();
            entityNames.clear();
            entityNamesReverse.clear();
        }

        Entity createEntity();

        Entity createEntity(const std::string &name);

        void destroyEntity(const Entity &entity);

        Entity getEntity(const std::string &name);

        const std::set<EntityHandle> &getEntities() const {
            return entities;
        }

        ComponentContainer &getComponentContainer() {
            return components;
        }

        const ComponentContainer &getComponentContainer() const {
            return components;
        }

        Messageable &operator<<(const Message &message) override {
            for (auto &msg: message.asList()) {
                deserializeEntity(msg);
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            auto list = std::vector<Message>();
            for (auto &ent: entities) {
                Message msg;
                serializeEntity(ent, msg);
                list.emplace_back(msg);
            }
            message = list;
            return message;
        }

        /**
         * To serialize user component types the user can subclass entity container and override these methods
         * to define serialization logic for the user component types.
         */
        virtual void serializeEntity(const EntityHandle &entity, Message &message) const;

        virtual void deserializeEntity(const Message &message);

        virtual void deserializeComponent(const EntityHandle &entity, const Message &message);

    private:
        std::set<int> idStore;
        int idCounter = 0;

        std::map<std::string, EntityHandle> entityNames;
        std::map<EntityHandle, std::string> entityNamesReverse;

        std::set<EntityHandle> entities;
        ComponentContainer components;
    };
}

#endif //XENGINE_ENTITYSCENE_HPP
