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

#ifndef XENGINE_GUIEVENTSYSTEM_HPP
#define XENGINE_GUIEVENTSYSTEM_HPP

#include "ecs/system.hpp"
#include "event/eventbus.hpp"
#include "display/window.hpp"

namespace xng {
    class XENGINE_EXPORT GuiEventSystem : public System {
    public:
        GuiEventSystem(Window &window, EventBus &eventBus);

        ~GuiEventSystem() override = default;

        void update(DeltaTime deltaTime, EntityScene &scene) override;

        void start(EntityScene &scene) override;

        void stop(EntityScene &scene) override;

        std::string getName() override { return "GuiEventSystem"; }

    private:
        Window &window;
        EventBus &eventBus;

        std::set<EntityHandle> hoverButtons;
        std::set<EntityHandle> clickButtons;
    };
}

#endif //XENGINE_GUIEVENTSYSTEM_HPP
