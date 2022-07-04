/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_COMPONENTPOOL_HPP
#define XENGINE_COMPONENTPOOL_HPP

#include <map>
#include <stdexcept>
#include <set>

#include "ecs/entityhandle.hpp"

namespace xng {
    class XENGINE_EXPORT ComponentPoolBase {
    public:
        virtual ~ComponentPoolBase() = default;

        virtual ComponentPoolBase *clone() = 0;

        virtual void clear() = 0;

        virtual void destroy(const EntityHandle &entity) = 0;
    };

    template<typename T>
    class XENGINE_EXPORT ComponentPool : public ComponentPoolBase {
    public:
        class XENGINE_EXPORT Listener {
        public:
            virtual void onComponentCreate(const EntityHandle &entity, const T &component) = 0;

            virtual void onComponentDestroy(const EntityHandle &entity, const T &component) = 0;

            virtual void onComponentUpdate(const EntityHandle &entity, const T &oldValue, const T &newValue) = 0;
        };

        ComponentPool() = default;

        ComponentPool(const ComponentPool<T> &other) {
            listeners = other.listeners;
            components = other.components;
        }

        ~ComponentPool() override = default;

        ComponentPoolBase *clone() override {
            return new ComponentPool<T>(*this);
        }

        void clear() override {
            for (auto &pair: components) {
                for (auto &l: listeners) {
                    l->onComponentDestroy(pair.first, pair.second);
                }
            }
            components.clear();
        }

        void destroy(const EntityHandle &entity) override {
            if (components.find(entity) != components.end()) {
                for (auto &listener: listeners) {
                    listener->onComponentDestroy(entity, components.at(entity));
                }
                components.erase(entity);
            }
        }

        typename std::map<EntityHandle, T>::iterator begin() {
            return components.begin();
        }

        typename std::map<EntityHandle, T>::iterator end() {
            return components.end();
        }

        const T &create(const EntityHandle &entity, const T &value = {}) {
            if (components.find(entity) != components.end())
                throw std::runtime_error("Entity "
                                         + std::to_string(entity.id)
                                         + " already has component of type "
                                         + typeid(T).name());
            auto &comp = components[entity];
            comp = value;
            for (auto &listener: listeners) {
                listener->onComponentCreate(entity, comp);
            }
            return comp;
        }

        const T &lookup(const EntityHandle &entity) const {
            return components.at(entity);
        }

        /**
         * Update the component value, components can only be updated by calling this method.
         *
         * The pool calls the onComponentUpdate callback on all listeners.
         *
         * @param entity
         * @param value
         * @return True if the component was not present and was created, otherwise false
         */
        bool update(const EntityHandle &entity, const T &value = {}) {
            auto it = components.find(entity);
            if (it == components.end()) {
                create(entity, value);
                return true;
            } else {
                auto &comp = it->second;
                auto val = comp;
                comp = value;
                for (auto &listener: listeners) {
                    listener->onComponentUpdate(entity, val, comp);
                }
                return false;
            }
        }

        bool check(const EntityHandle &entity) const {
            return components.find(entity) != components.end();
        }

        void addListener(Listener *listener) {
            listeners.insert(listener);
        }

        void removeListener(Listener *listener) {
            listeners.erase(listener);
        }

    private:
        std::set<Listener *> listeners;
        std::map<EntityHandle, T> components;
    };
}

#endif //XENGINE_COMPONENTPOOL_HPP
