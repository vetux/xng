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

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

namespace xng {
    class XENGINE_EXPORT RenderObject {
    public:
        enum Type {
            VERTEX_BUFFER,
            TEXTURE_BUFFER,
            SHADER_BUFFER,
            SHADER_PROGRAM,
            RENDER_TARGET,
            RENDER_PIPELINE,
            COMPUTE_PIPELINE,
            RAYTRACE_PIPELINE
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
