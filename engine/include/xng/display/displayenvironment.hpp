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

#ifndef XENGINE_DISPLAYENVIRONMENT_HPP
#define XENGINE_DISPLAYENVIRONMENT_HPP

#include "xng/display/window.hpp"
#include "xng/display/windowattributes.hpp"
#include "xng/display/graphicsapi.hpp"

namespace xng {
    class XENGINE_EXPORT DisplayEnvironment {
    public:
        virtual ~DisplayEnvironment() = default;

        virtual std::unique_ptr<Monitor> getPrimaryMonitor() = 0;

        virtual std::set<std::unique_ptr<Monitor>> getMonitors() = 0;

        virtual std::unique_ptr<Window> createWindow(GraphicsAPI api) = 0;

        virtual std::unique_ptr<Window> createWindow(GraphicsAPI api,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes) = 0;

        /**
         *
         * @param api The graphics api that the returned window should be compatible with.
         * @param title
         * @param size
         * @param attributes
         * @param monitor
         * @param mode
         * @return
         */
        virtual std::unique_ptr<Window> createWindow(GraphicsAPI api,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes,
                                                     Monitor &monitor,
                                                     VideoMode mode) = 0;

        /**
         * Returns the required vulkan extensions for creating surfaces for the window.
         *
         * @return
         */
        virtual std::vector<const char *> getRequiredVulkanExtensions() = 0;
    };
}

#endif //XENGINE_DISPLAYENVIRONMENT_HPP
