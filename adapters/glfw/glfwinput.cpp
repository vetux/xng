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

#include <map>
#include <stdexcept>
#include <mutex>
#include <functional>

#include "glfwinput.hpp"

#include "glfwtypeconverter.hpp"
#include "xng/input/device/gamepadevent.hpp"
#include "xng/input/device/keyboardevent.hpp"
#include "xng/input/device/mouseevent.hpp"

namespace xng {
    std::mutex windowMappingMutex;
    std::map<GLFWwindow *, GLFWInput *> windowMapping;

    void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        if (windowMapping.find(window) != windowMapping.end()) {
            windowMapping[window]->glfwKeyCallback(key, scancode, action, mods);
        } else {
            fprintf(stderr, "Received glfw callback with non registered window.");
        }
    }

    void glfwCharCallback(GLFWwindow *window, unsigned int codepoint) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        if (windowMapping.find(window) != windowMapping.end()) {
            windowMapping[window]->glfwCharCallback(codepoint);
        } else {
            fprintf(stderr, "Received glfw callback with non registered window.");
        }
    }

    void glfwCursorCallback(GLFWwindow *window, double xpos, double ypos) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        if (windowMapping.find(window) != windowMapping.end()) {
            windowMapping[window]->glfwCursorCallback(xpos, ypos);
        } else {
            fprintf(stderr, "Received glfw callback with non registered window.");
        }
    }

    void glfwMouseKeyCallback(GLFWwindow *window, int button, int action, int mods) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        if (windowMapping.find(window) != windowMapping.end()) {
            windowMapping[window]->glfwMouseKeyCallback(button, action, mods);
        } else {
            fprintf(stderr, "Received glfw callback with non registered window.");
        }
    }

    void glfwJoystickCallback(int jid, int event) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        for (auto &pair: windowMapping) {
            pair.second->glfwJoystickCallback(jid, event);
        }
    }

    void glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        if (windowMapping.find(window) != windowMapping.end()) {
            windowMapping[window]->glfwScrollCallback(xoffset, yoffset);
        } else {
            fprintf(stderr, "Received glfw callback with non registered window.");
        }
    }

    GLFWInput::GLFWInput(GLFWwindow &wndH) : wndH(wndH) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        windowMapping[&wndH] = this;

        glfwSetKeyCallback(&wndH, xng::glfwKeyCallback);
        glfwSetCursorPosCallback(&wndH, xng::glfwCursorCallback);
        glfwSetMouseButtonCallback(&wndH, xng::glfwMouseKeyCallback);
        glfwSetScrollCallback(&wndH, xng::glfwScrollCallback);
        glfwSetCharCallback(&wndH, xng::glfwCharCallback);

        //GLFW Does not appear to send connected events for joysticks which are already connected when the application starts.
        for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++) {
            if (glfwJoystickIsGamepad(i)) {
                gamepads[i] = GamePad();
                this->glfwJoystickCallback(i, GLFW_CONNECTED);
            }
        }

        glfwSetJoystickCallback(xng::glfwJoystickCallback);

        //GLFW Supports only one keyboard and mouse
        keyboards[0] = Keyboard();
        mice[0] = Mouse();
    }

    GLFWInput::~GLFWInput() {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        windowMapping.erase(&wndH);
    }

    void GLFWInput::glfwKeyCallback(int key, int scancode, int action, int mods) {
        KeyboardKey k = GLFWTypeConverter::convertKey(key);
        bool kd = action != GLFW_RELEASE;
        if (kd) {
            if (keyboards[0].keys[k] != HELD)
                keyboards[0].keys[k] = PRESSED;
            auto ev = KeyboardEvent();
            ev.type = KeyboardEvent::KEYBOARD_KEY_DOWN;
            ev.key = k;
            ev.id = 0;
            if (eventBus) {
                eventBus->invoke(ev);
            }
        } else {
            keyboards[0].keys[k] = RELEASED;
            auto ev = KeyboardEvent();
            ev.type = KeyboardEvent::KEYBOARD_KEY_UP;
            ev.key = k;
            ev.id = 0;
            if (eventBus) {
                eventBus->invoke(ev);
            }
        }
    }

    void GLFWInput::glfwCharCallback(unsigned int codepoint) {
        keyboards[0].characterInput += static_cast<char32_t>(codepoint);
        auto ev = KeyboardEvent();
        ev.type = KeyboardEvent::KEYBOARD_CHARACTER_INPUT;
        ev.value = static_cast<char32_t>(codepoint);
        ev.id = 0;
        if (eventBus) {
            eventBus->invoke(ev);
        }
    }

    void GLFWInput::glfwCursorCallback(double xPos, double yPos) {
        mice[0].positionDelta = mice[0].position - Vec2d(xPos, yPos);
        mice[0].position.x = xPos;
        mice[0].position.y = yPos;
        auto ev = MouseEvent();
        ev.type = MouseEvent::MOUSE_MOVE;
        ev.id = 0;
        ev.xPos = xPos;
        ev.yPos = yPos;
        if (eventBus) {
            eventBus->invoke(ev);
        }
    }

    void GLFWInput::glfwMouseKeyCallback(int button, int action, int mods) {
        MouseButton btn;
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                btn = MOUSE_BUTTON_LEFT;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                btn = MOUSE_BUTTON_MIDDLE;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                btn = MOUSE_BUTTON_RIGHT;
                break;
            default:
                break;
        }
        if (action == GLFW_PRESS) {
            if (mice[0].buttons[btn] != HELD)
                mice[0].buttons[btn] = PRESSED;
            auto ev = MouseEvent();
            ev.type = MouseEvent::MOUSE_KEY_DOWN;
            ev.id = 0;
            ev.key = btn;
            if (eventBus) {
                eventBus->invoke(ev);
            }
        } else {
            mice[0].buttons[btn] = RELEASED;
            auto ev = MouseEvent();
            ev.type = MouseEvent::MOUSE_KEY_UP;
            ev.id = 0;
            ev.key = btn;
            if (eventBus) {
                eventBus->invoke(ev);
            }
        }
    }

    void GLFWInput::glfwJoystickCallback(int jid, int event) {
        if (glfwJoystickIsGamepad(jid)) {
            switch (event) {
                case GLFW_CONNECTED: {
                    gamepads[jid] = GamePad();
                    auto ev = GamePadEvent();
                    ev.type = GamePadEvent::GAMEPAD_CONNECTED;
                    ev.id = jid;
                    if (eventBus) {
                        eventBus->invoke(ev);
                    }
                    break;
                }
                case GLFW_DISCONNECTED: {
                    gamepads.erase(jid);
                    auto ev = GamePadEvent();
                    ev.type = GamePadEvent::GAMEPAD_DISCONNECTED;
                    ev.id = jid;
                    if (eventBus) {
                        eventBus->invoke(ev);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void GLFWInput::glfwScrollCallback(double xoffset, double yoffset) {
        mice[0].wheelDelta.x = xoffset;
        mice[0].wheelDelta.y = yoffset;

        auto ev = MouseEvent();
        ev.type = MouseEvent::MOUSE_WHEEL_SCROLL;
        ev.id = 0;
        ev.xAmount = xoffset;
        ev.yAmount = yoffset;
        if (eventBus) {
            eventBus->invoke(ev);
        }
    }

    //TODO: Implement clipboard support
    void GLFWInput::setClipboardText(std::string text) {
        throw std::runtime_error("Not Implemented");
    }

    std::string GLFWInput::getClipboardText() {
        throw std::runtime_error("Not Implemented");
    }

    //TODO: Implement cursor image change
    void GLFWInput::setMouseCursorImage(const ImageRGBA &image) {
        throw std::runtime_error("Not implemented");
    }

    void GLFWInput::clearMouseCursorImage() {
        throw std::runtime_error("Not implemented");
    }

    void GLFWInput::setMouseCursorMode(CursorMode mode) {
        switch (mode) {
            case Input::CURSOR_NORMAL:
                glfwSetInputMode(&wndH, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case Input::CURSOR_HIDDEN:
                glfwSetInputMode(&wndH, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            case Input::CURSOR_DISABLED:
                glfwSetInputMode(&wndH, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
        }
    }

    void GLFWInput::update() {
        for (auto &pair: mice) {
            pair.second.wheelDelta = Vec2d(0);
            pair.second.positionDelta = Vec2d(0);
            for (auto &k: pair.second.buttons) {
                if (k.second == PRESSED)
                    k.second = HELD;
            }
        }

        for (auto &pair: keyboards) {
            for (auto &k: pair.second.keys) {
                if (k.second == PRESSED)
                    k.second = HELD;
            }
        }

        for (auto &pair: gamepads) {
            for (auto &k: pair.second.buttons) {
                if (k.second == PRESSED)
                    k.second = HELD;
            }
        }
    }

    const std::map<int, Keyboard> &GLFWInput::getKeyboards() {
        return keyboards;
    }

    const std::map<int, Mouse> &GLFWInput::getMice() {
        return mice;
    }

    const std::map<int, GamePad> &GLFWInput::getGamePads() {
        return gamepads;
    }

    void GLFWInput::setEventBus(const EventBus &bus) {
        eventBus = &bus;
    }

    void GLFWInput::clearEventBus() {
        eventBus = nullptr;
    }
}
