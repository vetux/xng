/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_GLFWWINDOWGL_HPP
#define XENGINE_GLFWWINDOWGL_HPP

#include <set>

#include "display/glfw/glfwwindow.hpp"

namespace xng::glfw {
    class GLFWWindowGL : public GLFWWindow {
    public:
        GLFWWindowGL(const std::string &title, Vec2i size, WindowAttributes attributes);

        GLFWWindowGL(const std::string &title,
                     Vec2i size,
                     WindowAttributes attributes,
                     MonitorGLFW &monitor,
                     VideoMode videoMode);

        ~GLFWWindowGL() override = default;

        std::unique_ptr<RenderTarget> getRenderTarget(RenderDevice &device) override;

        void swapBuffers() override;
    };
}

#endif //XENGINE_GLFWWINDOWGL_HPP
