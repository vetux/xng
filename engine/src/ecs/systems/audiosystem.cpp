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

#include "ecs/systems/audiosystem.hpp"

#include "ecs/components/audio/audiolistenercomponent.hpp"
#include "ecs/components/audio/audiosourcecomponent.hpp"

#include "resource/resourcehandle.hpp"

#define AUDIO_POS_SCALE 1

namespace xng {
    AudioSystem::AudioSystem(AudioDevice &device, ResourceRegistry &repo)
            : device(device), repo(repo) {
        context = device.createContext();
        context->makeCurrent();
    }

    void AudioSystem::start(EntityContainer &entityManager) {
        entityManager.getComponentContainer().getPool<AudioSourceComponent>().addListener(this);
    }

    void AudioSystem::stop(EntityContainer &entityManager) {
        entityManager.getComponentContainer().getPool<AudioSourceComponent>().removeListener(this);
    }

    void AudioSystem::update(DeltaTime deltaTime, EntityContainer &entityManager) {
        auto &componentManager = entityManager.getComponentContainer();

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

    void AudioSystem::onComponentCreate(const EntityHandle &entity, const AudioSourceComponent &component) {
        if (component.audio) {
            auto& buffer = component.audio.get();
            buffers[entity] = context->createBuffer();
            buffers[entity]->upload(buffer.buffer,
                                    buffer.format,
                                    buffer.frequency);

            sources[entity] = context->createSource();
            sources[entity]->setBuffer(*buffers[entity]);
        }
    }

    void AudioSystem::onComponentDestroy(const EntityHandle &entity, const AudioSourceComponent &component) {
        sources.erase(entity);
        buffers.erase(entity);
    }

    void AudioSystem::onComponentUpdate(const EntityHandle &entity,
                                        const AudioSourceComponent &oldValue,
                                        const AudioSourceComponent &newValue) {
        if (oldValue != newValue) {
            auto& buffer = newValue.audio.get();
            sources[entity]->stop();
            // Unbind buffer before uploading
            sources.at(entity)->clearBuffer();
            buffers.at(entity)->upload(buffer.buffer, buffer.format, buffer.frequency);
            sources.at(entity)->setBuffer(*buffers.at(entity));
        }
    }
}