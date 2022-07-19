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

#ifndef XENGINE_COMPONENTPOOL_HPP
#define XENGINE_COMPONENTPOOL_HPP

#include <map>
#include <stdexcept>
#include <set>
#include <any>

#include "ecs/entityhandle.hpp"

namespace xng {
    class XENGINE_EXPORT ComponentPoolBase {
    public:
        virtual ~ComponentPoolBase() = default;

        virtual std::unique_ptr<ComponentPoolBase> clone() = 0;

        virtual void clear() = 0;

        virtual void destroy(const EntityHandle &entity) = 0;

        virtual std::map<EntityHandle, std::any> getComponents() = 0;
    };

    template<typename T>
    class XENGINE_EXPORT ComponentPool : public ComponentPoolBase {
    public:
        ComponentPool() = default;

        ComponentPool(const ComponentPool<T> &other) {
            components = other.components;
        }

        ~ComponentPool() override = default;

        std::unique_ptr<ComponentPoolBase> clone() override {
            return std::make_unique<ComponentPool<T>>(*this);
        }

        std::map<EntityHandle, std::any> getComponents() override {
            auto ret = std::map<EntityHandle, std::any>();
            for (auto &pair: components) {
                ret[pair.first] = pair.second;
            }
            return ret;
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
            return comp;
        }

        void destroy(const EntityHandle &entity) override {
            if (components.find(entity) != components.end()) {
                components.erase(entity);
            }
        }

        void clear() override {
            components.clear();
        }

        const T &lookup(const EntityHandle &entity) const {
            return components.at(entity);
        }

        bool update(const EntityHandle &entity, const T &value = {}) {
            auto it = components.find(entity);
            if (it == components.end()) {
                create(entity, value);
                return true;
            } else {
                it->second = value;
                return false;
            }
        }

        bool check(const EntityHandle &entity) const {
            return components.find(entity) != components.end();
        }

    private:
        std::map<EntityHandle, T> components;
    };
}

#endif //XENGINE_COMPONENTPOOL_HPP
