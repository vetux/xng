/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/ecs/system.hpp"
#include "xng/ecs/components/meshcomponent.hpp"
#include "xng/ecs/components/skyboxcomponent.hpp"

#include "xng/render/scenerenderer.hpp"
#include "xng/util/time.hpp"

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

    private:
        SceneRenderer &renderer;
    };
}

#endif //XENGINE_MESHRENDERSYSTEM_HPP
