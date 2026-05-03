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

#ifndef XENGINE_GLFWWINDOWGL_HPP
#define XENGINE_GLFWWINDOWGL_HPP

#include "glfwwindow.hpp"
#include "monitorglfw.hpp"

#include "display/windowgl.hpp"

namespace xng::glfw {
    class GLFWWindowGL final : public GLFWWindow, public WindowGl {
    public:
        GLFWWindowGL(const std::string &title, Vec2i size, WindowAttributes attributes, GLFWwindow *share = nullptr);

        GLFWWindowGL(const std::string &title,
                     Vec2i size,
                     WindowAttributes attributes,
                     MonitorGLFW &monitor,
                     VideoMode videoMode,
                     GLFWwindow *share = nullptr);

        ~GLFWWindowGL() override = default;

        void bindContext() override;

        void unbindContext() override;

        void swapBuffers() override;

    private:
        GLFWwindow *share = nullptr;
    };
}

#endif //XENGINE_GLFWWINDOWGL_HPP
