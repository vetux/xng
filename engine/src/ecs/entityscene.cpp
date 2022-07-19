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

#include "ecs/entityscene.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"

namespace xng {

#define SERIALIZE_COMPONENT(NAME, TYPE) \
    if (components.check<TYPE>(entity)) {\
        Message component; \
        components.lookup<TYPE>(entity) >> component; \
        cmap[NAME] = component; \
    }

    void EntityScene::serializeEntity(const EntityHandle &entity, Message &message) const {
        message = Message(Message::DICTIONARY);
        auto it = entityNamesReverse.find(entity);
        if (it != entityNamesReverse.end()) {
            message["name"] = it->second;
        }
        auto cmap = std::map<std::string, Message>();

        if (components.check<AudioSourceComponent>(entity)) {
            Message component;
            components.lookup<AudioSourceComponent>(entity) >> component;
            cmap["test"] = component;
        }

        SERIALIZE_COMPONENT("audio_listener", AudioListenerComponent)
        SERIALIZE_COMPONENT("audio_source", AudioSourceComponent)
        SERIALIZE_COMPONENT("button", ButtonComponent)
        SERIALIZE_COMPONENT("camera", CameraComponent)
        SERIALIZE_COMPONENT("canvas", CanvasComponent)
        SERIALIZE_COMPONENT("collider", ColliderComponent)
        SERIALIZE_COMPONENT("light", LightComponent)
        SERIALIZE_COMPONENT("render", MeshRenderComponent)
        SERIALIZE_COMPONENT("particle", ParticleComponent)
        SERIALIZE_COMPONENT("rect_transform", RectTransform)
        SERIALIZE_COMPONENT("rigidbody", RigidBodyComponent)
        SERIALIZE_COMPONENT("skybox", SkyboxComponent)
        SERIALIZE_COMPONENT("sprite_animation", SpriteAnimationComponent)
        SERIALIZE_COMPONENT("sprite", SpriteComponent)
        SERIALIZE_COMPONENT("text", TextComponent)
        SERIALIZE_COMPONENT("transform", TransformComponent)

        message["components"] = cmap;
    }

    void EntityScene::deserializeEntity(const Message &message) {
        EntityHandle entity(0);
        if (message.has("name")) {
            entity = create(message.at("name"));
        } else {
            entity = create();
        }
        for (auto &c: message.value("components", std::map<std::string, Message>())) {
            deserializeComponent(entity, c.first, c.second);
        }
    }

#define DESERIALIZE_COMPONENT(NAME, TYPE) \
    } else if (type == (NAME)) { \
        TYPE component;  \
        component << message; \
        createComponent(entity, component); \


    void EntityScene::deserializeComponent(const EntityHandle &entity, const std::string &type, const Message &message) {
        if (false) {
        DESERIALIZE_COMPONENT("audio_listener", AudioListenerComponent)
        DESERIALIZE_COMPONENT("audio_source", AudioSourceComponent)
        DESERIALIZE_COMPONENT("button", ButtonComponent)
        DESERIALIZE_COMPONENT("camera", CameraComponent)
        DESERIALIZE_COMPONENT("canvas", CanvasComponent)
        DESERIALIZE_COMPONENT("collider", ColliderComponent)
        DESERIALIZE_COMPONENT("light", LightComponent)
        DESERIALIZE_COMPONENT("render", MeshRenderComponent)
        DESERIALIZE_COMPONENT("particle", ParticleComponent)
        DESERIALIZE_COMPONENT("rect_transform", RectTransform)
        DESERIALIZE_COMPONENT("rigidbody", RigidBodyComponent)
        DESERIALIZE_COMPONENT("skybox", SkyboxComponent)
        DESERIALIZE_COMPONENT("sprite_animation", SpriteAnimationComponent)
        DESERIALIZE_COMPONENT("sprite", SpriteComponent)
        DESERIALIZE_COMPONENT("text", TextComponent)
        DESERIALIZE_COMPONENT("transform", TransformComponent)
        }
    }

    Entity EntityScene::createEntity() {
        return {create(), *this};
    }

    Entity EntityScene::createEntity(const std::string &name) {
        return {create(name), *this};
    }

    void EntityScene::destroyEntity(const Entity &entity) {
        destroy(entity.getHandle());
    }

    Entity EntityScene::getEntity(const std::string &name) {
        return {getByName(name), *this};
    }

    std::unique_ptr<Resource> EntityScene::clone() {
        return std::make_unique<EntityScene>(*this);
    }

    std::type_index EntityScene::getTypeIndex() {
        return typeid(EntityScene);
    }
}