/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
#include <utility>

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

        virtual void destroy(const EntityHandle &entity) = 0;

        virtual std::map<EntityHandle, std::reference_wrapper<Component> > getComponents() = 0;

        template<typename T>
        const T &get(const EntityHandle &entity) const {
            return down_cast<const T &>(get(entity));
        }
    };

    /**
     * Component pools handle the memory layout of components.
     *
     * @tparam T The concrete type of the component must extend Component
     */
    template<typename T>
    class ComponentPool final : public ComponentPoolBase {
    public:
        struct ComponentPair {
            EntityHandle entity;
            T component;

            ComponentPair() = default;

            ComponentPair(const EntityHandle &entity, const T &component)
                : entity(entity),
                  component(component) {
            }
        };

        ComponentPool() = default;

        ComponentPool(const ComponentPool &other)
            : components(other.components) {
            updateMapping();
        }

        ~ComponentPool() override = default;

        std::unique_ptr<ComponentPoolBase> clone() override {
            return std::make_unique<ComponentPool>(*this);
        }

        void clear() override {
            components.clear();
            entityComponentMapping.clear();
            entityComponentIndexMapping.clear();
        }

        bool check(const EntityHandle &entity) const override {
            return entityComponentMapping.find(entity) != entityComponentMapping.end();
        }

        const Component &get(const EntityHandle &entity) const override {
            return getComponent(entity);
        }

        void destroy(const EntityHandle &entity) override {
            destroyComponent(entity);
        }

        std::map<EntityHandle, std::reference_wrapper<Component> > getComponents() override {
            auto ret = std::map<EntityHandle, std::reference_wrapper<Component> >();
            for (auto &pair: components) {
                ret.emplace(pair.entity, pair.component);
            }
            return ret;
        }

        typename std::vector<ComponentPair>::const_iterator begin() const {
            return components.begin();
        }

        typename std::vector<ComponentPair>::const_iterator end() const {
            return components.end();
        }

        void create(const EntityHandle &entity, const T &value = {}) {
            createComponent(entity, value);
        }

        const T &lookup(const EntityHandle &entity) const {
            return getComponent(entity);
        }

        void update(const EntityHandle &entity, const T &value = {}) {
            updateComponent(entity, value);
        }

    private:
        void createComponent(const EntityHandle &entity, const T &value) {
            if (entityComponentMapping.find(entity) != entityComponentMapping.end())
                throw std::runtime_error("Entity "
                                         + std::to_string(entity.id)
                                         + " already has component of type "
                                         + T::typeName);
            components.emplace_back(ComponentPair(entity, value));
            updateMapping();
        }

        void destroyComponent(const EntityHandle &entity) {
            if (entityComponentMapping.find(entity) != entityComponentMapping.end()) {
                auto index = entityComponentIndexMapping.at(entity);
                components.erase(components.begin() + index);
                updateMapping();
            }
        }

        const T &getComponent(const EntityHandle &entity) const {
            return getPair(entity).component;
        }

        const ComponentPair &getPair(const EntityHandle &entity) const {
            return entityComponentMapping.at(entity).get();
        }

        void updateComponent(const EntityHandle &entity, const T &value) {
            auto it = entityComponentIndexMapping.find(entity);
            if (it == entityComponentIndexMapping.end()) {
                throw std::runtime_error("No component for type "
                                         + std::string(T::typeName)
                                         + " on entity "
                                         + entity.toString());
            }
            components.at(it->second).component = value;
        }

        void updateMapping() {
            entityComponentMapping.clear();
            for (auto i = 0; i < components.size(); i++) {
                auto &pair = components.at(i);
                entityComponentIndexMapping[pair.entity] = i;
                entityComponentMapping.emplace(pair.entity, pair);
            }
        }

        std::vector<ComponentPair> components;

        std::unordered_map<EntityHandle,
            std::reference_wrapper<const ComponentPair>,
            EntityHandleHash> entityComponentMapping;

        std::unordered_map<EntityHandle, size_t, EntityHandleHash> entityComponentIndexMapping;
    };
}

#endif //XENGINE_COMPONENTPOOL_HPP
