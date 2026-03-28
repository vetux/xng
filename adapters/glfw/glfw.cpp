/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "monitorglfw.hpp"

#include "xng/adapters/glfw/glfw.hpp"

#ifdef BUILD_GLFW_OPENGL

#include "opengl/glfwwindowgl.hpp"

namespace xng::glfw {
    std::unique_ptr<Window> makeWindowGL(const std::string &title,
                                         const Vec2i &size,
                                         WindowAttributes attributes,
                                         Monitor *monitor = nullptr,
                                         VideoMode mode = {},
                                         GLFWwindow *share = nullptr) {
        if (monitor) {
            return std::make_unique<GLFWWindowGL>(title,
                                                  size,
                                                  attributes,
                                                  down_cast<MonitorGLFW &>(*monitor),
                                                  mode,
                                                  share);
        } else {
            return std::make_unique<GLFWWindowGL>(title,
                                                  size,
                                                  attributes,
                                                  share);
        }
    }
}

#else
namespace xng::glfw {
    std::unique_ptr<Window> makeWindowGL(const std::string &title,
                                         const Vec2i &size,
                                         WindowAttributes attributes,
                                         Monitor *monitor = nullptr,
                                         VideoMode mode = {}) {
        throw std::runtime_error("DisplayEnvironment OpenGL backend support not built");
    }
}
#endif

#ifdef BUILD_GLFW_VULKAN

#include "vulkan/glfwwindowvk.hpp"

namespace xng::glfw {
    std::unique_ptr<Window> makeWindowVK(const std::string &title,
                                         const Vec2i &size,
                                         WindowAttributes attributes,
                                         Monitor *monitor = nullptr,
                                         VideoMode mode = {}) {
        if (monitor) {
            return std::make_unique<GLFWWindowVk>(title,
                                                  size,
                                                  attributes,
                                                  down_cast<MonitorGLFW &>(*monitor),
                                                  mode);
        } else {
            return std::make_unique<GLFWWindowVk>(title,
                                                  size,
                                                  attributes);
        }
    }
}

#else
namespace xng::glfw {
    std::unique_ptr<Window> makeWindowVK(const std::string &title,
                                         const Vec2i &size,
                                         WindowAttributes attributes,
                                         Monitor *monitor = nullptr,
                                         VideoMode mode = {}) {
        throw std::runtime_error("DisplayEnvironment Vulkan backend support not built");
    }
}
#endif

namespace xng::glfw {
    std::unique_ptr<Window> DisplayEnvironment::makeWindow(GraphicsAPI api,
                                                           const std::string &title,
                                                           Vec2i size,
                                                           WindowAttributes attributes,
                                                           Monitor *monitor,
                                                           VideoMode mode) const {
        switch (api) {
            case OPENGL_4_6:
                if (primaryWindow == nullptr) {
                    throw std::runtime_error("DisplayEnvironment OpenGL backend support not built");
                }
                return makeWindowGL(title,
                                    size,
                                    attributes,
                                    monitor,
                                    mode,
                                    down_cast<GLFWWindowGL &>(*primaryWindow).windowHandle());
            case VULKAN_1_1:
                return makeWindowVK(title, size, attributes, monitor, mode);
            default:
                throw std::runtime_error("Unsupported gpu backend " + std::to_string(static_cast<int>(api)));
        }
    }

    DisplayEnvironment::DisplayEnvironment() {
        glfwInit();
#ifdef BUILD_GLFW_OPENGL
        auto attr = WindowAttributes();
        attr.visible = false;
        primaryWindow = makeWindowGL("XNG Window", Vec2i(0, 0), attr);
#endif
    }

    DisplayEnvironment::~DisplayEnvironment() {
        glfwTerminate();
    }

    std::unique_ptr<Monitor> DisplayEnvironment::getPrimaryMonitor() {
        return std::make_unique<MonitorGLFW>(glfwGetPrimaryMonitor());
    }

    std::set<std::unique_ptr<Monitor> > DisplayEnvironment::getMonitors() {
        std::set<std::unique_ptr<Monitor> > ret;

        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        for (int i = 0; i < count; i++) {
            ret.insert(std::make_unique<MonitorGLFW>(monitors[i]));
        }

        return ret;
    }

    std::unique_ptr<Window> DisplayEnvironment::createWindow() {
        return makeWindow(gpuBackend, "XNG Window", Vec2i(600, 300), WindowAttributes());
    }

    std::unique_ptr<Window> DisplayEnvironment::createWindow(const std::string &title,
                                                             Vec2i size,
                                                             WindowAttributes attributes) {
        return makeWindow(gpuBackend, title, size, attributes);
    }

    std::unique_ptr<Window> DisplayEnvironment::createWindow(const std::string &title,
                                                             Vec2i size,
                                                             WindowAttributes attributes,
                                                             Monitor &monitor,
                                                             VideoMode mode) {
        return makeWindow(gpuBackend, title, size, attributes, &monitor, mode);
    }

    std::vector<std::string> DisplayEnvironment::getRequiredVulkanExtensions() {
#ifdef BUILD_GLFW_VULKAN
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        return {glfwExtensions, glfwExtensions + glfwExtensionCount};
#else
        return {};
#endif
    }
}
