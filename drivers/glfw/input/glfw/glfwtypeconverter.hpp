/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_GLFWTYPECONVERTER_HPP
#define XENGINE_GLFWTYPECONVERTER_HPP

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "input/device/gamepad.hpp"
#include "input/device/keyboard.hpp"
#include "input/device/mouse.hpp"

namespace xng {
    class GLFWTypeConverter {
    public:
        static int convertMouseKey(MouseButton key) {
            switch (key) {
                case LEFT:
                    return GLFW_MOUSE_BUTTON_LEFT;
                case MIDDLE:
                    return GLFW_MOUSE_BUTTON_MIDDLE;
                case RIGHT:
                    return GLFW_MOUSE_BUTTON_RIGHT;
                case OPTIONAL_1:
                    return GLFW_MOUSE_BUTTON_1;
                case OPTIONAL_2:
                    return GLFW_MOUSE_BUTTON_2;
                case OPTIONAL_3:
                    return GLFW_MOUSE_BUTTON_3;
                case OPTIONAL_4:
                    return GLFW_MOUSE_BUTTON_4;
                case OPTIONAL_5:
                    return GLFW_MOUSE_BUTTON_5;
                default:
                    throw std::runtime_error("Cannot convert key code " + std::to_string(key));
            }
        }

        static MouseButton convertMouseKey(int key) {
            switch (key) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    return LEFT;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    return MIDDLE;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    return RIGHT;
                case GLFW_MOUSE_BUTTON_4:
                    return OPTIONAL_1;
                case GLFW_MOUSE_BUTTON_5:
                    return OPTIONAL_2;
                case GLFW_MOUSE_BUTTON_6:
                    return OPTIONAL_3;
                case GLFW_MOUSE_BUTTON_7:
                    return OPTIONAL_4;
                case GLFW_MOUSE_BUTTON_8:
                    return OPTIONAL_5;
                default:
                    return MOUSE_BUTTON_UNDEFINED;
            }
        }

        static int convertGamepadButton(GamePadButton key) {
            switch (key) {
                case A:
                    return GLFW_GAMEPAD_BUTTON_A;
                case B:
                    return GLFW_GAMEPAD_BUTTON_B;
                case X:
                    return GLFW_GAMEPAD_BUTTON_X;
                case Y:
                    return GLFW_GAMEPAD_BUTTON_Y;
                case BUMPER_LEFT:
                    return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
                case BUMPER_RIGHT:
                    return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
                case BACK:
                    return GLFW_GAMEPAD_BUTTON_BACK;
                case START:
                    return GLFW_GAMEPAD_BUTTON_START;
                case GUIDE:
                    return GLFW_GAMEPAD_BUTTON_GUIDE;
                case LEFT_STICK:
                    return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
                case RIGHT_STICK:
                    return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
                case DPAD_UP:
                    return GLFW_GAMEPAD_BUTTON_DPAD_UP;
                case DPAD_RIGHT:
                    return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
                case DPADP_LEFT:
                    return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
                default:
                    throw std::runtime_error("Cannot convert gamepad key code " + std::to_string(key));
            }
        }

        static GamePadButton convertGamepadKey(int key) {
            switch (key) {
                case GLFW_GAMEPAD_BUTTON_A:
                    return A;
                case GLFW_GAMEPAD_BUTTON_B:
                    return B;
                case GLFW_GAMEPAD_BUTTON_X:
                    return X;
                case GLFW_GAMEPAD_BUTTON_Y:
                    return Y;
                case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
                    return BUMPER_LEFT;
                case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
                    return BUMPER_RIGHT;
                case GLFW_GAMEPAD_BUTTON_BACK:
                    return BACK;
                case GLFW_GAMEPAD_BUTTON_START:
                    return START;
                case GLFW_GAMEPAD_BUTTON_GUIDE:
                    return GUIDE;
                case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:
                    return LEFT_STICK;
                case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:
                    return RIGHT_STICK;
                case GLFW_GAMEPAD_BUTTON_DPAD_UP:
                    return DPAD_UP;
                case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
                    return DPAD_RIGHT;
                case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
                    return DPADP_LEFT;
                default:
                    return GAMEPAD_BUTTON_UNDEFINED;
            }
        }

        static int convertGamepadAxis(GamePadAxis axis) {
            switch (axis) {
                case LEFT_X:
                    return GLFW_GAMEPAD_AXIS_LEFT_X;
                case LEFT_Y:
                    return GLFW_GAMEPAD_AXIS_LEFT_Y;
                case RIGHT_X:
                    return GLFW_GAMEPAD_AXIS_RIGHT_X;
                case RIGHT_Y:
                    return GLFW_GAMEPAD_AXIS_RIGHT_Y;
                case TRIGGER_LEFT:
                    return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
                case TRIGGER_RIGHT:
                    return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
                default:
                    throw std::runtime_error("Cannot convert axis code " + std::to_string(axis));
            }
        }

