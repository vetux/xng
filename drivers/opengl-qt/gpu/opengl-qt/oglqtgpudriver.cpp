/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "gpu/opengl-qt/oglqtgpudriver.hpp"

#define OPENGL_BUILD_QT

#include "gpu/opengl/oglrenderdevice.hpp"

#include "driver/registerdriver.hpp"

namespace xng {
    static bool dr = REGISTER_DRIVER("opengl-qt", GpuDriver, OGLQtGpuDriver);

    std::vector<RenderDeviceInfo> OGLQtGpuDriver::getAvailableRenderDevices() {
        return {{.name = "default"}};
    }

    std::unique_ptr<RenderDevice> OGLQtGpuDriver::createRenderDevice() {
        return std::make_unique<opengl::OGLQtRenderDevice>();
    }

    std::unique_ptr<RenderDevice> OGLQtGpuDriver::createRenderDevice(const std::string &deviceName) {
        return std::make_unique<opengl::OGLQtRenderDevice>();
    }

    std::type_index OGLQtGpuDriver::getType() {
        return typeid(OGLQtGpuDriver);
    }
}