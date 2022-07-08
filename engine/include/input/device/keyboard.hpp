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

#ifndef XENGINE_KEYBOARD_HPP
#define XENGINE_KEYBOARD_HPP

#include "input/device/buttonstate.hpp"

namespace xng {
    enum KeyboardKey {
        KEY_RETURN,
        KEY_ESCAPE,
        KEY_BACKSPACE,
        KEY_TAB,
        KEY_SPACE,

        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,

        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,

        KEY_CAPSLOCK,

        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,

        KEY_PRINTSCREEN,
        KEY_SCROLLLOCK,
        KEY_PAUSE,
        KEY_INSERT,
        KEY_HOME,
        KEY_PAGEUP,
        KEY_PAGEDOWN,
        KEY_DELETE,
        KEY_END,

        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,

        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_MINUS,
        KEY_KP_PLUS,
        KEY_KP_ENTER,
        KEY_KP_0,
        KEY_KP_1,
        KEY_KP_2,
        KEY_KP_3,
        KEY_KP_4,
        KEY_KP_5,
        KEY_KP_6,
        KEY_KP_7,
        KEY_KP_8,
        KEY_KP_9,
        KEY_KP_PERIOD,

        KEY_LCTRL,
        KEY_LSHIFT,
        KEY_LALT,
        KEY_RCTRL,
        KEY_RSHIFT,
        KEY_RALT,

        KEY_UNDEFINED
    };

    class XENGINE_EXPORT Keyboard {
    public:
        bool getKey(KeyboardKey key) const {
            auto it = keys.find(key);
            if (it != keys.end())
                return it->second != RELEASED;
            else
                return false;
        }

        bool getKeyDown(KeyboardKey key) const {
            auto it = keys.find(key);
            if (it != keys.end())
                return it->second == PRESSED;
            else
                return false;
        }

        std::map<KeyboardKey, ButtonState> keys;
    };
}

#endif //XENGINE_KEYBOARD_HPP
