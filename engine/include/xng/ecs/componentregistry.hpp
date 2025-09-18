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

#ifndef XENGINE_COMPONENTREGISTRY_HPP
#define XENGINE_COMPONENTREGISTRY_HPP

#include <string>
#include <functional>
#include <memory>

#include "xng/io/message.hpp"

#include "xng/ecs/component.hpp"

namespace xng {
    class EntityScene;

    struct EntityHandle;

    class Messageable;
}

/**
 * Register the given component type to the registry.
 * The component type must extend Component and be default constructable
 */
#define REGISTER_COMPONENT(type) \
xng::ComponentRegistry::instance().registerComponent(#type, \
                                                                             [](const xng::EntityScene &scene,\
                                                                                xng::EntityHandle ent, xng::Message &msg) {\
                                                                                 scene.getComponent<type>(ent) >> msg;\
                                                                             },\
                                                                             [](xng::EntityScene &scene, xng::EntityHandle ent,\
                                                                                const xng::Message &msg) {\
                                                                                 type comp;\
                                                                                 comp << msg;\
                                                                                 scene.createComponent<type>(ent, comp);\
                                                                             },\
                                                                             [](xng::EntityScene &scene, xng::EntityHandle ent) {\
                                                                                 scene.createComponent<type>(ent);\
                                                                             },\
                                                                             [](xng::EntityScene &scene,\
                                                                                xng::EntityHandle ent,\
                                                                                const Component &value) {\
                                                                                 scene.updateComponent(ent,\
                                                                                                       dynamic_cast<const type &>(\
                                                                                                               value));\
                                                                             });

#define UNREGISTER_COMPONENT(type) xng::ComponentRegistry::instance().unregisterComponent(typeid(type));

namespace xng {
    /**
     * The component registry is used by the entity scene when serializing / deserializing and when using the non template interface of the entity scene.
     *
     * Users have to register component types which are added to the entity scene to serialize / deserialize them and to be able to use the non template interface.
     */
    class XENGINE_EXPORT ComponentRegistry {
    public:
        typedef std::function<void(const EntityScene &, EntityHandle, Message &)> Serializer;
        typedef std::function<void(EntityScene &, EntityHandle, const Message &)> Deserializer;
        typedef std::function<void(EntityScene &scene, EntityHandle ent)> Constructor;
        typedef std::function<void(EntityScene &scene, EntityHandle ent, const Component &value)> Updater;

        /**
         * @return The component registry instance with the engine components already registered.
         */
        static ComponentRegistry &instance();

        void registerComponent(const std::string &typeName,
                               const Serializer &serializer,
                               const Deserializer &deserializer,
                               const Constructor &constructor,
                               const Updater &updater);

        void unregisterComponent(const std::string &typeName);

        bool checkTypeName(const std::string &typeName);

        const Serializer &getSerializer(const std::string &typeName);

        const Deserializer &getDeserializer(const std::string &typeName);

        const Constructor &getConstructor(const std::string &typeName);

        const Updater &getUpdater(const std::string &typeName);

    private:
        static std::unique_ptr<ComponentRegistry> inst;

        std::map<std::string, Serializer> serializers;
        std::map<std::string, Deserializer> deserializers;
        std::map<std::string, Constructor> constructors;
        std::map<std::string, Updater> updaters;
    };
}

#endif //XENGINE_COMPONENTREGISTRY_HPP
