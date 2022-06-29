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

#include "graphics/opengl/oglgraphicsdriver.hpp"
#include "graphics/opengl/oglrenderdevice.hpp"

#include "driver/drivermanager.hpp"

namespace xng::opengl {
    static bool dr = REGISTER_DRIVER("opengl", OGLGraphicsDriver);

    std::vector<RenderDeviceInfo> OGLGraphicsDriver::getAvailableRenderDevices() {
        return {{.name = "default"}};
    }

    std::unique_ptr<RenderDevice> OGLGraphicsDriver::createRenderDevice() {
        return std::make_unique<opengl::OGLRenderDevice>();
    }

    std::unique_ptr<RenderDevice> OGLGraphicsDriver::createRenderDevice(const std::string &deviceName) {
        return std::make_unique<opengl::OGLRenderDevice>();
    }

    std::type_index OGLGraphicsDriver::getType() {
        return typeid(OGLGraphicsDriver);
    }
}