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

#ifndef XENGINE_ENTITYCONTAINER_HPP
#define XENGINE_ENTITYCONTAINER_HPP

#include <set>
#include <limits>

#include "ecs/componentcontainer.hpp"

namespace xng {
    class XENGINE_EXPORT EntityContainer {
    public:
        EntityContainer() = default;

        ~EntityContainer() = default;

        EntityContainer(const EntityContainer &other) = default;

        EntityContainer(EntityContainer &&other) noexcept = default;

        EntityContainer &operator=(const EntityContainer &other) = default;

        EntityContainer &operator=(EntityContainer &&other) noexcept = default;

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

        EntityHandle create(const std::string &name){
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

        const std::set<EntityHandle> &getEntities() const {
            return entities;
        }

        ComponentContainer &getComponentContainer() {
            return components;
        }

        const ComponentContainer &getComponentContainer() const {
            return components;
        }

    private:
        std::set<int> idStore;
        int idCounter = 0;

        std::map<std::string, EntityHandle> entityNames;
        std::map<EntityHandle, std::string> entityNamesReverse;

        std::set<EntityHandle> entities;
        ComponentContainer components;
    };
}

#endif //XENGINE_ENTITYCONTAINER_HPP
