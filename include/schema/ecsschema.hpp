/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_ECSSCHEMA_HPP
#define MANA_ECSSCHEMA_HPP

#include "io/message.hpp"

#include "ecs/components/audio/audiosourcecomponent.hpp"
#include "ecs/components/audio/audiolistenercomponent.hpp"

#include "ecs/components.hpp"
#include "ecs/entitymanager.hpp"

namespace engine {
    //TODO: Design better schema generation method
    MANA_EXPORT CameraType &operator<<(CameraType &value, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, CameraType type);

    MANA_EXPORT LightType &operator<<(LightType &value, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, LightType type);

    MANA_EXPORT AssetPath &operator<<(AssetPath &value, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const AssetPath &path);

    MANA_EXPORT CameraComponent &operator<<(CameraComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const CameraComponent &component);

    MANA_EXPORT TransformComponent &operator<<(TransformComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const TransformComponent &component);

    MANA_EXPORT MeshRenderComponent &operator<<(MeshRenderComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const MeshRenderComponent &component);

    MANA_EXPORT SkyboxComponent &operator<<(SkyboxComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const SkyboxComponent &component);

    MANA_EXPORT LightComponent &operator<<(LightComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const LightComponent &component);

    MANA_EXPORT MonoScriptComponent &operator<<(MonoScriptComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const MonoScriptComponent &component);

    MANA_EXPORT MonoSyncComponent &operator<<(MonoSyncComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const MonoSyncComponent &component);

    MANA_EXPORT AudioSourceComponent &operator<<(AudioSourceComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const AudioSourceComponent &component);

    MANA_EXPORT AudioListenerComponent &operator<<(AudioListenerComponent &component, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const AudioListenerComponent &component);

    MANA_EXPORT EntityManager &operator<<(EntityManager &manager, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, EntityManager &manager);
}

#endif //MANA_ECSSCHEMA_HPP
