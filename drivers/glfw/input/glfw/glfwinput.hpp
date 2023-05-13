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

#ifndef XENGINE_GLFWINPUT_HPP
#define XENGINE_GLFWINPUT_HPP

#include <set>
#include <map>

#include <GLFW/glfw3.h>

#include "xng/asset/image.hpp"
#include "xng/input/input.hpp"

namespace xng {
    class GLFWInput : public Input {
    public:
        explicit GLFWInput(GLFWwindow &wndH);

        ~GLFWInput() override;

        void glfwKeyCallback(int key, int scancode, int action, int mods);

        void glfwCharCallback(unsigned int codepoint);

        void glfwCursorCallback(double xpos, double ypos);

        void glfwMouseKeyCallback(int button, int action, int mods);

        void glfwJoystickCallback(int jid, int event);

        void glfwScrollCallback(double xoffset, double yoffset);

        void setClipboardText(std::string text) override;

        std::string getClipboardText() override;

        void setMouseCursorImage(const ImageRGBA &image) override;

        void clearMouseCursorImage() override;

        void setMouseCursorMode(CursorMode mode) override;

        void setEventBus(const EventBus &bus) override;

        void clearEventBus() override;

        const InputDevice &getDevice(std::type_index deviceType, int id) override;

         std::map<int, const std::reference_wrapper<InputDevice>> getDevices(std::type_index deviceType) override;

        void update();

    private:
        void invokeEvent(const Event &event);

        GLFWwindow &wndH;

        const EventBus * eventBus = nullptr;

        std::map<int, Keyboard> keyboards;

        std::map<int, Mouse> mice;

        std::map<int, GamePad> gamepads;
    };
}

#endif //XENGINE_GLFWINPUT_HPP
