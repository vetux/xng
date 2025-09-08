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

#include "display/glfw/monitorglfw.hpp"

#include "xng/adapters/glfw/glfw.hpp"

#ifdef BUILD_GLFW_OPENGL

#include "display/glfw/opengl/glfwwindowgl.hpp"

#endif

#ifdef BUILD_GLFW_VULKAN

#include "display/glfw/vulkan/glfwwindowvk.hpp"

#endif

#include "xng/gpu/gpuengine.hpp"

namespace xng::glfw {
    static std::unique_ptr<Window> makeWindow(GpuEngineBackend gpuBackend,
                                              const std::string &title,
                                              Vec2i size,
                                              WindowAttributes attributes,
                                              Monitor *monitor = nullptr,
                                              VideoMode mode = {}) {
        if (false) {}
#ifdef BUILD_GLFW_OPENGL
        else if (gpuBackend == OPENGL_4_6) {
            if (monitor) {
                return std::make_unique<GLFWWindowGL>(title,
                                                      size,
                                                      attributes,
                                                      dynamic_cast<MonitorGLFW &>(*monitor),
                                                      mode);
            } else {
                return std::make_unique<GLFWWindowGL>(title,
                                                      size,
                                                      attributes);
            }
        }
#endif
#ifdef BUILD_GLFW_VULKAN
        else if (gpuBackend == VULKAN_1_1) {
            if (monitor) {
                return std::make_unique<GLFWWindowVk>(title,
                                                      size,
                                                      attributes,
                                                      dynamic_cast<MonitorGLFW &>(*monitor),
                                                      mode);
            } else {
                return std::make_unique<GLFWWindowVk>(title,
                                                      size,
                                                      attributes);
            }
        }
#endif
        else {
            throw std::runtime_error("Unsupported gpu backend " + std::to_string(gpuBackend));
        }
    }

    GLFW::GLFW() {
        glfwInit();
    }

    GLFW::~GLFW() {
        glfwTerminate();
    }

    std::unique_ptr<Monitor> GLFW::getPrimaryMonitor() {
        return std::make_unique<MonitorGLFW>(glfwGetPrimaryMonitor());
    }

    std::set<std::unique_ptr<Monitor>> GLFW::getMonitors() {
        std::set<std::unique_ptr<Monitor>> ret;

        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        for (int i = 0; i < count; i++) {
            ret.insert(std::make_unique<MonitorGLFW>(monitors[i]));
        }

        return ret;
    }

    std::unique_ptr<Window> GLFW::createWindow(GpuEngineBackend gpuBackend) {
        return makeWindow(gpuBackend, "Window GLFW", Vec2i(600, 300), WindowAttributes());
    }

    std::unique_ptr<Window> GLFW::createWindow(GpuEngineBackend gpuBackend,
                                               const std::string &title,
                                               Vec2i size,
                                               WindowAttributes attributes) {
        return makeWindow(gpuBackend, title, size, attributes);
    }

    std::unique_ptr<Window> GLFW::createWindow(GpuEngineBackend gpuBackend,
                                               const std::string &title,
                                               Vec2i size,
                                               WindowAttributes attributes,
                                               Monitor &monitor,
                                               VideoMode mode) {
        return makeWindow(gpuBackend, title, size, attributes, &monitor, mode);
    }

    std::vector<const char *> GLFW::getRequiredVulkanExtensions() {
#ifdef BUILD_GLFW_VULKAN
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        return {glfwExtensions, glfwExtensions + glfwExtensionCount};
#else
        return {};
#endif
    }
}