        static GamePadAxis convertGamepadAxis(int axis) {
            switch (axis) {
                case GLFW_GAMEPAD_AXIS_LEFT_X:
                    return LEFT_X;
                case GLFW_GAMEPAD_AXIS_LEFT_Y:
                    return LEFT_Y;
                case GLFW_GAMEPAD_AXIS_RIGHT_X:
                    return RIGHT_X;
                case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                    return RIGHT_Y;
                case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
                    return TRIGGER_LEFT;
                case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
                    return TRIGGER_RIGHT;
                default:
                    return AXIS_UNDEFINED;
            }
        }

        static int convertKey(KeyboardKey key) {
            switch (key) {
                case KEY_RETURN:
                    return GLFW_KEY_ENTER;
                case KEY_ESCAPE:
                    return GLFW_KEY_ESCAPE;
                case KEY_BACKSPACE:
                    return GLFW_KEY_BACKSPACE;
                case KEY_TAB:
                    return GLFW_KEY_TAB;
                case KEY_SPACE:
                    return GLFW_KEY_SPACE;
                case KEY_0:
                    return GLFW_KEY_0;
                case KEY_1:
                    return GLFW_KEY_1;
                case KEY_2:
                    return GLFW_KEY_2;
                case KEY_3:
                    return GLFW_KEY_3;
                case KEY_4:
                    return GLFW_KEY_4;
                case KEY_5:
                    return GLFW_KEY_5;
                case KEY_6:
                    return GLFW_KEY_6;
                case KEY_7:
                    return GLFW_KEY_7;
                case KEY_8:
                    return GLFW_KEY_8;
                case KEY_9:
                    return GLFW_KEY_9;
                case KEY_A:
                    return GLFW_KEY_A;
                case KEY_B:
                    return GLFW_KEY_B;
                case KEY_C:
                    return GLFW_KEY_C;
                case KEY_D:
                    return GLFW_KEY_D;
                case KEY_E:
                    return GLFW_KEY_E;
                case KEY_F:
                    return GLFW_KEY_F;
                case KEY_G:
                    return GLFW_KEY_G;
                case KEY_H:
                    return GLFW_KEY_H;
                case KEY_I:
                    return GLFW_KEY_I;
                case KEY_J:
                    return GLFW_KEY_J;
                case KEY_K:
                    return GLFW_KEY_K;
                case KEY_L:
                    return GLFW_KEY_L;
                case KEY_M:
                    return GLFW_KEY_M;
                case KEY_N:
                    return GLFW_KEY_N;
                case KEY_O:
                    return GLFW_KEY_O;
                case KEY_P:
                    return GLFW_KEY_P;
                case KEY_Q:
                    return GLFW_KEY_Q;
                case KEY_R:
                    return GLFW_KEY_R;
                case KEY_S:
                    return GLFW_KEY_S;
                case KEY_T:
                    return GLFW_KEY_T;
                case KEY_U:
                    return GLFW_KEY_U;
                case KEY_V:
                    return GLFW_KEY_V;
                case KEY_W:
                    return GLFW_KEY_W;
                case KEY_X:
                    return GLFW_KEY_X;
                case KEY_Y:
                    return GLFW_KEY_Y;
                case KEY_Z:
                    return GLFW_KEY_Z;
                case KEY_CAPSLOCK:
                    return GLFW_KEY_CAPS_LOCK;
                case KEY_F1:
                    return GLFW_KEY_F1;
                case KEY_F2:
                    return GLFW_KEY_F2;
                case KEY_F3:
                    return GLFW_KEY_F3;
                case KEY_F4:
                    return GLFW_KEY_F4;
                case KEY_F5:
                    return GLFW_KEY_F5;
                case KEY_F6:
                    return GLFW_KEY_F6;
                case KEY_F7:
                    return GLFW_KEY_F7;
                case KEY_F8:
                    return GLFW_KEY_F8;
                case KEY_F9:
                    return GLFW_KEY_F9;
                case KEY_F10:
                    return GLFW_KEY_F10;
                case KEY_F11:
                    return GLFW_KEY_F11;
                case KEY_F12:
                    return GLFW_KEY_F12;
                case KEY_PRINTSCREEN:
                    return GLFW_KEY_PRINT_SCREEN;
                case KEY_SCROLLLOCK:
                    return GLFW_KEY_SCROLL_LOCK;
                case KEY_PAUSE:
                    return GLFW_KEY_PAUSE;
                case KEY_INSERT:
                    return GLFW_KEY_INSERT;
                case KEY_HOME:
                    return GLFW_KEY_HOME;
                case KEY_PAGEUP:
                    return GLFW_KEY_PAGE_UP;
                case KEY_PAGEDOWN:
                    return GLFW_KEY_PAGE_DOWN;
                case KEY_DELETE:
                    return GLFW_KEY_DELETE;
                case KEY_END:
                    return GLFW_KEY_END;
                case KEY_UP:
                    return GLFW_KEY_UP;
                case KEY_DOWN:
                    return GLFW_KEY_DOWN;
                case KEY_LEFT:
                    return GLFW_KEY_LEFT;
                case KEY_RIGHT:
                    return GLFW_KEY_RIGHT;
                case KEY_KP_DIVIDE:
                    return GLFW_KEY_KP_DIVIDE;
                case KEY_KP_MULTIPLY:
                    return GLFW_KEY_KP_MULTIPLY;
                case KEY_KP_MINUS:
                    return GLFW_KEY_KP_SUBTRACT;
                case KEY_KP_PLUS:
                    return GLFW_KEY_KP_ADD;
                case KEY_KP_ENTER:
                    return GLFW_KEY_KP_ENTER;
                case KEY_KP_0:
                    return GLFW_KEY_KP_0;
                case KEY_KP_1:
                    return GLFW_KEY_KP_1;
                case KEY_KP_2:
                    return GLFW_KEY_KP_2;
                case KEY_KP_3:
                    return GLFW_KEY_KP_3;
                case KEY_KP_4:
                    return GLFW_KEY_KP_4;
                case KEY_KP_5:
                    return GLFW_KEY_KP_5;
                case KEY_KP_6:
                    return GLFW_KEY_KP_6;
                case KEY_KP_7:
                    return GLFW_KEY_KP_7;
                case KEY_KP_8:
                    return GLFW_KEY_KP_8;
                case KEY_KP_9:
                    return GLFW_KEY_KP_9;
                case KEY_KP_PERIOD:
                    return GLFW_KEY_KP_DECIMAL;
                case KEY_LCTRL:
                    return GLFW_KEY_LEFT_CONTROL;
                case KEY_LSHIFT:
                    return GLFW_KEY_LEFT_SHIFT;
                case KEY_LALT:
                    return GLFW_KEY_LEFT_ALT;
                case KEY_RCTRL:
                    return GLFW_KEY_RIGHT_CONTROL;
                case KEY_RSHIFT:
                    return GLFW_KEY_RIGHT_SHIFT;
                case KEY_RALT:
                    return GLFW_KEY_RIGHT_ALT;
                case KEY_UNDEFINED:
                default:
                    throw std::runtime_error("Cannot convert key code " + std::to_string(key));
            }
        }

