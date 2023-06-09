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

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

namespace xng {
    class XENGINE_EXPORT RenderObject {
    public:
        enum Type {
            RENDER_OBJECT_MEMORY,
            RENDER_OBJECT_VERTEX_BUFFER,
            RENDER_OBJECT_VERTEX_ARRAY_OBJECT,
            RENDER_OBJECT_TEXTURE_BUFFER,
            RENDER_OBJECT_TEXTURE_ARRAY_BUFFER,
            RENDER_OBJECT_SHADER_UNIFORM_BUFFER,
            RENDER_OBJECT_SHADER_STORAGE_BUFFER,
            RENDER_OBJECT_INDEX_BUFFER,
            RENDER_OBJECT_RENDER_TARGET,
            RENDER_OBJECT_RENDER_PIPELINE,
            RENDER_OBJECT_COMPUTE_PIPELINE,
            RENDER_OBJECT_RAYTRACE_PIPELINE,
            RENDER_OBJECT_RENDER_PASS,
            RENDER_OBJECT_COMMAND_QUEUE,
            RENDER_OBJECT_COMMAND_BUFFER,
            RENDER_OBJECT_SEMAPHORE,
            RENDER_OBJECT_FENCE,
        };

        /**
         * Calling this destructor deallocates all memory associated with the object
         * and removes the object from the device objects.
         */
        virtual ~RenderObject() = default;

        virtual Type getType() = 0;
    };
}

#endif //XENGINE_RENDEROBJECT_HPP
