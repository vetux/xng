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

#ifndef XENGINE_ECSSCHEMA_HPP
#define XENGINE_ECSSCHEMA_HPP

#include "io/message.hpp"

#include "ecs/components/audio/audiosourcecomponent.hpp"
#include "ecs/components/audio/audiolistenercomponent.hpp"

#include "ecs/components.hpp"
#include "ecs/entitymanager.hpp"

namespace xengine {
    //TODO: Design better schema generation method
    XENGINE_EXPORT CameraType &operator<<(CameraType &value, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, CameraType type);

    XENGINE_EXPORT LightType &operator<<(LightType &value, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, LightType type);

    XENGINE_EXPORT Uri &operator<<(Uri &value, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const Uri &path);

    XENGINE_EXPORT CameraComponent &operator<<(CameraComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const CameraComponent &component);

    XENGINE_EXPORT TransformComponent &operator<<(TransformComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const TransformComponent &component);

    XENGINE_EXPORT MeshRenderComponent &operator<<(MeshRenderComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const MeshRenderComponent &component);

    XENGINE_EXPORT SkyboxComponent &operator<<(SkyboxComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const SkyboxComponent &component);

    XENGINE_EXPORT LightComponent &operator<<(LightComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const LightComponent &component);

    XENGINE_EXPORT MonoScriptComponent &operator<<(MonoScriptComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const MonoScriptComponent &component);

    XENGINE_EXPORT MonoSyncComponent &operator<<(MonoSyncComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const MonoSyncComponent &component);

    XENGINE_EXPORT AudioSourceComponent &operator<<(AudioSourceComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const AudioSourceComponent &component);

    XENGINE_EXPORT AudioListenerComponent &operator<<(AudioListenerComponent &component, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, const AudioListenerComponent &component);

    XENGINE_EXPORT EntityManager &operator<<(EntityManager &manager, const Message &message);

    XENGINE_EXPORT Message &operator<<(Message &message, EntityManager &manager);
}

#endif //XENGINE_ECSSCHEMA_HPP
