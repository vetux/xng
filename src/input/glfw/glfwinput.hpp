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

#ifndef XENGINE_GLFWINPUT_HPP
#define XENGINE_GLFWINPUT_HPP

#include <set>
#include <map>

#include <GLFW/glfw3.h>

#include "asset/image.hpp"
#include "input/input.hpp"

namespace xengine {
    class GLFWInput : public Input, InputListener {
    public:
        explicit GLFWInput(GLFWwindow &wndH);

        ~GLFWInput() override;

        void glfwKeyCallback(int key, int scancode, int action, int mods);

        void glfwCursorCallback(double xpos, double ypos);

        void glfwMouseKeyCallback(int button, int action, int mods);

        void glfwJoystickCallback(int jid, int event);

        void glfwScrollCallback(double xoffset, double yoffset);

        void addListener(InputListener &listener) override;

        void removeListener(InputListener &listener) override;

        void setClipboardText(std::string text) override;

        std::string getClipboardText() override;

        void setMouseCursorImage(const ImageRGBA &image) override;

        void clearMouseCursorImage() override;

        const std::map<int, Keyboard> &getKeyboards() const override;

        const std::map<int, Mouse> &getMice() const override;

        const std::map<int, GamePad> &getGamePads() const override;

        void update();

    private:
        void onKeyDown(KeyboardKey key) override;

        void onKeyUp(KeyboardKey key) override;

        void onMouseMove(double xPos, double yPos) override;

        void onMouseWheelScroll(double amount) override;

        void onMouseKeyDown(MouseButton key) override;

        void onMouseKeyUp(MouseButton key) override;

        void onGamepadConnected(int id) override;

        void onGamepadDisconnected(int id) override;

        void onGamepadAxis(int id, GamePadAxis axis, double amount) override;

        void onGamepadButtonDown(int id, GamePadButton button) override;

        void onGamepadButtonUp(int id, GamePadButton button) override;

    private:
        GLFWwindow &wndH;

        std::set<InputListener *> listeners;

        std::map<int, Keyboard> keyboards;

        std::map<int, Mouse> mice;

        std::map<int, GamePad> gamepads;
    };
}

#endif //XENGINE_GLFWINPUT_HPP
