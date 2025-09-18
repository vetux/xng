/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/ecs/entityhandle.hpp"
#include "xng/ecs/componentpool.hpp"
#include "xng/ecs/componentregistry.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    class Entity;

    class XENGINE_EXPORT EntityScene final : public Resource, public Messageable {
    public:
        RESOURCE_TYPENAME(EntityScene)

        class XENGINE_EXPORT Listener {
        public:
            virtual ~Listener() = default;

            virtual void onEntityCreate(const EntityHandle &entity) {
            };

            virtual void onEntityDestroy(const EntityHandle &entity) {
            };

            virtual void onEntityNameChanged(const EntityHandle &entity,
                                             const std::string &newName,
                                             const std::string &oldName) {
            };

            virtual void onComponentCreate(const EntityHandle &entity, const Component &component) {
            };

            virtual void onComponentDestroy(const EntityHandle &entity, const Component &component) {
            };

            virtual void onComponentUpdate(const EntityHandle &entity,
                                           const Component &oldComponent,
                                           const Component &newComponent) {
            };
        };

        EntityScene() = default;

        EntityScene(const EntityScene &other);

        EntityScene(EntityScene &&other) noexcept = default;

        EntityScene &operator=(const EntityScene &other);

        EntityScene &operator=(EntityScene &&other) noexcept = default;

        std::unique_ptr<Resource> clone() override;

        const std::string &getName() const {
            return sceneName;
        }

        void setName(const std::string &v) {
            sceneName = v;
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
            const auto it = entityNamesReverse.find(entity);
            std::string oldName;
            if (it != entityNamesReverse.end()) {
                oldName = it->second;
            }
            entityNamesReverse.erase(entity);
            entityNames.erase(oldName);
            if (!name.empty()) {
                if (entityNames.find(name) != entityNames.end())
                    throw std::runtime_error("Entity with name " + name + " already exists");
                entityNames.insert(std::make_pair(name, entity));
                entityNamesReverse[entity] = name;
            }
            for (auto &listener: listeners) {
                listener->onEntityNameChanged(entity, name, oldName);
            }
        }

        const std::string &getEntityName(const EntityHandle &entity) const {
            return entityNamesReverse.at(entity);
        }

        EntityHandle getEntityByName(const std::string &name) const {
            if (name.empty()) {
                throw std::runtime_error("Empty name passed to getEntityByName");
            }
            return entityNames.at(name);
        }

        bool entityNameExists(const std::string &name) const {
            if (name.empty()) {
                return false;
            }
            return entityNames.find(name) != entityNames.end();
        }

        bool entityHasName(const EntityHandle handle) const {
            return entityNamesReverse.find(handle) != entityNamesReverse.end();
        }

        EntityHandle create() {
            if (idStore.empty()) {
                if (idCounter == std::numeric_limits<int>::max())
                    throw std::runtime_error("Cannot create entity, id overflow");
                const auto ret = EntityHandle(idCounter++);
                entities.insert(ret);
                for (auto &listener: listeners) {
                    listener->onEntityCreate(ret);
                }
                return ret;
            } else {
                const auto it = idStore.begin();
                const EntityHandle ret(*it);
                idStore.erase(it);
                entities.insert(ret);
                for (auto &listener: listeners) {
                    listener->onEntityCreate(ret);
                }
                return ret;
            }
        }

        EntityHandle create(const std::string &name) {
            const auto ret = create();
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
            auto it = componentPools.find(T::typeName);
            if (it == componentPools.end()) {
                componentPools[T::typeName] = std::make_unique<ComponentPool<T> >();
            }
            return dynamic_cast<ComponentPool<T> &>(*componentPools.at(T::typeName));
        }

        template<typename T>
        const ComponentPool<T> &getPool() const {
            auto it = componentPools.find(T::typeName);
            if (it == componentPools.end()) {
                throw std::runtime_error("Pool does not exist");
            }
            return dynamic_cast<ComponentPool<T> &>(*componentPools.at(T::typeName));
        }

        template<typename T>
        bool checkPool() const {
            return componentPools.find(T::typeName) != componentPools.end();
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
            auto oldValue = getComponent<T>(entity);
            getPool<T>().update(entity, value);
            for (auto &listener: listeners) {
                listener->onComponentUpdate(entity, oldValue, value);
            }
        }

        template<typename T>
        bool checkComponent(const EntityHandle &entity) const {
            return checkPool<T>() && getPool<T>().check(entity);
        }

        Entity createEntity();

        Entity createEntity(const std::string &name);

        void destroyEntity(const Entity &entity);

        Entity getEntity(const std::string &name);

        const std::set<EntityHandle> &getEntities() const {
            return entities;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("sceneName", sceneName);
            if (message.getMessage("entities").getType() == Message::LIST) {
                for (auto &msg: message.getMessage("entities").asList()) {
                    deserializeEntity(msg);
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["sceneName"] = sceneName;
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
        std::map<std::string, std::unique_ptr<ComponentPoolBase> > componentPools;

        std::set<Listener *> listeners;

        std::string sceneName;
    };
}

#endif //XENGINE_ENTITYSCENE_HPP
