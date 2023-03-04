/**
 *  This file is part of xEngine, a C++ game engine library.
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

#include "xng/ecs/systems/eventsystem.hpp"

namespace xng {
    EventSystem::EventSystem(Window &wnd) : wnd(wnd) {
        wnd.addListener(*this);
        wnd.getInput().addListener(*this);
    }

    EventSystem::~EventSystem() {
        wnd.removeListener(*this);
        wnd.getInput().removeListener(*this);
    }

    void EventSystem::start(EntityScene &scene, EventBus &eventBus) {
        bus = &eventBus;
        scene.addListener(*this);
    }

    void EventSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
        bus = nullptr;
    }

    void EventSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {}

    void EventSystem::onKeyDown(KeyboardKey key) {
        if (bus != nullptr)
            bus->invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_KEY_DOWN, .key = key}));
    }

    void EventSystem::onKeyUp(KeyboardKey key) {
        if (bus != nullptr)
            bus->invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_KEY_UP, .key = key}));
    }

    void EventSystem::onMouseMove(double xPos, double yPos) {
        if (bus != nullptr)
            bus->invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_MOVE, .xPos = xPos, .yPos = yPos}));
    }

    void EventSystem::onMouseWheelScroll(double amount) {
        if (bus != nullptr)
            bus->invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_WHEEL_SCROLL, .amount = amount}));
    }

    void EventSystem::onMouseKeyDown(MouseButton key) {
        if (bus != nullptr)
            bus->invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_KEY_DOWN, .key = key}));
    }

    void EventSystem::onMouseKeyUp(MouseButton key) {
        if (bus != nullptr)
            bus->invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_KEY_UP, .key = key}));
    }

    void EventSystem::onCharacterInput(char32_t val) {
        if (bus != nullptr)
            bus->invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_CHAR_INPUT, .character=val}));
    }

    void EventSystem::onGamepadConnected(int id) {
        if (bus != nullptr)
            bus->invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_CONNECTED, .id=id}));
    }

    void EventSystem::onGamepadDisconnected(int id) {
        if (bus != nullptr)
            bus->invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_DISCONNECTED, .id=id}));
    }

    void EventSystem::onGamepadAxis(int id, GamePadAxis axis, double amount) {
        if (bus != nullptr)
            bus->invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_AXIS,
                    .id=id, .axis=axis, .amount=amount}));
    }

    void EventSystem::onGamepadButtonDown(int id, GamePadButton button) {
        if (bus != nullptr)
            bus->invoke(InputEvent(
                    GamePadEventData{.type = GamePadEventData::GAMEPAD_BUTTON_DOWN, .id=id, .button = button}));
    }

    void EventSystem::onGamepadButtonUp(int id, GamePadButton button) {
        if (bus != nullptr)
            bus->invoke(InputEvent(
                    GamePadEventData{.type = GamePadEventData::GAMEPAD_BUTTON_UP, .id=id, .button = button}));
    }

    void EventSystem::onWindowClose() {
        if (bus != nullptr)
            bus->invoke(WindowEvent(WindowEvent::WINDOW_CLOSE, {}, {}));
    }

    void EventSystem::onWindowResize(Vec2i size) {
        if (bus != nullptr)
            bus->invoke(WindowEvent(WindowEvent::WINDOW_RESIZE, size, {}));
    }
}