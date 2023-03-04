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

#ifndef XENGINE_RENDERDEVICEINFO_HPP
#define XENGINE_RENDERDEVICEINFO_HPP

#include <string>

#include "xng/gpu/renderdevicecapability.hpp"

namespace xng {
    /**
     * A render device info presents information about a physical gpu device available on the system.
     * The user can then create a RenderDevice instance from a physical device identifier which then represents a logical gpu device.
     */
    struct RenderDeviceInfo {
        std::string name; // The unique identifier of this physical render device

        int maxSampleCount = 0; // The maximum supported amount of msaa samples per texture or render target. 0 if msaa is not supported.

        // The total amount of video memory available on the device in bytes
        // (E.g. in Vulkan The sum of the device memory pool sizes excluding System (CPU) pools),
        // or 0 if the platform does not support querying for it.
        // All RenderDevice instances for this name share the memory.
        size_t availableDeviceMemory = 0;

        std::set<RenderDeviceCapability> capabilities;
    };
}

#endif //XENGINE_RENDERDEVICEINFO_HPP
