/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_VULKAN_HPP
#define XENGINE_VULKAN_HPP

#include "xng/gpu/gpuengine.hpp"

#include "xng/display/displayenvironment.hpp"

namespace xng::vulkan {
    class XENGINE_EXPORT Vulkan : public GpuEngine {
    public:
        struct InstanceData {
            virtual ~InstanceData() = default;
        };

        Vulkan(DisplayEnvironment &displayDriver);

        ~Vulkan();

        std::vector<RenderDeviceInfo> getAvailableRenderDevices() override;

        std::unique_ptr<RenderDevice> createRenderDevice() override;

        std::unique_ptr<RenderDevice> createRenderDevice(const std::string &deviceName) override;

        GpuEngineBackend getBackend() override;

    private:
        std::shared_ptr<InstanceData> instanceData;
        std::vector<RenderDeviceInfo> deviceInfos;
    };
}

#endif //XENGINE_VULKAN_HPP
