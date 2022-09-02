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

#include "ecs/systems/eventsystem.hpp"

namespace xng {
    EventSystem::EventSystem(Window &wnd, EventBus &eventBus) : wnd(wnd), eventBus(eventBus) {
        wnd.addListener(*this);
        wnd.getInput().addListener(*this);
    }

    EventSystem::~EventSystem() {
        wnd.removeListener(*this);
        wnd.getInput().removeListener(*this);
    }

    void EventSystem::start(EntityScene &scene) {
        scene.addListener(*this);
    }

    void EventSystem::stop(EntityScene &scene) {
        scene.removeListener(*this);
    }

    void EventSystem::update(DeltaTime deltaTime, EntityScene &scene) {}

    void EventSystem::onKeyDown(KeyboardKey key) {
        eventBus.invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_KEY_DOWN, .key = key}));
    }

    void EventSystem::onKeyUp(KeyboardKey key) {
        eventBus.invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_KEY_UP, .key = key}));
    }

    void EventSystem::onMouseMove(double xPos, double yPos) {
        eventBus.invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_MOVE, .xPos = xPos, .yPos = yPos}));
    }

    void EventSystem::onMouseWheelScroll(double amount) {
        eventBus.invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_WHEEL_SCROLL, .amount = amount}));
    }

    void EventSystem::onMouseKeyDown(MouseButton key) {
        eventBus.invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_KEY_DOWN, .key = key}));
    }

    void EventSystem::onMouseKeyUp(MouseButton key) {
        eventBus.invoke(InputEvent(MouseEventData{.type = MouseEventData::MOUSE_KEY_UP, .key = key}));
    }

    void EventSystem::onTextInput(const std::string &text) {
        eventBus.invoke(InputEvent(KeyboardEventData{.type = KeyboardEventData::KEYBOARD_TEXT_INPUT, .text=text}));
    }

    void EventSystem::onGamepadConnected(int id) {
        eventBus.invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_CONNECTED, .id=id}));
    }

    void EventSystem::onGamepadDisconnected(int id) {
        eventBus.invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_DISCONNECTED, .id=id}));
    }

    void EventSystem::onGamepadAxis(int id, GamePadAxis axis, double amount) {
        eventBus.invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_AXIS,
                .id=id, .axis=axis, .amount=amount}));
    }

    void EventSystem::onGamepadButtonDown(int id, GamePadButton button) {
        eventBus.invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_BUTTON_DOWN, .id=id, .button = button}));
    }

    void EventSystem::onGamepadButtonUp(int id, GamePadButton button) {
        eventBus.invoke(InputEvent(GamePadEventData{.type = GamePadEventData::GAMEPAD_BUTTON_UP, .id=id, .button = button}));
    }

    void EventSystem::onWindowClose() {
        eventBus.invoke(WindowEvent(WindowEvent::WINDOW_CLOSE, {}, {}));
    }

    void EventSystem::onWindowResize(Vec2i size) {
        eventBus.invoke(WindowEvent(WindowEvent::WINDOW_RESIZE, size, {}));
    }
}