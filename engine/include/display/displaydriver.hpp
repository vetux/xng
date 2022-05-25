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

#ifndef XENGINE_DISPLAYDRIVER_HPP
#define XENGINE_DISPLAYDRIVER_HPP

#include "driver/driver.hpp"
#include "display/window.hpp"

namespace xengine {
    class XENGINE_EXPORT DisplayDriver : public Driver {
    public:
        virtual std::unique_ptr<Monitor> getPrimaryMonitor() = 0;

        virtual std::set<std::unique_ptr<Monitor>> getMonitors() = 0;

        virtual std::unique_ptr<Window> createWindow(const std::string &graphicsDriver) = 0;

        virtual std::unique_ptr<Window> createWindow(const std::string &graphicsDriver,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes) = 0;

        virtual std::unique_ptr<Window> createWindow(const std::string &graphicsDriver,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes,
                                                     Monitor &monitor,
                                                     VideoMode mode) = 0;
    };
}
#endif //XENGINE_DISPLAYDRIVER_HPP
