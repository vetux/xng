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

#include "graphics/opengl-qt/oglqtgraphicsdriver.hpp"

#define OPENGL_BUILD_QT

#include "graphics/opengl/oglrenderdevice.hpp"

#include "driver/drivermacro.hpp"

namespace xng {
    static bool dr = REGISTER_DRIVER("opengl-qt", OGLQtGraphicsDriver);

    std::vector<RenderDeviceInfo> OGLQtGraphicsDriver::getAvailableRenderDevices() {
        return {{.name = "default"}};
    }

    std::unique_ptr<RenderDevice> OGLQtGraphicsDriver::createRenderDevice() {
        return std::make_unique<opengl::OGLQtRenderDevice>();
    }

    std::unique_ptr<RenderDevice> OGLQtGraphicsDriver::createRenderDevice(const std::string &deviceName) {
        return std::make_unique<opengl::OGLQtRenderDevice>();
    }

    std::type_index OGLQtGraphicsDriver::getType() {
        return typeid(OGLQtGraphicsDriver);
    }
}