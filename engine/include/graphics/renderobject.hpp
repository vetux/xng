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

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

namespace xng {
    class RenderObject {
    public:
        enum Type {
            VERTEX_BUFFER,
            TEXTURE_BUFFER,
            SHADER_BUFFER,
            SHADER_PROGRAM,
            RENDER_TARGET,
            RENDER_PIPELINE
        };

        virtual ~RenderObject() = default;

        virtual Type getType() = 0;

        /**
         * Pin the render object to the implementations fastest memory type.
         * If the implementation supports different memory types
         * it shall move the object's data to the fastest memory while it is pinned. (Eg. Staging buffer in Vulkan)
         * In render pipeline executions the implementation ensures that the objects data is accessible somehow regardless if its pinned or not.
         */
        virtual void pinGpuMemory() = 0;

        virtual void unpinGpuMemory() = 0;
    };
}

#endif //XENGINE_RENDEROBJECT_HPP
