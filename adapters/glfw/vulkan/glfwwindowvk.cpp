/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef BUILD_GLFW_VULKAN

#include "glfwwindowvk.hpp"

namespace xng::glfw {
    GLFWWindowVk::GLFWWindowVk(const std::string &title, Vec2i size, WindowAttributes attributes) {
        glfwDefaultWindowHints();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        createWindow(title, size, attributes);
    }

    GLFWWindowVk::GLFWWindowVk(const std::string &title,
                               Vec2i size,
                               WindowAttributes attributes,
                               MonitorGLFW &monitor,
                               VideoMode videoMode) {
        glfwDefaultWindowHints();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        //Setup monitor video mode hints
        glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
        glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);

        createWindow(title, size, attributes, monitor, videoMode);
    }

    void GLFWWindowVk::swapBuffers() {

    }
}

#endif