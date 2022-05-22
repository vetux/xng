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

#include "display/displaymanager.hpp"

#ifdef DRIVER_GLFW

#include "display/glfw/glfwdisplay.hpp"

#endif

namespace xengine {
    DisplayManager::DisplayManager() = default;

    DisplayManager::DisplayManager(DisplayBackend backend) : backend(backend) {}

    DisplayManager::~DisplayManager() = default;

    std::unique_ptr<Monitor> DisplayManager::getPrimaryMonitor() const {
        switch (backend) {
#ifdef DRIVER_GLFW
            case GLFW:
                return glfw::getPrimaryMonitor();
#endif
            default:
                throw std::runtime_error("Unsupported display api");
        }
    }

    std::set<std::unique_ptr<Monitor>> DisplayManager::getMonitors() const {
        switch (backend) {
#ifdef DRIVER_GLFW
            case GLFW:
                return glfw::getMonitors();
#endif
            default:
                throw std::runtime_error("Unsupported display api");
        }
    }

    std::unique_ptr<Window> DisplayManager::createWindow(RenderPlatform platform) const {
        switch (backend) {
#ifdef DRIVER_GLFW
            case GLFW:
                return glfw::createWindow(platform);
#endif
            default:
                throw std::runtime_error("Unsupported display api");
        }
    }

    std::unique_ptr<Window>
    DisplayManager::createWindow(RenderPlatform platform, const std::string &title, Vec2i size,
                                 WindowAttributes attributes) const {
        switch (backend) {
#ifdef DRIVER_GLFW
            case GLFW:
                return glfw::createWindow(platform, title, size, attributes);
#endif
            default:
                throw std::runtime_error("Unsupported display api");
        }
    }

    std::unique_ptr<Window> DisplayManager::createWindow(RenderPlatform platform,
                                                         const std::string &title, Vec2i size,
                                                         WindowAttributes attributes,
                                                         Monitor &monitor,
                                                         VideoMode mode) const {
        switch (backend) {
#ifdef DRIVER_GLFW
            case GLFW:
                return glfw::createWindow(platform, title, size, attributes, monitor, mode);
#endif
            default:
                throw std::runtime_error("Unsupported display api");
        }
    }

    DisplayBackend DisplayManager::getBackend() const {
        return backend;
    }
}