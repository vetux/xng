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

#ifndef XENGINE_ENTITY_HPP
#define XENGINE_ENTITY_HPP

#include "ecs/entityhandle.hpp"
#include "ecs/entityscene.hpp"
#include "ecs/componentcontainer.hpp"

namespace xng {
    /**
     * Wrapper class for representing entities as objects in code.
     */
    class Entity {
    public:
        Entity() = default;

        Entity(EntityHandle handle,
               EntityScene &entityContainer)
                : handle(handle),
                  scene(&entityContainer) {}

        void setName(const std::string &name) {
            checkPointer();
            scene->setEntityName(handle, name);
        }

        void clearName() {
            checkPointer();
            scene->clearEntityName(handle);
        }

        const std::string &getName() {
            checkPointer();
            return scene->getEntityName(handle);
        }

        template<typename T>
        const T &createComponent(const T &value = {}) {
            checkPointer();
            return scene->createComponent<T>(handle, value);
        }

        template<typename T>
        void destroyComponent() {
            checkPointer();
            return scene->destroyComponent<T>(handle);
        }

        template<typename T>
        const T &getComponent() {
            checkPointer();
            return scene->lookup<T>(handle);
        }

        template<typename T>
        bool updateComponent(const T &value = {}) {
            checkPointer();
            return scene->updateComponent<T>(handle, value);
        }

        template<typename T>
        bool checkComponent(const T &value = {}) {
            checkPointer();
            return scene->check<T>(value);
        }

        const EntityHandle &getHandle() const {
            checkPointer();
            return handle;
        }

        EntityScene &getScene() {
            checkPointer();
            return *scene;
        }

        const EntityScene &getScene() const {
            checkPointer();
            return *scene;
        }

    private:
        void checkPointer() const {
            if (scene == nullptr)
                throw std::runtime_error("Entity not initialized.");
        }

        EntityHandle handle = EntityHandle(0);
        EntityScene *scene = nullptr;
    };
}

#endif //XENGINE_ENTITY_HPP
