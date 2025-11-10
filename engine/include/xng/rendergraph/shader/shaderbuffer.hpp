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

#ifndef XENGINE_SHADERBUFFER_HPP
#define XENGINE_SHADERBUFFER_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    /**
     * A shader buffer can either be dynamic or static.
     *
     * Shaders can access dynamic buffers without knowing the exact count of the elements this is useful
     * for dynamic data such as lights.
     */
    struct ShaderBuffer {
        bool readOnly = true; // Whether shaders are allowed to write to the buffer

        // If true, this buffer is a dynamic buffer and elements can be accessed by subscripting the buffer object
        bool dynamic = false;

        // The type name of the structure defining the contents of the buffer. For dynamic buffers an instance of the structure is created for every element.
        ShaderStructType typeName;

        ShaderBuffer() = default;

        ShaderBuffer(const bool read_only, const bool dynamic, ShaderStructType type_name)
            : readOnly(read_only),
              dynamic(dynamic),
              typeName(std::move(type_name)) {
        }
    };
}

#endif //XENGINE_SHADERBUFFER_HPP
