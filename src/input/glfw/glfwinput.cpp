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

#ifdef BUILD_ENGINE_DISPLAY_GLFW

#include <map>
#include <stdexcept>
#include <mutex>

#include "glfwinput.hpp"

#include "glfwtypeconverter.hpp"

namespace xengine {
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

    GLFWInput::GLFWInput(GLFWwindow &wndH) : wndH(wndH) {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        windowMapping[&wndH] = this;

        glfwSetKeyCallback(&wndH, xengine::glfwKeyCallback);
        glfwSetCursorPosCallback(&wndH, xengine::glfwCursorCallback);
        glfwSetMouseButtonCallback(&wndH, xengine::glfwMouseKeyCallback);

        //GLFW Does not appear to send connected events for joysticks which are already connected when the application starts.
        for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++) {
            if (glfwJoystickIsGamepad(i)) {
                gamepads[i] = GamePad();
                for (auto listener: listeners)
                    listener->onGamepadConnected(i);
            }
        }

        glfwSetJoystickCallback(xengine::glfwJoystickCallback);

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
                    listener->onMouseKeyDown(LEFT);
                }
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                for (auto listener: listeners) {
                    listener->onMouseKeyDown(MIDDLE);
                }
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                for (auto listener: listeners) {
                    listener->onMouseKeyDown(RIGHT);
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

    void GLFWInput::addListener(InputListener &listener) {
        if (listeners.find(&listener) != listeners.end())
            throw std::runtime_error("Input listener already registered");
        listeners.insert(&listener);
    }

    void GLFWInput::removeListener(InputListener &listener) {
        listeners.erase(&listener);
    }

    //TODO: Implement clipboard support
    void GLFWInput::setClipboardText(std::string text) {
        throw std::runtime_error("Not Implemented");
    }

    std::string GLFWInput::getClipboardText() {
        throw std::runtime_error("Not Implemented");
    }

    //TODO: Implement cursor image change
    void GLFWInput::setMouseCursorImage(const Image<ColorRGBA> &image) {
        throw std::runtime_error("Not implemented");
    }

    void GLFWInput::clearMouseCursorImage() {
        throw std::runtime_error("Not implemented");
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

    void GLFWInput::onKeyDown(KeyboardKey key) {
        keyboards[0].keysDown.insert(key);
    }

    void GLFWInput::onKeyUp(KeyboardKey key) {
        keyboards[0].keysDown.erase(key);
    }

    void GLFWInput::onMouseMove(double xPos, double yPos) {
        mice[0].position.x = xPos;
        mice[0].position.y = yPos;
    }

    void GLFWInput::onMouseWheelScroll(double amount) {
        mice[0].wheelDelta = amount;
    }

    void GLFWInput::onMouseKeyDown(MouseButton key) {
        mice[0].buttonsDown.insert(key);
    }

    void GLFWInput::onMouseKeyUp(MouseButton key) {
        mice[0].buttonsDown.erase(key);
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
        gamepads[id].buttonsDown.insert(button);
    }

    void GLFWInput::onGamepadButtonUp(int id, GamePadButton button) {
        gamepads[id].buttonsDown.erase(button);
    }
}

#endif