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

#include "xng/adapters/opengl/opengl.hpp"

#include "gpu/opengl/oglrenderdevice.hpp"

#include "glad/glad.h"

namespace xng::opengl {
    std::vector<RenderDeviceInfo> OpenGL::getAvailableRenderDevices() {
        if (!retrievedInfos) {
            retrievedInfos = true;
            glGetIntegerv(GL_MAX_SAMPLES, &deviceInfos.at(0).maxSampleCount);
            const GLubyte *vendor = glGetString(GL_VENDOR);
            const GLubyte *renderer = glGetString(GL_RENDERER);
            const GLubyte *version = glGetString(GL_VERSION);
            deviceInfos.at(0).renderer = std::string(reinterpret_cast<const char *>(renderer));
            deviceInfos.at(0).vendor = std::string(reinterpret_cast<const char *>(vendor));
            deviceInfos.at(0).version = std::string(reinterpret_cast<const char *>(version));
            deviceInfos.at(0).capabilities.insert(CAPABILITY_BASE_VERTEX);
            deviceInfos.at(0).capabilities.insert(CAPABILITY_INSTANCING);
            deviceInfos.at(0).capabilities.insert(CAPABILITY_MULTI_DRAW);
            GLint tmp;
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &tmp);
            deviceInfos.at(0).uniformBufferMaxSize = tmp;
            glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &tmp);
            deviceInfos.at(0).storageBufferMaxSize = tmp;
        }
        return deviceInfos;
    }

    std::unique_ptr<RenderDevice> OpenGL::createRenderDevice() {
        return std::make_unique<opengl::OGLRenderDevice>(getAvailableRenderDevices().at(0));
    }

    std::unique_ptr<RenderDevice> OpenGL::createRenderDevice(const std::string &deviceName) {
        return std::make_unique<opengl::OGLRenderDevice>(getAvailableRenderDevices().at(0));
    }

    GpuEngineBackend OpenGL::getBackend() {
        return OPENGL_4_6;
    }
}