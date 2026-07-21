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

#ifndef XENGINE_SHADERBLOB_HPP
#define XENGINE_SHADERBLOB_HPP

#include "xng/rendergraph/shader/shader.hpp"

namespace xng {
    /**
     * TODO: Offline Shader Compilation (Replace string based binding model with explicit binding points) / Implement separate descriptor set support
     *
     * To allow offline compilation and decouple the shader IR from the hardware interface,
     * the shader IR will be compiled to implementation-dependent blobs.
     *
     * This will cleanup the runtime overhead of the shader IR and also allows support for graphics platforms
     * where shader compilation must happen offline (Console Devkits) and eliminates the need to ship shader IR
     * to end users (IP Protection, Shipping size).
     *
     * These blobs must be in a format supported by the implementation when passing them to the pipeline cache.
     */
    struct ShaderBlob {
        /**
         * The shader in implementation dependent format.
         *
         * E.g. glsl on gl or spirv on vulkan
         */
        std::vector<uint8_t> shader;
    };
}

#endif //XENGINE_SHADERBLOB_HPP