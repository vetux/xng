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

#ifndef XENGINE_DISPLAYMANAGER_HPP
#define XENGINE_DISPLAYMANAGER_HPP

#include <set>

#include "display/displaybackend.hpp"
#include "display/window.hpp"
#include "display/monitor.hpp"

namespace xengine {
    //TODO: Add monitor gamma configuration interface
    class XENGINE_EXPORT DisplayManager {
    public:
        DisplayManager();

        explicit DisplayManager(DisplayBackend backend);

        ~DisplayManager();

        std::unique_ptr<Monitor> getPrimaryMonitor() const;

        std::set<std::unique_ptr<Monitor>> getMonitors() const;

        std::unique_ptr<Window> createWindow() const;

        std::unique_ptr<Window> createWindow(const std::string &title, Vec2i size, WindowAttributes attributes) const;

        std::unique_ptr<Window> createWindow(const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes,
                                             Monitor &monitor,
                                             VideoMode mode) const;

        DisplayBackend getBackend() const;

    private:
        DisplayBackend backend;
    };
}
#endif //XENGINE_DISPLAYMANAGER_HPP
