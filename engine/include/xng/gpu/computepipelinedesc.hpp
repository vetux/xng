/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_COMPUTEPIPELINEDESC_HPP
#define XENGINE_COMPUTEPIPELINEDESC_HPP

#include "xng/shader/shaderstage.hpp"
#include "xng/shader/spirvshader.hpp"

#include "xng/gpu/shaderresource.hpp"

namespace xng {
    struct ComputePipelineDesc {
        std::map<ShaderStage, SPIRVShader> shaders; // The shaders to use for this pipeline
        std::vector<RenderPipelineBindingType> bindings; // The set of binding types defining how shader resources are bound

        bool operator==(const ComputePipelineDesc &other) const {
            return true;
        }
    };
}

#endif //XENGINE_COMPUTEPIPELINEDESC_HPP
