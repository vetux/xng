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

#ifndef XENGINE_ENTITYSCENE_HPP
#define XENGINE_ENTITYSCENE_HPP

#include <set>
#include <limits>
#include <functional>

#include "xng/resource/resource.hpp"

#include "entityhandle.hpp"
#include "componentpool.hpp"
#include "componentregistry.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    class Entity;

    class XENGINE_EXPORT EntityScene : public Resource, public Messageable {
    public:
        class XENGINE_EXPORT Listener {
        public:
            virtual void onEntityCreate(const EntityHandle &entity) {};

            virtual void onEntityDestroy(const EntityHandle &entity) {};

            virtual void onComponentCreate(const EntityHandle &entity, const Component &component) {};

            virtual void onComponentDestroy(const EntityHandle &entity, const Component &component) {};

            virtual void onComponentUpdate(const EntityHandle &entity,
                                           const Component &oldComponent,
                                           const Component &newComponent) {};
        };

        EntityScene() = default;

        ~EntityScene() = default;

        EntityScene(const EntityScene &other);

        EntityScene(EntityScene &&other) noexcept = default;

        EntityScene &operator=(const EntityScene &other);

        EntityScene &operator=(EntityScene &&other) noexcept = default;

        std::unique_ptr<Resource> clone() override;

        std::type_index getTypeIndex() override;

        const std::string &getName() const {
            return name;
        }

        void setName(const std::string &v) {
            name = v;
        }

        /**
         * Optional name mapping.
         * Set the name of the given entity to the passed value.
         * The entity can then be accessed by name by calling getEntityByName().
         *
         * @param entity
         * @param name
         */
        void setEntityName(const EntityHandle &entity, const std::string &name) {
            if (entityNames.find(name) != entityNames.end())
                throw std::runtime_error("Entity with name " + name + " already exists");
            entityNames.insert(std::make_pair(name, entity));
            entityNamesReverse[entity] = name;
        }

        void clearEntityName(const EntityHandle &entity) {
            if (entityNamesReverse.find(entity) == entityNamesReverse.end())
                throw std::runtime_error("Entity does not have a name mapping");
            auto name = entityNamesReverse.at(entity);
            entityNames.erase(name);
            entityNamesReverse.erase(entity);
        }

        const std::string &getEntityName(const EntityHandle &entity) const {
            return entityNamesReverse.at(entity);
        }

        EntityHandle getEntityByName(const std::string &name) const {
            return entityNames.at(name);
        }

        bool entityNameExists(const std::string &name) const {
            return entityNames.find(name) != entityNames.end();
        }

        bool entityHasName(EntityHandle handle) const {
            return entityNamesReverse.find(handle) != entityNamesReverse.end();
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
            setEntityName(ret, name);
            return ret;
        }

        void destroy(const EntityHandle &entity) {
            for (auto &listener: listeners) {
                listener->onEntityDestroy(entity);
            }
            for (auto &p: componentPools) {
                if (p.second->check(entity)) {
                    for (auto &listener: listeners) {

                    }
                    p.second->destroy(entity);
                }
            }
            idStore.insert(entity.id);
            entities.erase(entity);
            auto it = entityNamesReverse.find(entity);
            if (it != entityNamesReverse.end()) {
                entityNames.erase(it->second);
                entityNamesReverse.erase(entity);
            }
        }

        void clear() {
            for (auto &pair: componentPools) {
                for (auto &cpair: pair.second->getComponents()) {
                    for (auto &listener: listeners) {
                        listener->onComponentDestroy(cpair.first, *cpair.second);
                    }
                }
            }
            for (auto &ent: entities) {
                for (auto &listener: listeners) {
                    listener->onEntityDestroy(ent);
                }
            }
            componentPools.clear();
            idStore.clear();
            idCounter = 0;
            entities.clear();
            entityNames.clear();
            entityNamesReverse.clear();
        }

        /**
         * Template interface used when the component type is known at compile time.
         */

        template<typename T>
        ComponentPool<T> &getPool() {
            auto it = componentPools.find(typeid(T));
            if (it == componentPools.end()) {
                componentPools[typeid(T)] = std::make_unique<ComponentPool<T>>();
            }
            return dynamic_cast<ComponentPool<T> &>(*componentPools[typeid(T)]);
        }

        template<typename T>
        const ComponentPool<T> &getPool() const {
            auto it = componentPools.find(typeid(T));
            if (it == componentPools.end()) {
                throw std::runtime_error("Pool does not exist");
            }
            return dynamic_cast<ComponentPool<T> &>(*componentPools.at(typeid(T)));
        }

        template<typename T>
        bool checkPool() const {
            return componentPools.find(typeid(T)) != componentPools.end();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator begin() {
            return getPool<T>().begin();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::const_iterator begin() const {
            return getPool<T>().begin();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator end() {
            return getPool<T>().end();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::const_iterator end() const {
            return getPool<T>().end();
        }

        template<typename T>
        const T &createComponent(const EntityHandle &entity, const T &value = {}) {
            auto &ret = getPool<T>().create(entity, value);
            for (auto &listener: listeners) {
                listener->onComponentCreate(entity, value);
            }
            return ret;
        }

        template<typename T>
        void destroyComponent(const EntityHandle &entity) {
            auto comp = getComponent<T>(entity);
            getPool<T>().destroy(entity);
            for (auto &listener: listeners) {
                listener->onComponentDestroy(entity, comp);
            }
        }

        template<typename T>
        const T &getComponent(const EntityHandle &entity) const {
            return getPool<T>().lookup(entity);
        }

        template<typename T>
        void updateComponent(const EntityHandle &entity, const T &value) {
            for (auto &listener: listeners) {
                listener->onComponentUpdate(entity, getComponent<T>(entity), value);
            }
            getPool<T>().update(entity, value);
        }

        template<typename T>
        bool checkComponent(const EntityHandle &entity) const {
            return checkPool<T>() && getPool<T>().check(entity);
        }

        /**
         * Generic interface use for component types that are not known at compile time.
         */

        void createComponent(const EntityHandle &entity, const std::type_index &type) {
            ComponentRegistry::instance().getConstructor(type)(*this, entity);
        }

        void updateComponent(const EntityHandle &entity,
                             const Component &value) {
            ComponentRegistry::instance().getUpdater(value.getType())(*this, entity, value);
        }

        void destroyComponent(const EntityHandle &entity,
                              const std::type_index &type) {
            componentPools.at(type)->destroy(entity);
        }

        bool checkComponent(const EntityHandle &entity, const std::type_index &type) {
            return checkPool(type) && componentPools.at(type)->check(entity);
        }

        bool checkPool(const std::type_index &type) const {
            return componentPools.find(type) != componentPools.end();
        }

        Entity createEntity();

        Entity createEntity(const std::string &name);

        void destroyEntity(const Entity &entity);

        Entity getEntity(const std::string &name);

        const std::set<EntityHandle> &getEntities() const {
            return entities;
        }

        Messageable &operator<<(const Message &message) override {
            name = message.value("name", std::string());
            if (message.value("entities").getType() == Message::LIST) {
                for (auto &msg: message.value("entities").asList()) {
                    deserializeEntity(msg);
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["name"] = name;
            auto list = std::vector<Message>();
            for (auto &ent: entities) {
                Message msg;
                serializeEntity(ent, msg);
                list.emplace_back(msg);
            }
            message["entities"] = list;
            return message;
        }

        void addListener(Listener &listener) {
            listeners.insert(&listener);
        }

        void removeListener(Listener &listener) {
            listeners.erase(&listener);
        }

        void serializeEntity(const EntityHandle &entity, Message &message) const;

        void deserializeEntity(const Message &message);

    private:
        std::set<int> idStore;
        int idCounter = 0;

        std::map<std::string, EntityHandle> entityNames;
        std::map<EntityHandle, std::string> entityNamesReverse;

        std::set<EntityHandle> entities;
        std::map<std::type_index, std::unique_ptr<ComponentPoolBase>> componentPools;

        std::set<Listener *> listeners;

        std::string name;
    };
}

#endif //XENGINE_ENTITYSCENE_HPP
