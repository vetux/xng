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

#ifndef XENGINE_ENTITYMANAGER_HPP
#define XENGINE_ENTITYMANAGER_HPP

#include <set>
#include <limits>

#include "ecs/componentmanager.hpp"

namespace xengine {
    class XENGINE_EXPORT EntityManager {
    public:
        EntityManager() = default;

        ~EntityManager() = default;

        EntityManager(const EntityManager &other) = default;

        EntityManager(EntityManager &&other) noexcept = default;

        EntityManager &operator=(const EntityManager &other) = default;

        EntityManager &operator=(EntityManager &&other) noexcept = default;

        /**
         * Optional name mapping.
         * Set the name of the given entity to the passed value.
         * The entity can then be accessed by name by calling getByName().
         *
         * @param entity
         * @param name
         */
        void setName(const Entity &entity, const std::string &name) {
            if (entityNames.find(name) != entityNames.end())
                throw std::runtime_error("Entity with name " + name + " already exists");
            entityNames[name] = entity;
            entityNamesReverse[entity] = name;
        }

        const std::string &getName(const Entity &entity) {
            return entityNamesReverse.at(entity);
        }

        Entity getByName(const std::string &name) {
            return entityNames.at(name);
        }

        Entity create() {
            if (idStore.empty()) {
                if (idCounter == std::numeric_limits<int>::max())
                    throw std::runtime_error("Cannot create entity, id overflow");
                auto ret = Entity(idCounter++);
                entities.insert(ret);
                return ret;
            } else {
                auto it = idStore.begin();
                Entity ret(*it);
                idStore.erase(it);
                entities.insert(ret);
                return ret;
            }
        }

        void destroy(const Entity &entity) {
            componentManager.destroy(entity);
            idStore.insert(entity.id);
            entities.erase(entity);
            entityNames.erase(entityNamesReverse.at(entity));
            entityNamesReverse.erase(entity);
        }

        void clear() {
            componentManager.clear();
            idStore.clear();
            idCounter = 0;
            entities.clear();
            entityNames.clear();
            entityNamesReverse.clear();
        }

        const std::set<Entity> &getEntities() const {
            return entities;
        }

        ComponentManager &getComponentManager() {
            return componentManager;
        }

        const ComponentManager &getComponentManager() const {
            return componentManager;
        }

    private:
        std::set<int> idStore;
        int idCounter = 0;
        std::set<Entity> entities;

        std::map<std::string, Entity> entityNames;
        std::map<Entity, std::string> entityNamesReverse;

        ComponentManager componentManager;
    };
}

#endif //XENGINE_ENTITYMANAGER_HPP
