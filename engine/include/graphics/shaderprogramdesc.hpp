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

#ifndef XENGINE_SHADERPROGRAMDESC_HPP
#define XENGINE_SHADERPROGRAMDESC_HPP

#include <map>
#include <vector>
#include <string>

#include "graphics/shader/spirvbuffer.hpp"
#include "graphics/shader/shaderstage.hpp"

namespace xengine {
    /**
     * The shaders are required to store all global variables in uniform buffers.
     * The bindings of uniform buffers and samplers have to be specified in the source.
     *
     *  The shaders specify the layout of the uniform buffers and users have to ensure that
     *  the layout of data the shader buffers matches the layout specified in the shaders.
     *
     *  The shaders specify the layout of the vertex input data and users have to ensure that
     *  the layout of the mesh buffers matches the layout specified in the vertex shader.
     */
    struct ShaderProgramDesc {
        struct ShaderEntry {
            std::string entryPoint; // The name of the entry point.
            size_t bufferIndex; // The index into the buffers vector of the buffer containing the entry point.
        };
        std::map<ShaderStage, ShaderEntry> entries;
        std::vector<SPIRVBuffer> buffers;

        bool operator==(const ShaderProgramDesc &other) const {
            return entries == other.entries && buffers == other.buffers;
        }
    };
}

using namespace xengine;
namespace std {
    template<>
    struct hash<ShaderProgramDesc> {
        std::size_t operator()(const ShaderProgramDesc &k) const {
            size_t ret = 0;
            for (auto &e: k.entries) {
                hash_combine(ret, e.second.entryPoint);
                hash_combine(ret, e.second.bufferIndex);
            }
            for (auto &e: k.buffers) {
                hash_combine(ret, e.blob);
            }
            return ret;
        }
    };
}

#endif //XENGINE_SHADERPROGRAMDESC_HPP
