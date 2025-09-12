/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifdef BUILD_GLFW_OPENGL

#include <stdexcept>
#include <mutex>

#include "glad/glad.h"

#include "display/glfw/opengl/glfwwindowgl.hpp"

namespace xng::glfw {
    GLFWWindowGL::GLFWWindowGL(const std::string &title, Vec2i size, WindowAttributes attributes) {
        glfwDefaultWindowHints();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, attributes.debug);

        createWindow(title, size, attributes);

        glfwMakeContextCurrent(wndH);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize glad");
        }

        glfwSwapInterval(attributes.vsync);
    }

    GLFWWindowGL::GLFWWindowGL(const std::string &title,
                               Vec2i size,
                               WindowAttributes attributes,
                               MonitorGLFW &monitor,
                               VideoMode videoMode) {
        glfwDefaultWindowHints();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, attributes.debug);

        //Setup monitor video mode hints
        glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
        glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);

        createWindow(title, size, attributes, monitor, videoMode);

        glfwMakeContextCurrent(wndH);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize glad");
        }
    }

    void GLFWWindowGL::swapBuffers() {
        glfwSwapBuffers(wndH);
    }
}

#endif