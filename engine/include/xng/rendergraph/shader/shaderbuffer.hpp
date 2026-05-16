/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERGRAPH_SHADERBUFFER_HPP
#define XENGINE_RENDERGRAPH_SHADERBUFFER_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng::rg {
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
        // If the type is a shader struct type name, the buffer is read in std140 layout.
        // If the type is a primitive, the buffer is read in std430 layout.
        ShaderDataType type;

        ShaderBuffer() = default;

        ShaderBuffer(const bool read_only, const bool dynamic, ShaderDataType _type)
            : readOnly(read_only),
              dynamic(dynamic),
              type(std::move(_type)) {
        }

        bool operator==(const ShaderBuffer& other) const
        {
            return readOnly == other.readOnly && dynamic == other.dynamic && type == other.type;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_SHADERBUFFER_HPP
