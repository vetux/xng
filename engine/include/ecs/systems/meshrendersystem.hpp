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

#ifndef XENGINE_MESHRENDERSYSTEM_HPP
#define XENGINE_MESHRENDERSYSTEM_HPP

#include <map>
#include <string>

#include "ecs/system.hpp"
#include "ecs/components/meshrendercomponent.hpp"
#include "ecs/components/skyboxcomponent.hpp"

#include "render/scenerenderer.hpp"

namespace xng {
    class XENGINE_EXPORT MeshRenderSystem : public System {
    public:
        MeshRenderSystem(SceneRenderer &pipeline);

        ~MeshRenderSystem() override;

        void start(EntityScene &entityManager, EventBus &eventBus) override;

        void stop(EntityScene &entityManager, EventBus &eventBus) override;

        void update(DeltaTime deltaTime, EntityScene &entityManager, EventBus &eventBus) override;

        std::string getName() override { return "MeshRenderSystem"; }

        SceneRenderer &getPipeline();

        size_t getPolyCount() const { return polyCount; }

    private:
        SceneRenderer &pipeline;
        size_t polyCount{};
    };
}

#endif //XENGINE_MESHRENDERSYSTEM_HPP
