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

#ifndef XENGINE_ENTITY_HPP
#define XENGINE_ENTITY_HPP

#include "ecs/entityhandle.hpp"
#include "ecs/entitycontainer.hpp"
#include "ecs/componentcontainer.hpp"

namespace xng {
    /**
     * Convenience wrapper around the container classes
     */
    class Entity {
    public:
        Entity() = default;

        Entity(EntityHandle handle,
               EntityContainer &entityContainer)
                : handle(handle),
                  entityContainer(&entityContainer) {}

        void setName(const std::string &name) {
            checkPointer();
            entityContainer->setName(handle, name);
        }

        void clearName() {
            checkPointer();
            entityContainer->clearName(handle);
        }

        const std::string &getName() {
            checkPointer();
            return entityContainer->getName(handle);
        }

        template<typename T>
        const T &createComponent(const T &value = {}) {
            checkPointer();
            return entityContainer->getComponentContainer().create<T>(handle, value);
        }

        template<typename T>
        void destroyComponent() {
            checkPointer();
            return entityContainer->getComponentContainer().destroy(handle);
        }

        template<typename T>
        const T &getComponent() {
            checkPointer();
            return entityContainer->getComponentContainer().lookup<T>(handle);
        }

        template<typename T>
        bool updateComponent(const T &value = {}) {
            checkPointer();
            return entityContainer->getComponentContainer().update<T>(handle, value);
        }

        template<typename T>
        bool checkComponent(const T &value = {}) {
            checkPointer();
            return entityContainer->getComponentContainer().check<T>(value);
        }

        const EntityHandle &getHandle() const {
            checkPointer();
            return handle;
        }

        EntityContainer &getEntityContainer() {
            checkPointer();
            return *entityContainer;
        }

        const EntityContainer &getEntityContainer() const {
            checkPointer();
            return *entityContainer;
        }

    private:
        void checkPointer() const {
            if (entityContainer == nullptr)
                throw std::runtime_error("Entity not initialized.");
        }

        EntityHandle handle = EntityHandle(0);
        EntityContainer *entityContainer = nullptr;
    };
}

#endif //XENGINE_ENTITY_HPP
