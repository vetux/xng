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

#ifndef XENGINE_AUDIOSYSTEM_HPP
#define XENGINE_AUDIOSYSTEM_HPP


#include "xng/ecs/system.hpp"


#include "xng/ecs/components/audiosourcecomponent.hpp"
#include "xng/ecs/components/transformcomponent.hpp"

#include "xng/audio/audiodevice.hpp"
#include "xng/resource/resourceregistry.hpp"

namespace xng {
    class XENGINE_EXPORT AudioSystem : public System, public EntityScene::Listener {
    public:
        explicit AudioSystem(AudioDevice &device, ResourceRegistry &repo);

        ~AudioSystem() override = default;

        AudioSystem(const AudioSystem &other) = delete;

        AudioSystem & operator=(const AudioSystem &other) = delete;

        void start(EntityScene &scene, EventBus &eventBus) override;

        void stop(EntityScene &scene, EventBus &eventBus) override;

        void update(DeltaTime deltaTime, EntityScene &entityManager, EventBus &eventBus) override;

        void onComponentCreate(const EntityHandle &entity, const Component &component) override;

        void onComponentDestroy(const EntityHandle &entity, const Component &component) override;

        void onComponentUpdate(const EntityHandle &entity,
                               const Component &oldComponent,
                               const Component &newComponent) override;

        std::string getName() override { return "AudioSystem"; }

        void onEntityDestroy(const EntityHandle &entity) override;

    private:
        AudioDevice &device;
        ResourceRegistry &repo;

        std::unique_ptr<AudioContext> context;

        std::map<EntityHandle, std::unique_ptr<AudioSource>> sources;
        std::map<EntityHandle, std::unique_ptr<AudioBuffer>> buffers;
        std::set<EntityHandle> playingSources;
    };
}

#endif //XENGINE_AUDIOSYSTEM_HPP
