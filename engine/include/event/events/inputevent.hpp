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

#ifndef XENGINE_INPUTEVENT_HPP
#define XENGINE_INPUTEVENT_HPP

#include <variant>

#include "event/event.hpp"

#include "input/input.hpp"

namespace xng {
    struct GamePadEventData {
        enum Type {
            GAMEPAD_CONNECTED,
            GAMEPAD_DISCONNECTED,
            GAMEPAD_AXIS,
            GAMEPAD_BUTTON_DOWN,
            GAMEPAD_BUTTON_UP
        } type;

        int id;
        GamePadButton button;
        GamePadAxis axis;
        double amount;
    };

    struct KeyboardEventData {
        enum Type {
            KEYBOARD_CHAR_INPUT,
            KEYBOARD_KEY_DOWN,
            KEYBOARD_KEY_UP
        } type;

        KeyboardKey key;
        char32_t character;
    };

    struct MouseEventData {
        enum Type {
            MOUSE_MOVE,
            MOUSE_WHEEL_SCROLL,
            MOUSE_KEY_DOWN,
            MOUSE_KEY_UP
        } type;

        double xPos;
        double yPos;
        double amount;
        MouseButton key;
    };

    struct InputEvent : public Event {
        std::type_index getEventType() const override {
            return std::type_index(typeid(InputEvent));
        }

        InputEvent() = default;

        explicit InputEvent(GamePadEventData event) : data(event), deviceType(DEVICE_GAMEPAD) {}

        explicit InputEvent(KeyboardEventData event) : data(event), deviceType(DEVICE_KEYBOARD) {}

        explicit InputEvent(MouseEventData event) : data(event), deviceType(DEVICE_MOUSE) {}

        enum DeviceType {
            DEVICE_GAMEPAD,
            DEVICE_KEYBOARD,
            DEVICE_MOUSE
        } deviceType;

        std::variant<GamePadEventData, KeyboardEventData, MouseEventData> data;
    };
}

#endif //XENGINE_INPUTEVENT_HPP
