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
#include <any>

#include "ecs/componentcontainer.hpp"
#include "resource/resource.hpp"

#include "io/messageable.hpp"

namespace xng {
    class Entity;

    class XENGINE_EXPORT EntityScene : public Resource, public Messageable {
    public:
        class Listener {
        public:
            virtual void onEntityCreate(const EntityHandle &entity) {};

            virtual void onEntityDestroy(const EntityHandle &entity) {};

            virtual void onComponentCreate(const EntityHandle &entity,
                                           const std::any &component,
                                           std::type_index componentType) {};

            virtual void onComponentDestroy(const EntityHandle &entity,
                                            const std::any &component,
                                            std::type_index componentType) {};

            virtual void onComponentUpdate(const EntityHandle &entity,
                                           const std::any &oldComponent,
                                           const std::any &newComponent,
                                           std::type_index componentType) {};
        };

        EntityScene() = default;

        ~EntityScene() = default;

        EntityScene(const EntityScene &other) = default;

        EntityScene(EntityScene &&other) noexcept = default;

        EntityScene &operator=(const EntityScene &other) = default;

        EntityScene &operator=(EntityScene &&other) noexcept = default;

        std::unique_ptr<Resource> clone() override;

        std::type_index getTypeIndex() override;

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
                for (auto &listener: listeners) {
                    listener->onEntityCreate(ret);
                }
                return ret;
            } else {
                auto it = idStore.begin();
                EntityHandle ret(*it);
                idStore.erase(it);
                entities.insert(ret);
                for (auto &listener: listeners) {
                    listener->onEntityCreate(ret);
                }
                return ret;
            }
        }

        EntityHandle create(const std::string &name) {
            auto ret = create();
            setName(ret, name);
            return ret;
        }

        void destroy(const EntityHandle &entity) {
            for (auto &listener: listeners) {
                listener->onEntityDestroy(entity);
            }
            components.destroy(entity);
            idStore.insert(entity.id);
            entities.erase(entity);
            entityNames.erase(entityNamesReverse.at(entity));
            entityNamesReverse.erase(entity);
        }

        void clear() {
            for (auto &pair: components.getPools()) {
                for (auto &cpair: pair.second->getComponents()) {
                    for (auto &listener: listeners) {
                        listener->onComponentDestroy(cpair.first, cpair.second, pair.first);
                    }
                }
            }
            for (auto &ent: entities) {
                for (auto &listener: listeners) {
                    listener->onEntityDestroy(ent);
                }
            }
            components.clear();
            idStore.clear();
            idCounter = 0;
            entities.clear();
            entityNames.clear();
            entityNamesReverse.clear();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator begin() {
            return components.begin<T>();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator begin() const {
            return components.begin<T>();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator end() {
            return components.end<T>();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator end() const {
            return components.end<T>();
        }

        template<typename T>
        const T &createComponent(const EntityHandle &entity, const T &value = {}) {
            for (auto &listener: listeners) {
                listener->onComponentCreate(entity, value, typeid(T));
            }
            return components.create(entity, value);
        }

        template<typename T>
        void destroyComponent(const EntityHandle &entity) {
            for (auto &listener: listeners) {
                listener->onComponentDestroy(entity, lookupComponent<T>(entity), typeid(T));
            }
            components.getPool<T>()->destroy(entity);
        }

        template<typename T>
        const T &lookupComponent(const EntityHandle &entity) const {
            return components.lookup<T>(entity);
        }

        template<typename T>
        bool updateComponent(const EntityHandle &entity, const T &value) {
            for (auto &listener: listeners) {
                listener->onComponentUpdate(entity, lookupComponent<T>(entity), value, typeid(T));
            }
            return components.update(entity, value);
        }

        template<typename T>
        bool checkComponent(const EntityHandle &entity) const {
            return components.check<T>(entity);
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

        void addListener(Listener &listener) {
            listeners.insert(&listener);
        }

        void removeListener(Listener &listener) {
            listeners.erase(&listener);
        }

        /**
         * To serialize user component types the user can subclass entity container and override these methods
         * to define serialization logic for the user component types.
         */
        virtual void serializeEntity(const EntityHandle &entity, Message &message) const;

        virtual void deserializeEntity(const Message &message);

        virtual void deserializeComponent(const EntityHandle &entity, const std::string &type, const Message &message);

    private:
        std::set<int> idStore;
        int idCounter = 0;

        std::map<std::string, EntityHandle> entityNames;
        std::map<EntityHandle, std::string> entityNamesReverse;

        std::set<EntityHandle> entities;
        ComponentContainer components;

        std::set<Listener *> listeners;
    };
}

#endif //XENGINE_ENTITYSCENE_HPP
