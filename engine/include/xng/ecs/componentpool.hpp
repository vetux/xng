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

#ifndef XENGINE_COMPONENTPOOL_HPP
#define XENGINE_COMPONENTPOOL_HPP

#include <map>
#include <stdexcept>

#include "xng/ecs/entityhandle.hpp"
#include "xng/ecs/component.hpp"

#include "xng/util/downcast.hpp"

namespace xng {
    class ComponentPoolBase {
    public:
        virtual ~ComponentPoolBase() = default;

        virtual std::unique_ptr<ComponentPoolBase> clone() = 0;

        virtual void clear() = 0;

        virtual bool check(const EntityHandle &entity) const = 0;

        virtual const Component &get(const EntityHandle &entity) const = 0;

        virtual std::vector<std::unique_ptr<Component>> destroy(const EntityHandle &entity) = 0;

        virtual std::map<EntityHandle, Component *> getComponents() = 0;

        template<typename T>
        const T &get(const EntityHandle &entity) const {
            return down_cast<const T &>(get(entity));
        }
    };

    /**
     * Component pools handle the memory layout of components.
     *
     * @tparam T The concrete type of the component, must extend Component
     */
    template<typename T>
    class ComponentPool : public ComponentPoolBase {
    public:
        ComponentPool() = default;

        ComponentPool(const ComponentPool<T> &other) {
            components = other.components;
        }

        ~ComponentPool() override = default;

        std::unique_ptr<ComponentPoolBase> clone() override {
            return std::make_unique<ComponentPool<T>>(*this);
        }


        void clear() override {
            components.clear();
        }

        bool check(const EntityHandle &entity) const override {
            return components.find(entity) != components.end();
        }

        const Component &get(const EntityHandle &entity) const override {
            return components.at(entity);
        }

        std::vector<std::unique_ptr<Component>> destroy(const EntityHandle &entity) override {
            std::vector<std::unique_ptr<Component>> ret;
            if (components.find(entity) != components.end()) {
                ret.emplace_back(std::make_unique<T>(components.at(entity)));
                components.erase(entity);
            }
            return ret;
        }

        std::map<EntityHandle, Component *> getComponents() override {
            auto ret = std::map<EntityHandle, Component *>();
            for (auto &pair: components) {
                ret[pair.first] = &pair.second;
            }
            return ret;
        }

        typename std::map<EntityHandle, T>::const_iterator begin() const {
            return components.begin();
        }

        typename std::map<EntityHandle, T>::const_iterator end() const {
            return components.end();
        }

        const T &create(const EntityHandle &entity, const T &value = {}) {
            if (components.find(entity) != components.end())
                throw std::runtime_error("Entity "
                                         + std::to_string(entity.id)
                                         + " already has component of type "
                                         + T::typeName);
            auto &comp = components[entity];
            comp = value;
            return comp;
        }

        const T &lookup(const EntityHandle &entity) const {
            return components.at(entity);
        }

        void update(const EntityHandle &entity, const T &value = {}) {
            auto it = components.find(entity);
            if (it == components.end()) {
                throw std::runtime_error("No component for type "
                                         + std::string(T::typeName)
                                         + " on entity "
                                         + entity.toString());
            } else {
                it->second = value;
            }
        }

    private:
        std::map<EntityHandle, T> components; // TODO: Allocate components in contiguous memory.
    };
}

#endif //XENGINE_COMPONENTPOOL_HPP
