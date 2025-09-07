/**
*  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FGSHADERDECOMPILER_HPP
#define XENGINE_FGSHADERDECOMPILER_HPP

#include <vector>

#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    /**
     * The shader decompiler builds a shader graph from the given shader source in some language.
     *
     * This is pretty complex to implement, so this would be something for when I have a custom shading language
     * for the engine.
     */
    class FGShaderDecompiler {
    public:
        virtual ~FGShaderDecompiler() = default;

        virtual FGShaderSource decompile(const std::string &shader) = 0;
    };
}

#endif //XENGINE_FGSHADERDECOMPILER_HPP
