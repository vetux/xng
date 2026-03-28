/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_WINDOWVK_HPP
#define XENGINE_WINDOWVK_HPP

#include <vulkan/vulkan.h>

namespace xng {
    /**
     * The glfw display adapter implements this interface for VULKAN_1_1 windows.
     *
     * The vulkan adapter depends on this interface.
     * The window passed to vulkan::Runtime.setWindow must implement this interface.
     */
    class WindowVk {
    public:
        virtual ~WindowVk() = default;

        /**
         * @param instance The instance in which to create the surface.
         * @param allocator Optional allocator to create the surface for.
         * @return The created surface.
         */
        virtual VkSurfaceKHR createSurface(VkInstance instance, VkAllocationCallbacks *allocator) = 0;
    };
}

#endif //XENGINE_WINDOWVK_HPP