        static KeyboardKey convertKey(int key) {
            switch (key) {
                case GLFW_KEY_ENTER:
                    return KEY_RETURN;
                case GLFW_KEY_ESCAPE:
                    return KEY_ESCAPE;
                case GLFW_KEY_BACKSPACE:
                    return KEY_BACKSPACE;
                case GLFW_KEY_TAB:
                    return KEY_TAB;
                case GLFW_KEY_SPACE:
                    return KEY_SPACE;
                case GLFW_KEY_0:
                    return KEY_0;
                case GLFW_KEY_1:
                    return KEY_1;
                case GLFW_KEY_2:
                    return KEY_2;
                case GLFW_KEY_3:
                    return KEY_3;
                case GLFW_KEY_4:
                    return KEY_4;
                case GLFW_KEY_5:
                    return KEY_5;
                case GLFW_KEY_6:
                    return KEY_6;
                case GLFW_KEY_7:
                    return KEY_7;
                case GLFW_KEY_8:
                    return KEY_8;
                case GLFW_KEY_9:
                    return KEY_9;

                case GLFW_KEY_A:
                    return KEY_A;
                case GLFW_KEY_B:
                    return KEY_B;
                case GLFW_KEY_C:
                    return KEY_C;
                case GLFW_KEY_D:
                    return KEY_D;
                case GLFW_KEY_E:
                    return KEY_E;
                case GLFW_KEY_F:
                    return KEY_F;
                case GLFW_KEY_G:
                    return KEY_G;
                case GLFW_KEY_H:
                    return KEY_H;
                case GLFW_KEY_I:
                    return KEY_I;
                case GLFW_KEY_J:
                    return KEY_J;
                case GLFW_KEY_K:
                    return KEY_K;
                case GLFW_KEY_L:
                    return KEY_L;
                case GLFW_KEY_M:
                    return KEY_M;
                case GLFW_KEY_N:
                    return KEY_N;
                case GLFW_KEY_O:
                    return KEY_O;
                case GLFW_KEY_P:
                    return KEY_P;
                case GLFW_KEY_Q:
                    return KEY_Q;
                case GLFW_KEY_R:
                    return KEY_R;
                case GLFW_KEY_S:
                    return KEY_S;
                case GLFW_KEY_T:
                    return KEY_T;
                case GLFW_KEY_U:
                    return KEY_U;
                case GLFW_KEY_V:
                    return KEY_V;
                case GLFW_KEY_W:
                    return KEY_W;
                case GLFW_KEY_X:
                    return KEY_X;
                case GLFW_KEY_Y:
                    return KEY_Y;
                case GLFW_KEY_Z:
                    return KEY_Z;

                case GLFW_KEY_CAPS_LOCK:
                    return KEY_CAPSLOCK;

                case GLFW_KEY_F1:
                    return KEY_F1;
                case GLFW_KEY_F2:
                    return KEY_F2;
                case GLFW_KEY_F3:
                    return KEY_F3;
                case GLFW_KEY_F4:
                    return KEY_F4;
                case GLFW_KEY_F5:
                    return KEY_F5;
                case GLFW_KEY_F6:
                    return KEY_F6;
                case GLFW_KEY_F7:
                    return KEY_F7;
                case GLFW_KEY_F8:
                    return KEY_F8;
                case GLFW_KEY_F9:
                    return KEY_F9;
                case GLFW_KEY_F10:
                    return KEY_F10;
                case GLFW_KEY_F11:
                    return KEY_F11;
                case GLFW_KEY_F12:
                    return KEY_F12;

                case GLFW_KEY_PRINT_SCREEN:
                    return KEY_PRINTSCREEN;
                case GLFW_KEY_SCROLL_LOCK:
                    return KEY_SCROLLLOCK;
                case GLFW_KEY_PAUSE:
                    return KEY_PAUSE;
                case GLFW_KEY_INSERT:
                    return KEY_INSERT;
                case GLFW_KEY_HOME:
                    return KEY_HOME;
                case GLFW_KEY_PAGE_UP:
                    return KEY_PAGEUP;
                case GLFW_KEY_PAGE_DOWN:
                    return KEY_PAGEDOWN;
                case GLFW_KEY_DELETE:
                    return KEY_DELETE;
                case GLFW_KEY_END:
                    return KEY_END;
                case GLFW_KEY_UP:
                    return KEY_UP;
                case GLFW_KEY_DOWN:
                    return KEY_DOWN;
                case GLFW_KEY_LEFT:
                    return KEY_LEFT;
                case GLFW_KEY_RIGHT:
                    return KEY_RIGHT;

                case GLFW_KEY_KP_DIVIDE:
                    return KEY_KP_DIVIDE;
                case GLFW_KEY_KP_MULTIPLY:
                    return KEY_KP_MULTIPLY;
                case GLFW_KEY_KP_SUBTRACT:
                    return KEY_KP_MINUS;
                case GLFW_KEY_KP_ADD:
                    return KEY_KP_PLUS;
                case GLFW_KEY_KP_ENTER:
                    return KEY_KP_ENTER;
                case GLFW_KEY_KP_0:
                    return KEY_KP_0;
                case GLFW_KEY_KP_1:
                    return KEY_KP_1;
                case GLFW_KEY_KP_2:
                    return KEY_KP_2;
                case GLFW_KEY_KP_3:
                    return KEY_KP_3;
                case GLFW_KEY_KP_4:
                    return KEY_KP_4;
                case GLFW_KEY_KP_5:
                    return KEY_KP_5;
                case GLFW_KEY_KP_6:
                    return KEY_KP_6;
                case GLFW_KEY_KP_7:
                    return KEY_KP_7;
                case GLFW_KEY_KP_8:
                    return KEY_KP_8;
                case GLFW_KEY_KP_9:
                    return KEY_KP_9;
                case GLFW_KEY_KP_DECIMAL:
                    return KEY_KP_PERIOD;

                case GLFW_KEY_LEFT_CONTROL:
                    return KEY_LCTRL;
                case GLFW_KEY_LEFT_SHIFT:
                    return KEY_LSHIFT;
                case GLFW_KEY_LEFT_ALT:
                    return KEY_LALT;
                case GLFW_KEY_RIGHT_CONTROL:
                    return KEY_RCTRL;
                case GLFW_KEY_RIGHT_SHIFT:
                    return KEY_RSHIFT;
                case GLFW_KEY_RIGHT_ALT:
                    return KEY_RALT;

                default:
                    return KEY_UNDEFINED;
            }
        }
    };
}

#endif //XENGINE_GLFWTYPECONVERTER_HPP
