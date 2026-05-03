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

#ifndef XENGINE_DISPLAYENVIRONMENT_HPP
#define XENGINE_DISPLAYENVIRONMENT_HPP

#include <memory>

#include "xng/display/window.hpp"
#include "xng/display/windowattributes.hpp"

namespace xng {
    class XENGINE_EXPORT DisplayEnvironment {
    public:
        virtual ~DisplayEnvironment() = default;

        virtual std::unique_ptr<Monitor> getPrimaryMonitor() = 0;

        virtual std::set<std::unique_ptr<Monitor>> getMonitors() = 0;

        virtual std::unique_ptr<Window> createWindow() = 0;

        virtual std::unique_ptr<Window> createWindow(const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes) = 0;

        virtual std::unique_ptr<Window> createWindow(const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes,
                                                     Monitor &monitor,
                                                     VideoMode mode) = 0;
    };
}

#endif //XENGINE_DISPLAYENVIRONMENT_HPP
