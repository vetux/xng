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

#ifndef XENGINE_EVENTSYSTEM_HPP
#define XENGINE_EVENTSYSTEM_HPP

#include "ecs/system.hpp"

#include "event/event.hpp"
#include "event/eventbus.hpp"
#include "event/events/entityevent.hpp"
#include "event/events/inputevent.hpp"
#include "event/events/windowevent.hpp"

#include "display/window.hpp"

namespace xng {
    class XENGINE_EXPORT EventSystem : public System,
                                       public InputListener,
                                       public WindowListener,
                                       public EntityScene::Listener {
    public:
        explicit EventSystem(Window &wnd, EventBus &eventBus);

        ~EventSystem() override;

        void start(EntityScene &scene) override;

        void stop(EntityScene &scene) override;

        void update(DeltaTime deltaTime, EntityScene &scene) override;

        void onKeyDown(KeyboardKey key) override;

        void onKeyUp(KeyboardKey key) override;

        void onMouseMove(double xPos, double yPos) override;

        void onMouseWheelScroll(double amount) override;

        void onMouseKeyDown(MouseButton key) override;

        void onMouseKeyUp(MouseButton key) override;

        void onTextInput(const std::string &text) override;

        void onGamepadConnected(int id) override;

        void onGamepadDisconnected(int id) override;

        void onGamepadAxis(int id, GamePadAxis axis, double amount) override;

        void onGamepadButtonDown(int id, GamePadButton button) override;

        void onGamepadButtonUp(int id, GamePadButton button) override;

        void onWindowClose() override;

        void onWindowResize(Vec2i size) override;

    private:
        Window &wnd;
        EventBus &eventBus;
    };
}

#endif //XENGINE_EVENTSYSTEM_HPP
