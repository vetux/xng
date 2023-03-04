/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_SHADERLANGUAGE_HPP
#define XENGINE_SHADERLANGUAGE_HPP

namespace xng {
    enum ShaderLanguage : int {
        HLSL_SHADER_MODEL_4,
        GLSL_460, //GLSL 4.6 with OpenGL semantics
        GLSL_460_VK, //GLSL 4.6 with Vulkan semantics
        GLSL_420, //GLSL 4.2 with OpenGL semantics
        GLSL_420_VK, //GLSL 4.2 with Vulkan semantics
        GLSL_ES_320 //GLSL 3.2 ES with OpenGL semantics
    };
}

#endif //XENGINE_SHADERLANGUAGE_HPP
