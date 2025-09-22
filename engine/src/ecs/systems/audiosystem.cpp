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

#include "xng/ecs/systems/audiosystem.hpp"

#include "xng/ecs/components/audio/audiolistenercomponent.hpp"
#include "xng/ecs/components/audio/audiosourcecomponent.hpp"

#include "xng/resource/resourcehandle.hpp"
#include "xng/util/time.hpp"

#define AUDIO_POS_SCALE 1

namespace xng {
    AudioSystem::AudioSystem(std::shared_ptr<AudioDevice> audioDevice)
        : device(std::move(audioDevice)) {
        context = device->createContext();
        context->makeCurrent();
    }

    void AudioSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
    }

    void AudioSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
        sources.clear();
        buffers.clear();
    }

    void AudioSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (const auto &pair: scene.getPool<AudioListenerComponent>()) {
            auto &transform = scene.getComponent<TransformComponent>(pair.entity);
            auto &listener = context->getListener();
            listener.setPosition(transform.transform.getPosition() * AUDIO_POS_SCALE);
            listener.setOrientation({transform.transform.getPosition()},
                                    transform.transform.getRotation().getEulerAngles());
            listener.setVelocity(pair.component.velocity);
        }

        for (auto &pair: scene.getPool<AudioSourceComponent>()) {
            auto comp = pair.component;
            auto &transform = scene.getComponent<TransformComponent>(pair.entity);
            auto &source = sources.at(pair.entity);

            if (!pair.component.audio.isLoaded()) {
                continue;
            }

            if (buffers.find(pair.entity) == buffers.end()) {
                auto &buffer = pair.component.audio.get();
                buffers[pair.entity] = context->createBuffer();
                buffers[pair.entity]->upload(buffer.buffer,
                                            buffer.format,
                                            buffer.frequency);

                sources[pair.entity] = context->createSource();
                sources[pair.entity]->setBuffer(*buffers[pair.entity]);
                playingSources.erase(pair.entity);
            }

            source->setPosition(transform.transform.getPosition() * AUDIO_POS_SCALE);
            source->setLooping(comp.loop);
            source->setVelocity(comp.velocity);

            //TODO: Source Volume and Distance

            if (comp.play && playingSources.find(pair.entity) == playingSources.end()) {
                source->play();
                playingSources.insert(pair.entity);
            } else if (!comp.play && playingSources.find(pair.entity) != playingSources.end()) {
                source->pause();
                playingSources.erase(pair.entity);
            }

            scene.updateComponent(pair.entity, comp);
        }
    }

    void AudioSystem::onEntityDestroy(const EntityHandle &entity) {
        sources.erase(entity);
        buffers.erase(entity);
        playingSources.erase(entity);
    }

    void AudioSystem::onComponentCreate(const EntityHandle &entity, const Component &value) {
    }

    void AudioSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == AudioSourceComponent::typeName) {
            sources.erase(entity);
            buffers.erase(entity);
            playingSources.erase(entity);
        }
    }

    void AudioSystem::onComponentUpdate(const EntityHandle &entity,
                                        const Component &oldComponent,
                                        const Component &newComponent) {
        if (oldComponent.getTypeName() == AudioSourceComponent::typeName) {
            const auto &oldValue = down_cast<const AudioSourceComponent &>(oldComponent);
            const auto &newValue = down_cast<const AudioSourceComponent &>(newComponent);
            if (oldValue != newValue) {
                if (oldValue.audio != newValue.audio) {
                    sources[entity]->stop();
                    // Unbind buffer before uploading
                    sources.at(entity)->clearBuffer();
                }
            }
        }
    }
}
