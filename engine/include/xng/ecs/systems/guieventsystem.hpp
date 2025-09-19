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

#ifndef XENGINE_GUIEVENTSYSTEM_HPP
#define XENGINE_GUIEVENTSYSTEM_HPP

#include "xng/ecs/system.hpp"
#include "xng/event/eventbus.hpp"
#include "xng/display/window.hpp"
#include "xng/util/time.hpp"

namespace xng {
    /**
     * Depends on the layout system.
     */
    class XENGINE_EXPORT GuiEventSystem final : public System {
    public:
        explicit GuiEventSystem(std::shared_ptr<Window> window);

        ~GuiEventSystem() override = default;

        void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override;

        void start(EntityScene &scene, EventBus &eventBus) override;

        void stop(EntityScene &scene, EventBus &eventBus) override;

        std::string getName() override { return "GuiEventSystem"; }

    private:
        std::shared_ptr<Window> window;

        std::set<EntityHandle> hoverButtons;
        std::set<EntityHandle> clickButtons;
    };
}

#endif //XENGINE_GUIEVENTSYSTEM_HPP
