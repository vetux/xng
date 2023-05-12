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

#include "xng/driver/glfw/glfwdisplaydriver.hpp"

#ifdef DRIVER_GLFW_OPENGL

#include "display/glfw/opengl/windowglfwgl.hpp"

#endif

#include "display/glfw/monitorglfw.hpp" //Has to come after windowglfw because of glad include collision with glfw (Including glfw and then glad afterwards gives compiler error, the reverse is legal)

#include "xng/gpu/gpudriver.hpp"

namespace xng::glfw {
    static std::unique_ptr<Window> makeWindow(GpuDriverBackend gpuBackend,
                                              const std::string &title,
                                              Vec2i size,
                                              WindowAttributes attributes,
                                              Monitor *monitor = nullptr,
                                              VideoMode mode = {}) {
        if (false) {}
#ifdef DRIVER_GLFW_OPENGL
        else if (gpuBackend == OPENGL_4_6) {
            if (monitor) {
                return std::make_unique<WindowGLFWGL>(title,
                                                      size,
                                                      attributes,
                                                      dynamic_cast<MonitorGLFW &>(*monitor),
                                                      mode);
            } else {
                return std::make_unique<WindowGLFWGL>(title,
                                                      size,
                                                      attributes);
            }
        }
#endif
        else {
            throw std::runtime_error("Unsupported gpu backend " + std::to_string(gpuBackend));
        }
    }

    std::unique_ptr<Monitor> GLFWDisplayDriver::getPrimaryMonitor() {
        return std::make_unique<MonitorGLFW>(glfwGetPrimaryMonitor());
    }

    std::set<std::unique_ptr<Monitor>> GLFWDisplayDriver::getMonitors() {
        std::set<std::unique_ptr<Monitor>> ret;

        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        for (int i = 0; i < count; i++) {
            ret.insert(std::make_unique<MonitorGLFW>(monitors[i]));
        }

        return ret;
    }

    std::unique_ptr<Window> GLFWDisplayDriver::createWindow(GpuDriverBackend gpuBackend) {
        return makeWindow(gpuBackend, "Window GLFW", Vec2i(600, 300), WindowAttributes());
    }

    std::unique_ptr<Window> GLFWDisplayDriver::createWindow(GpuDriverBackend gpuBackend,
                                                            const std::string &title,
                                                            Vec2i size,
                                                            WindowAttributes attributes) {
        return makeWindow(gpuBackend, title, size, attributes);
    }

    std::unique_ptr<Window> GLFWDisplayDriver::createWindow(GpuDriverBackend gpuBackend,
                                                            const std::string &title,
                                                            Vec2i size,
                                                            WindowAttributes attributes,
                                                            Monitor &monitor,
                                                            VideoMode mode) {
        return makeWindow(gpuBackend, title, size, attributes, &monitor, mode);
    }
}
