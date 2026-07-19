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

#ifndef XENGINE_SHADERUNIFORMBUFFER_HPP
#define XENGINE_SHADERUNIFORMBUFFER_HPP


#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng::rg {
    /**
     * A uniform buffer has a fixed element count.
     */
    struct ShaderUniformBuffer {
        // If the type is a shader struct type name, the buffer is read in std140 layout.
        // If the type is a primitive, the buffer is read in std430 layout.
        ShaderDataType type;

        ShaderUniformBuffer() = default;

        explicit ShaderUniformBuffer(ShaderDataType _type)
            : type(std::move(_type)) {
        }

        bool operator==(const ShaderUniformBuffer &other) const {
            return type == other.type;
        }
    };
}

#endif //XENGINE_SHADERUNIFORMBUFFER_HPP
