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

#ifndef XENGINE_SHADERPROGRAMDESC_HPP
#define XENGINE_SHADERPROGRAMDESC_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include "shader/spirvbundle.hpp"
#include "shader/shaderstage.hpp"

#include "util/hashcombine.hpp"
#include "util/crc.hpp"

namespace xng {
    struct ShaderProgramDesc {
        enum ShaderEnvironment {
            ENV_RENDER,
            ENV_COMPUTE,
            ENV_RAYTRACE
        } environment = ENV_RENDER;

        std::map<ShaderStage, SPIRVShader> shaders;

        bool operator==(const ShaderProgramDesc &other) const {
            return environment == other.environment && shaders == other.shaders;
        }
    };
}

using namespace xng;
namespace std {
    template<>
    struct hash<ShaderProgramDesc> {
        std::size_t operator()(const ShaderProgramDesc &k) const {
            size_t ret = 0;
            hash_combine(ret, k.environment);
            for (auto &e: k.shaders) {
                hash_combine(ret, e.first);
                hash_combine(ret, e.second.getStage());
                hash_combine(ret, e.second.getEntryPoint());
                hash_combine(ret, crc(e.second.getBlob().begin(), e.second.getBlob().end()));
            }
            return ret;
        }
    };
}

#endif //XENGINE_SHADERPROGRAMDESC_HPP
