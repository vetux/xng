/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "gpu/opengl/oglgpudriver.hpp"
#include "gpu/opengl/oglrenderdevice.hpp"

namespace xng::opengl {
    const std::vector<RenderDeviceInfo> &OGLGpuDriver::getAvailableRenderDevices() {
        if (!retrievedMaxSamples) {
            retrievedMaxSamples = true;
            glGetIntegerv(GL_MAX_SAMPLES, &deviceInfos.at(0).maxSampleCount);
        }
        return deviceInfos;
    }

    std::unique_ptr<RenderDevice> OGLGpuDriver::createRenderDevice() {
        return std::make_unique<opengl::OGLRenderDevice>(getAvailableRenderDevices().at(0));
    }

    std::unique_ptr<RenderDevice> OGLGpuDriver::createRenderDevice(const std::string &deviceName) {
        return std::make_unique<opengl::OGLRenderDevice>(getAvailableRenderDevices().at(0));
    }

    std::type_index OGLGpuDriver::getType() {
        return typeid(OGLGpuDriver);
    }

    std::set<GpuFeature> OGLGpuDriver::getSupportedFeatures() {
        return {};
    }
}