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

#ifndef XENGINE_DISPLAYDRIVER_HPP
#define XENGINE_DISPLAYDRIVER_HPP

#include "xng/driver/driver.hpp"

#include "xng/gpu/gpudriver.hpp"

#include "xng/display/window.hpp"
#include "xng/display/displaydriverbackend.hpp"

namespace xng {
    class XENGINE_EXPORT DisplayDriver : public Driver {
    public:
        static std::string getBackendName(DisplayDriverBackend backend){
            switch(backend){
                case DISPLAY_GLFW:
                    return "glfw";
                default:
                    throw std::runtime_error("Invalid backend");
            }
        }

        static std::unique_ptr<DisplayDriver> load(DisplayDriverBackend backend) {
            return std::unique_ptr<DisplayDriver>(
                    dynamic_cast<DisplayDriver *>(Driver::load(getBackendName(DISPLAY_GLFW)).release()));
        }

        virtual std::unique_ptr<Monitor> getPrimaryMonitor() = 0;

        virtual std::set<std::unique_ptr<Monitor>> getMonitors() = 0;

        virtual std::unique_ptr<Window> createWindow(GpuDriverBackend gpuBackend) = 0;

        virtual std::unique_ptr<Window> createWindow(GpuDriverBackend gpuBackend,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes) = 0;

        virtual std::unique_ptr<Window> createWindow(GpuDriverBackend gpuBackend,
                                                     const std::string &title,
                                                     Vec2i size,
                                                     WindowAttributes attributes,
                                                     Monitor &monitor,
                                                     VideoMode mode) = 0;

    private:
        std::type_index getBaseType() override {
            return typeid(DisplayDriver);
        }
    };
}
#endif //XENGINE_DISPLAYDRIVER_HPP
