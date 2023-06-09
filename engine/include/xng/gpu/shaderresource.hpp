/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_SHADERBINDING_HPP
#define XENGINE_SHADERBINDING_HPP

#include <set>

#include "xng/shader/shaderstage.hpp"

namespace xng {
    class TextureBuffer;

    class TextureArrayBuffer;

    class ShaderUniformBuffer;

    class ShaderStorageBuffer;

    typedef std::variant <
            std::reference_wrapper<TextureBuffer>,
            std::reference_wrapper<TextureArrayBuffer>,
            std::reference_wrapper<ShaderUniformBuffer>,
            std::reference_wrapper<ShaderStorageBuffer>
    > ShaderData;

    enum ShaderDataType : int {
        SHADER_TEXTURE_BUFFER = 0,
        SHADER_TEXTURE_ARRAY_BUFFER,
        SHADER_UNIFORM_BUFFER,
        SHADER_STORAGE_BUFFER
    };

    /**
     * A shader resource specifies the data bound to a index in the shader bindings array and
     * the pipeline stages that access the resource and the type of access.
     */
    struct ShaderResource {
        enum AccessMode {
            READ,
            WRITE,
            READ_WRITE
        };

        ShaderData data;
        std::map<ShaderStage, AccessMode> stages;
    };
}
#endif //XENGINE_SHADERBINDING_HPP
