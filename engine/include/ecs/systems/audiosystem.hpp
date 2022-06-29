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

#ifndef XENGINE_AUDIOSYSTEM_HPP
#define XENGINE_AUDIOSYSTEM_HPP


#include "ecs/system.hpp"


#include "ecs/components/audio/audiosourcecomponent.hpp"
#include "ecs/components/transformcomponent.hpp"

#include "audio/audiodevice.hpp"
#include "resource/resourceregistry.hpp"

namespace xng {
    class XENGINE_EXPORT AudioSystem : public System, ComponentPool<AudioSourceComponent>::Listener {
    public:
        explicit AudioSystem(AudioDevice &device, ResourceRegistry &repo);

        ~AudioSystem() override = default;

        void start(EntityManager &entityManager) override;

        void stop(EntityManager &entityManager) override;

        void update(float deltaTime, EntityManager &entityManager) override;

    private:
        void onComponentCreate(const Entity &entity, const AudioSourceComponent &component) override;

        void onComponentDestroy(const Entity &entity, const AudioSourceComponent &component) override;

        void onComponentUpdate(const Entity &entity, const AudioSourceComponent &oldValue,
                               const AudioSourceComponent &newValue) override;

    private:
        AudioDevice &device;
        ResourceRegistry &repo;

        std::unique_ptr<AudioContext> context;

        std::map<Entity, std::unique_ptr<AudioSource>> sources;
        std::map<Entity, std::unique_ptr<AudioBuffer>> buffers;
    };
}

#endif //XENGINE_AUDIOSYSTEM_HPP
