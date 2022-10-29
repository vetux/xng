/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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


#include "windowglfwgl.hpp"
#include "monitorglfw.hpp" //Has to come after windowglfw because of glad include collision with glfw (Including glfw and then glad afterwards gives compiler error, the reverse is legal)

namespace xng::glfw {
        std::unique_ptr<Monitor> getPrimaryMonitor() {
            return std::make_unique<MonitorGLFW>(glfwGetPrimaryMonitor());
        }

        std::set<std::unique_ptr<Monitor>> getMonitors() {
            std::set<std::unique_ptr<Monitor>> ret;

            int count;
            GLFWmonitor **monitors = glfwGetMonitors(&count);
            for (int i = 0; i < count; i++) {
                ret.insert(std::make_unique<MonitorGLFW>(monitors[i]));
            }

            return ret;
        }

        std::unique_ptr<Window> createWindow(RenderPlatform platform) {
            switch (platform) {
                case OPENGL_4_1:
                    return std::make_unique<WindowGLFWGL>("Window GLFW", Vec2i(600, 300), WindowAttributes());
                default:
                    throw std::runtime_error("Unsupported render platform");
            }
        }

        std::unique_ptr<Window> createWindow(RenderPlatform platform,
                                             const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes) {
            switch (platform) {
                case OPENGL_4_1:
                    return std::make_unique<WindowGLFWGL>(title, size, attributes);
                default:
                    throw std::runtime_error("Unsupported render platform");
            }
        }

        std::unique_ptr<Window> createWindow(RenderPlatform platform,
                                             const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes,
                                             Monitor &monitor,
                                             VideoMode mode) {
            switch (platform) {
                case OPENGL_4_1:
                    return std::make_unique<WindowGLFWGL>(title,
                                                          size,
                                                          attributes,
                                                          dynamic_cast<MonitorGLFW &>(monitor),
                                                          mode);
                default:
                    throw std::runtime_error("Unsupported render platform");
            }
        }
    }
}