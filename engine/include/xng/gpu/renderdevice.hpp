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

#ifndef XENGINE_RENDERDEVICE_HPP
#define XENGINE_RENDERDEVICE_HPP

#include <map>
#include <functional>

#include "xng/gpu/renderallocator.hpp"
#include "xng/gpu/renderdeviceinfo.hpp"

namespace xng {
    /**
     * The render device allocates gpu resources.
     *
     * Smart pointers are used as return type to ensure that user programs are memory safe by default,
     * the user can unbox the returned pointers to use different memory management patterns.
     */
    class XENGINE_EXPORT RenderDevice {
    public:
        /**
         * Deallocates all objects belonging to this device which have not been deallocated yet.
         * Deallocating objects returned by this device after calling this destructor results in undefined behaviour.
         */
        virtual ~RenderDevice() = default;

        /**
         * @return The information about this device.
         */
        virtual const RenderDeviceInfo &getInfo() = 0;

        /**
         * @return The allocator for this device.
         */
        virtual RenderAllocator &getAllocator() = 0;
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
