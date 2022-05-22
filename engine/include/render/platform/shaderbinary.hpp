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

#ifndef XENGINE_SHADERBINARY_HPP
#define XENGINE_SHADERBINARY_HPP

namespace xengine {
    /**
     * A platform and device specific compiled and linked shader program in binary form.
     *
     * Eg. glProgramBinary in opengl or
     * the blob returned by ID3D11Linker::Link
     *
     * ID3D11Linker::Link does not document if it is possible to reuse the shader blob as it can be in opengl.
     * I assume it works identically.
     *
     * Preferably individual shaders should get compiled and those binaries stored because linking is cheap,
     * but opengl does not offer a way to retrieve the binary for only a single shader.
     *
     * Users can also pass SPIRV to the render allocator and if the platform supports SPIRV directly no additional
     * cross compile happens.
     */
    struct ShaderBinary {
        unsigned int token; // Platform specific identification token
        std::vector<char> buffer; // The shader binary
    };
}
#endif //XENGINE_SHADERBINARY_HPP
