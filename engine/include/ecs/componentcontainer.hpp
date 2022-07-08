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

#ifndef XENGINE_COMPONENTCONTAINER_HPP
#define XENGINE_COMPONENTCONTAINER_HPP

#include <map>
#include <vector>
#include <typeindex>

#include "ecs/entityhandle.hpp"

#include "ecs/componentpool.hpp"

namespace xng {
    /**
     * Any default constructable type can be used as a component.
     *
     * The engine defines some default components which are used by provided default systems (Rendering, Physics etc.)
     * The user can define custom component types and systems which use these types.
     */
    class XENGINE_EXPORT ComponentContainer {
    public:
        ComponentContainer() = default;

        ~ComponentContainer() {
            for (auto &p: pools) {
                delete p.second;
            }
        }

        ComponentContainer(const ComponentContainer &other) {
            for (auto &p: other.pools) {
                pools[p.first] = p.second->clone();
            }
        }

        ComponentContainer(ComponentContainer &&other) noexcept = default;

        ComponentContainer &operator=(const ComponentContainer &other) = default;

        ComponentContainer &operator=(ComponentContainer &&other) noexcept = default;

        template<typename T>
        ComponentPool<T> &getPool() {
            auto it = pools.find(typeid(T));
            if (it == pools.end()) {
                pools[typeid(T)] = new ComponentPool<T>();
            }
            return dynamic_cast<ComponentPool<T> &>(*pools[typeid(T)]);
        }

        template<typename T>
        const ComponentPool<T> &getPool() const {
            auto it = pools.find(typeid(T));
            if (it == pools.end()) {
                throw std::runtime_error("Pool does not exist");
            }
            return dynamic_cast<ComponentPool<T> &>(*pools.at(typeid(T)));
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator begin() {
            return getPool<T>().begin();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator begin() const {
            return getPool<T>().begin();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator end() {
            return getPool<T>().end();
        }

        template<typename T>
        typename std::map<EntityHandle, T>::iterator end() const {
            return getPool<T>().end();
        }

        template<typename T>
        const T &create(const EntityHandle &entity, const T &value = {}) {
            return getPool<T>().create(entity, value);
        }

        template<typename T>
        void destroy(const EntityHandle &entity) {
            getPool<T>()->destroy(entity);
        }

        void destroy(const EntityHandle &entity) {
            for (auto &p: pools) {
                p.second->destroy(entity);
            }
        }

        void clear() {
            for (auto &p: pools) {
                p.second->clear();
            }
        }

        template<typename T>
        const T &lookup(const EntityHandle &entity) const {
            return getPool<T>().lookup(entity);
        }

        template<typename T>
        bool update(const EntityHandle &entity, const T &value) {
            return getPool<T>().update(entity, value);
        }

        template<typename T>
        bool check(const EntityHandle &entity) const {
            return getPool<T>().check(entity);
        }

    private:
        std::map<std::type_index, ComponentPoolBase *> pools;
    };
}

#endif //XENGINE_COMPONENTCONTAINER_HPP
