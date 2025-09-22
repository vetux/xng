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

#include "xng/ecs/componentregistry.hpp"
#include "xng/ecs/components.hpp"

namespace xng {
    std::unique_ptr<ComponentRegistry> ComponentRegistry::inst = nullptr;

    ComponentRegistry &ComponentRegistry::instance() {
        if (inst == nullptr) {
            inst = std::make_unique<ComponentRegistry>();
            REGISTER_COMPONENT(xng::AudioListenerComponent)
            REGISTER_COMPONENT(xng::AudioSourceComponent)

            REGISTER_COMPONENT(xng::ButtonComponent)
            REGISTER_COMPONENT(xng::CanvasComponent)
            REGISTER_COMPONENT(xng::FlexLayoutComponent)
            REGISTER_COMPONENT(xng::ImageComponent)
            REGISTER_COMPONENT(xng::ScrollBoxComponent)
            REGISTER_COMPONENT(xng::TextComponent)

            REGISTER_COMPONENT(xng::ColliderComponent)
            REGISTER_COMPONENT(xng::MeshColliderComponent)
            REGISTER_COMPONENT(xng::RigidBodyComponent)

            REGISTER_COMPONENT(xng::CameraComponent)
            REGISTER_COMPONENT(xng::DirectionalLightComponent)
            REGISTER_COMPONENT(xng::PointLightComponent)
            REGISTER_COMPONENT(xng::SpotLightComponent)
            REGISTER_COMPONENT(xng::MeshComponent)
            REGISTER_COMPONENT(xng::ParticleComponent)
            REGISTER_COMPONENT(xng::SkyboxComponent)
            REGISTER_COMPONENT(xng::SpriteAnimationComponent)
            REGISTER_COMPONENT(xng::SpriteComponent)
            REGISTER_COMPONENT(xng::TransformComponent)
        }
        return *inst;
    }

    void ComponentRegistry::registerComponent(const std::string &typeName,
                                              const Serializer &serializer,
                                              const Deserializer &deserializer,
                                              const Constructor &constructor,
                                              const Updater &updater) {
        if (serializers.find(typeName) != serializers.end()) {
            throw std::runtime_error("Duplicate component type name registration for typeName " + typeName);
        }

        serializers[typeName] = serializer;
        deserializers[typeName] = deserializer;
        constructors[typeName] = constructor;
        updaters[typeName] = updater;
    }

    void ComponentRegistry::unregisterComponent(const std::string &typeName) {
        serializers.erase(typeName);
        deserializers.erase(typeName);
        constructors.erase(typeName);
        updaters.erase(typeName);
    }

    bool ComponentRegistry::checkTypeName(const std::string &typeName) {
        return serializers.find(typeName) != serializers.end();
    }

    const ComponentRegistry::Serializer &ComponentRegistry::getSerializer(const std::string &typeName) {
        return serializers.at(typeName);
    }

    const ComponentRegistry::Deserializer &ComponentRegistry::getDeserializer(const std::string &typeName) {
        return deserializers.at(typeName);
    }

    const ComponentRegistry::Constructor &ComponentRegistry::getConstructor(const std::string &typeName) {
        return constructors.at(typeName);
    }

    const ComponentRegistry::Updater &ComponentRegistry::getUpdater(const std::string &typeName) {
        return updaters.at(typeName);
    }
}