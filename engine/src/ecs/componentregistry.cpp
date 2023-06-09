/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/ecs/componentregistry.hpp"
#include "xng/ecs/components.hpp"

namespace xng {
    std::unique_ptr<ComponentRegistry> ComponentRegistry::inst = nullptr;

    ComponentRegistry &ComponentRegistry::instance() {
        if (inst == nullptr) {
            inst = std::make_unique<ComponentRegistry>();
            REGISTER_COMPONENT(AudioListenerComponent)
            REGISTER_COMPONENT(AudioSourceComponent)
            REGISTER_COMPONENT(ButtonComponent)
            REGISTER_COMPONENT(CameraComponent)
            REGISTER_COMPONENT(CanvasComponent)
            REGISTER_COMPONENT(LightComponent)
            REGISTER_COMPONENT(MeshRenderComponent)
            REGISTER_COMPONENT(ParticleComponent)
            REGISTER_COMPONENT(RigidBodyComponent)
            REGISTER_COMPONENT(SkyboxComponent)
            REGISTER_COMPONENT(SpriteAnimationComponent)
            REGISTER_COMPONENT(SpriteComponent)
            REGISTER_COMPONENT(TextComponent)
            REGISTER_COMPONENT(TransformComponent)
            REGISTER_COMPONENT(RectTransformComponent)
        }
        return *inst;
    }

    bool ComponentRegistry::registerComponent(std::type_index type,
                                              const std::string &typeName,
                                              const Serializer &serializer,
                                              const Deserializer &deserializer,
                                              const Constructor &constructor,
                                              const Updater &updater) noexcept {
        if (nameMapping.find(type) != nameMapping.end()) {
            return false;
        }

        nameMapping[type] = typeName;
        nameReverseMapping.insert(std::pair(typeName, type));
        serializers[type] = serializer;
        deserializers[type] = deserializer;
        constructors[type] = constructor;
        updaters[type] = updater;
        return true;
    }

    void ComponentRegistry::unregisterComponent(std::type_index type) noexcept {
        auto &name = nameMapping.at(type);
        nameReverseMapping.erase(name);
        nameMapping.erase(type);
        serializers.erase(type);
        deserializers.erase(type);
        constructors.erase(type);
        updaters.erase(type);
    }

    const std::type_index &ComponentRegistry::getTypeFromName(const std::string &typeName) {
        return nameReverseMapping.at(typeName);
    }

    const std::string &ComponentRegistry::getNameFromType(const std::type_index &index) {
        return nameMapping.at(index);
    }

    bool ComponentRegistry::checkTypeName(const std::string &typeName) {
        return nameReverseMapping.find(typeName) != nameReverseMapping.end();
    }

    const ComponentRegistry::Serializer &ComponentRegistry::getSerializer(const std::type_index &index) {
        return serializers.at(index);
    }

    const ComponentRegistry::Deserializer &ComponentRegistry::getDeserializer(const std::type_index &index) {
        return deserializers.at(index);
    }

    const ComponentRegistry::Constructor &ComponentRegistry::getConstructor(const std::type_index &index) {
        return constructors.at(index);
    }

    const ComponentRegistry::Updater &ComponentRegistry::getUpdater(const std::type_index &index) {
        return updaters.at(index);
    }

    const std::map<std::type_index, std::string> &ComponentRegistry::getComponents() {
        return nameMapping;
    }
}