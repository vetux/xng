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

#ifndef XENGINE_RENDERSYSTEM_HPP
#define XENGINE_RENDERSYSTEM_HPP

#include <string>

#include "xng/ecs/system.hpp"
#include "xng/ecs/components/rendering/meshcomponent.hpp"

#include "xng/graphics/3d/renderpassscheduler.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/renderconfiguration.hpp"

#include "xng/util/time.hpp"

namespace xng {
    class XENGINE_EXPORT RenderSystem final : public System {
    public:
        explicit RenderSystem(std::shared_ptr<RenderGraphRuntime> renderGraphRuntime);

        ~RenderSystem() override;

        void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override;

        std::string getName() override { return "RenderSystem"; }

    private:
        std::shared_ptr<RenderGraphRuntime> runtime;
        RenderPassScheduler scheduler;

        std::shared_ptr<SharedResourceRegistry> registry;
        std::shared_ptr<RenderConfiguration> config;

        RenderGraphHandle graph;
    };
}

#endif //XENGINE_RENDERSYSTEM_HPP
