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

#include "ecs/systems/audiosystem.hpp"

#include "ecs/components/audio/audiolistenercomponent.hpp"
#include "ecs/components/audio/audiosourcecomponent.hpp"

#define AUDIO_POS_SCALE 1

namespace engine {
    AudioSystem::AudioSystem(AudioDevice &device, AssetManager &assetManager)
            : device(device), assetManager(assetManager) {
        context = device.createContext();
        context->makeCurrent();
    }

    void AudioSystem::start(EntityManager &entityManager) {
        entityManager.getComponentManager().getPool<AudioSourceComponent>().addListener(this);
    }

    void AudioSystem::stop(EntityManager &entityManager) {
        entityManager.getComponentManager().getPool<AudioSourceComponent>().removeListener(this);
    }

    void AudioSystem::update(float deltaTime, EntityManager &entityManager) {
        auto &componentManager = entityManager.getComponentManager();

        for (auto &pair: componentManager.getPool<AudioListenerComponent>()) {
            auto &transform = componentManager.lookup<TransformComponent>(pair.first);
            auto &listener = context->getListener();
            listener.setPosition(transform.transform.getPosition() * AUDIO_POS_SCALE);
            listener.setOrientation({transform.transform.getPosition()},
                                    transform.transform.getRotation().getEulerAngles());
            listener.setVelocity(pair.second.velocity);
        }

        for (auto &pair: componentManager.getPool<AudioSourceComponent>()) {
            auto &comp = pair.second;
            auto &transform = componentManager.lookup<TransformComponent>(pair.first);
            auto &source = sources.at(pair.first);

            source->setPosition(transform.transform.getPosition() * AUDIO_POS_SCALE);
            source->setLooping(comp.loop);
            source->setVelocity(comp.velocity);

            //TODO: Source Volume and Distance

            if (comp.play && !comp.playing) {
                source->play();
                comp.playing = true;
            } else if (!comp.play && comp.playing) {
                source->pause();
                comp.playing = false;
            }
        }
    }

    void AudioSystem::onComponentCreate(const Entity &entity, const AudioSourceComponent &component) {
        if (!component.audioPath.empty()) {
            auto handle = AssetHandle<Audio>(component.audioPath, assetManager);

            buffers[entity] = context->createBuffer();
            buffers[entity]->upload(handle.get().buffer, handle.get().format, handle.get().frequency);

            sources[entity] = context->createSource();
            sources[entity]->setBuffer(*buffers[entity]);
        }
    }

    void AudioSystem::onComponentDestroy(const Entity &entity, const AudioSourceComponent &component) {
        sources.erase(entity);
        buffers.erase(entity);
    }

    void AudioSystem::onComponentUpdate(const Entity &entity,
                                        const AudioSourceComponent &oldValue,
                                        const AudioSourceComponent &newValue) {
        if (!(oldValue.audioPath == newValue.audioPath)) {
            sources[entity]->stop();
            auto handle = AssetHandle<Audio>(newValue.audioPath, assetManager);
            buffers.at(entity)->upload(handle.get().buffer, handle.get().format, handle.get().frequency);
            sources.at(entity)->setBuffer(*buffers.at(entity));
        }
    }
}