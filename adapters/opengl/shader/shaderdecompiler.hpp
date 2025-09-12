/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SHADERDECOMPILER_HPP
#define XENGINE_SHADERDECOMPILER_HPP

#include <string>
#include <vector>

#include "shaderlanguage.hpp"
#include "shaderstage.hpp"

namespace xng {
    /**
     * A ShaderDecompiler decompiles SPIRV to the languages defined in ShaderLanguage.
     */
    class XENGINE_EXPORT ShaderDecompiler {
    public:
        /**
         * Decompile the given spirv to the given output language.
         *
         * spirv-cross notes:
         *  Decompiling spirv to hlsl currently is only possible with vertex and fragment shaders.
         *  https://github.com/KhronosGroup/SPIRV-Cross/issues/904
         *
         * @param source The spirv binary
         * @param entryPoint The name of the entry point in the spirv binary
         * @param stage The shader stage that the given entry point represents
         * @param targetLanguage The language to decompile spirv to
         * @return The decompiled spirv
         */
        virtual std::string decompile(const std::vector<uint32_t> &source,
                                      const std::string &entryPoint,
                                      ShaderStage stage,
                                      ShaderLanguage targetLanguage) const = 0;
    };
}

#endif //XENGINE_SHADERDECOMPILER_HPP
