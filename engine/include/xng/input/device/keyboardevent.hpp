/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_KEYBOARDEVENT_HPP
#define XENGINE_KEYBOARDEVENT_HPP

#include "xng/event/event.hpp"

#include "xng/input/device/keyboard.hpp"

namespace xng {
    struct KeyboardEvent {
        EVENT_TYPENAME(KeyboardEvent)

        enum Type {
            KEYBOARD_CONNECTED,
            KEYBOARD_DISCONNECTED,
            KEYBOARD_KEY_DOWN,
            KEYBOARD_KEY_UP,
            KEYBOARD_CHARACTER_INPUT,
        } type{};

        int id{};
        KeyboardKey key{};
        char32_t value{};

        KeyboardEvent() = default;
    };
}
#endif //XENGINE_KEYBOARDEVENT_HPP
