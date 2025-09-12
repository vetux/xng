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

#ifndef XENGINE_ANDROID_HPP
#define XENGINE_ANDROID_HPP

// MSVC, for some reason, references the getWindow method when this header is included even if the Android type is not used by user code.
#ifndef WIN32

#include "xng/display/displayenvironment.hpp"

#include "androidapp.hpp"

namespace xng::android {
    class XENGINE_EXPORT Android : public DisplayEnvironment {
    public:
        Android();

        Android(Android &other) = delete;

        std::unique_ptr<Monitor> getPrimaryMonitor() override {
            throw std::runtime_error("No monitors support on android");
        }

        std::set<std::unique_ptr<Monitor>> getMonitors() override {
            throw std::runtime_error("No monitors support on android");
        }

        std::unique_ptr<Window> createWindow(GraphicsAPI gpuBackend) override {
            return getWindow(gpuBackend, {});
        }

        std::unique_ptr<Window> createWindow(GraphicsAPI gpuBackend,
                                             const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes) override {
            return getWindow(gpuBackend, attributes);
        }

        std::unique_ptr<Window> createWindow(GraphicsAPI gpuBackend,
                                             const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes,
                                             Monitor &monitor,
                                             VideoMode mode) override {
            return getWindow(gpuBackend, attributes);
        }

        android_app *app;

    private:
        std::unique_ptr<Window> getWindow(GraphicsAPI gpuDriverBackend, WindowAttributes attributes);
    };
}

#endif

#endif //XENGINE_ANDROID_HPP
