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

#include <map>
#include <stdexcept>
#include <mutex>
#include <functional>

#include "glfwinput.hpp"

#include "glfwtypeconverter.hpp"

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

        //GLFW Does not appear to send connected events for joysticks which are already connected when the application starts.
        for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++) {
            if (glfwJoystickIsGamepad(i)) {
                gamepads[i] = GamePad();
                for (auto listener: listeners)
                    listener->onGamepadConnected(i);
            }
        }

        glfwSetJoystickCallback(xng::glfwJoystickCallback);

        listeners.insert(this);

        //GLFW Supports only one keyboard and mouse
        keyboards[0] = Keyboard();
        mice[0] = Mouse();
    }

    GLFWInput::~GLFWInput() {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        windowMapping.erase(&wndH);
        listeners.erase(this);
    }

    void GLFWInput::glfwKeyCallback(int key, int scancode, int action, int mods) {
        KeyboardKey k = GLFWTypeConverter::convertKey(key);
        bool kd = action != GLFW_RELEASE;
        for (auto listener: listeners) {
            if (kd)
                listener->onKeyDown(k);
            else
                listener->onKeyUp(k);
        }
    }

    void GLFWInput::glfwCursorCallback(double xpos, double ypos) {
        for (auto listener: listeners) {
            listener->onMouseMove(xpos, ypos);
        }
    }

    void GLFWInput::glfwMouseKeyCallback(int button, int action, int mods) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                for (auto listener: listeners) {
                    if (action == GLFW_PRESS)
                        listener->onMouseKeyDown(LEFT);
                    else
                        listener->onMouseKeyUp(LEFT);
                }
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                for (auto listener: listeners) {
                    if (action == GLFW_PRESS)
                        listener->onMouseKeyDown(MIDDLE);
                    else
                        listener->onMouseKeyUp(MIDDLE);
                }
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                for (auto listener: listeners) {
                    if (action == GLFW_PRESS)
                        listener->onMouseKeyDown(RIGHT);
                    else
                        listener->onMouseKeyUp(RIGHT);
                }
                break;
            default:
                break;
        }
    }

    void GLFWInput::glfwJoystickCallback(int jid, int event) {
        if (glfwJoystickIsGamepad(jid)) {
            switch (event) {
                case GLFW_CONNECTED:
                    gamepads[jid] = GamePad();
                    for (auto listener: listeners)
                        listener->onGamepadConnected(jid);
                    break;
                case GLFW_DISCONNECTED:
                    gamepads.erase(jid);
                    for (auto listener: listeners)
                        listener->onGamepadDisconnected(jid);
                    break;
                default:
                    break;
            }
        }
    }

    void GLFWInput::glfwScrollCallback(double xoffset, double yoffset) {
        mice.at(0).wheelDelta = yoffset;
        for (auto listener: listeners)
            listener->onMouseWheelScroll(yoffset);
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

    void GLFWInput::setMouseCursorHidden(bool cursorHidden) {
        glfwSetInputMode(&wndH, GLFW_CURSOR, cursorHidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
    }

    const Keyboard &GLFWInput::getKeyboard() const {
        return keyboards.at(0);
    }

    const Mouse &GLFWInput::getMouse() const {
        return mice.at(0);
    }

    const GamePad &GLFWInput::getGamePad() const {
        return gamepads.at(0);
    }

    const std::map<int, Keyboard> &GLFWInput::getKeyboards() const {
        return keyboards;
    }

    const std::map<int, Mouse> &GLFWInput::getMice() const {
        return mice;
    }

    const std::map<int, GamePad> &GLFWInput::getGamePads() const {
        return gamepads;
    }

    void GLFWInput::update() {
        for (auto &pair: mice) {
            pair.second.wheelDelta = 0;
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

    void GLFWInput::onKeyDown(KeyboardKey key) {
        if (keyboards[0].keys[key] != HELD)
            keyboards[0].keys[key] = PRESSED;
    }

    void GLFWInput::onKeyUp(KeyboardKey key) {
        keyboards[0].keys[key] = RELEASED;
    }

    void GLFWInput::onMouseMove(double xPos, double yPos) {
        mice[0].positionDelta = mice[0].position - Vec2d(xPos, yPos);

        mice[0].position.x = xPos;
        mice[0].position.y = yPos;
    }

    void GLFWInput::onMouseWheelScroll(double amount) {
        mice[0].wheelDelta = amount;
    }

    void GLFWInput::onMouseKeyDown(MouseButton key) {
        if (mice[0].buttons[key] != HELD)
            mice[0].buttons[key] = PRESSED;
    }

    void GLFWInput::onMouseKeyUp(MouseButton key) {
        mice[0].buttons[key] = RELEASED;
    }

    void GLFWInput::onGamepadConnected(int id) {
        gamepads[id] = GamePad();
    }

    void GLFWInput::onGamepadDisconnected(int id) {
        gamepads.erase(id);
    }

    void GLFWInput::onGamepadAxis(int id, GamePadAxis axis, double amount) {
        gamepads[id].axies.at(axis) = amount;
    }

    void GLFWInput::onGamepadButtonDown(int id, GamePadButton button) {
        if (gamepads[id].buttons[button] != HELD)
            gamepads[id].buttons[button] = PRESSED;
    }

    void GLFWInput::onGamepadButtonUp(int id, GamePadButton button) {
        gamepads[id].buttons[button] = RELEASED;
    }

    std::function<void()> GLFWInput::addListener(InputListener &listener) {
        if (listeners.find(&listener) != listeners.end())
            throw std::runtime_error("Input listener already registered");
        listeners.insert(&listener);
        return [this, &listener]() {
            removeListener(listener);
        };
    }

    void GLFWInput::removeListener(InputListener &listener) {
        listeners.erase(&listener);
    }
}