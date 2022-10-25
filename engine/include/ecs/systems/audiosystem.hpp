/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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


#include "ecs/system.hpp"


#include "ecs/components/audiosourcecomponent.hpp"
#include "ecs/components/transformcomponent.hpp"

#include "audio/audiodevice.hpp"
#include "resource/resourceregistry.hpp"

namespace xng {
    class XENGINE_EXPORT AudioSystem : public System, public EntityScene::Listener {
    public:
        explicit AudioSystem(AudioDevice &device, ResourceRegistry &repo);

        ~AudioSystem() override = default;

        AudioSystem(const AudioSystem &other) = delete;

        AudioSystem & operator=(const AudioSystem &other) = delete;

        void start(EntityScene &scene) override;

        void stop(EntityScene &scene) override;

        void update(DeltaTime deltaTime, EntityScene &entityManager) override;

        void onComponentCreate(const EntityHandle &entity, const std::any &component) override;

        void onComponentDestroy(const EntityHandle &entity, const std::any &component) override;

        void onComponentUpdate(const EntityHandle &entity, const std::any &oldComponent,
                               const std::any &newComponent) override;

        std::string getName() override { return "AudioSystem"; }

    private:
        AudioDevice &device;
        ResourceRegistry &repo;

        std::unique_ptr<AudioContext> context;

        std::map<EntityHandle, std::unique_ptr<AudioSource>> sources;
        std::map<EntityHandle, std::unique_ptr<AudioBuffer>> buffers;
    };
}

#endif //XENGINE_AUDIOSYSTEM_HPP
