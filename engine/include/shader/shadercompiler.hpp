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

#ifndef XENGINE_SHADERCOMPILER_HPP
#define XENGINE_SHADERCOMPILER_HPP

#include <map>
#include <string>
#include <functional>
#include <stdexcept>

#include "shader/shaderlanguage.hpp"
#include "shader/shaderstage.hpp"

namespace xengine {
    namespace ShaderCompiler {
        //TODO: Shader uniform buffer support

        enum OptimizationLevel {
            OPTIMIZATION_NONE,
            OPTIMIZATION_PERFORMANCE,
            OPTIMIZATION_SIZE
        };

        /**
         * Compile the given source to spirv.
         *
         * BEWARE that when compiling HLSL TO SPIRV the implementation currently uses shaderc which merges
         * all global variables into a single uniform buffer in the resulting spirv module for some elusive reason.
         *
         * Compiling GLSL TO SPIRV should work without issues.
         *
         * @param source
         * @param entryPoint
         * @param stage
         * @param language
         * @return
         */
        std::vector<uint32_t> compileToSPIRV(const std::string &source,
                                             const std::string &entryPoint,
                                             ShaderStage stage,
                                             ShaderLanguage language,
                                             OptimizationLevel optimizationLevel = OPTIMIZATION_NONE);

        /**
         * Decompile the given spirv to the given output language.
         *
         * Decompiling spirv to hlsl currently is only possible with vertex and fragment shaders.
         * https://github.com/KhronosGroup/SPIRV-Cross/issues/904
         *
         * @param source The spirv binary
         * @param entryPoint The name of the entry point in the spirv binary
         * @param stage The shader stage that the given entry point represents
         * @param targetLanguage The language to decompile spirv to
         * @return The decompiled spirv
         */
        std::string decompileSPIRV(const std::vector<uint32_t> &source,
                                   const std::string &entryPoint,
                                   ShaderStage stage,
                                   ShaderLanguage targetLanguage);

        /**
         * Preprocess the given source
         *
         * @param source
         * @param stage
         * @param language
         * @param include
         * @param macros
         * @param optimizationLevel
         * @return
         */
        std::string preprocess(const std::string &source,
                               ShaderStage stage,
                               ShaderLanguage language,
                               const std::function<std::string(const char *)> &include = {},
                               const std::map<std::string, std::string> &macros = {},
                               OptimizationLevel optimizationLevel = OPTIMIZATION_NONE);

        /**
         * Cross compile the source by using spirv as an intermediate.
         *
         * @param source
         * @param entryPoint
         * @param stage
         * @param language
         * @param targetLanguage
         * @return
         */
        inline std::string crossCompile(const std::string &source,
                                 const std::string &entryPoint,
                                 ShaderStage stage,
                                 ShaderLanguage sourceLanguage,
                                 ShaderLanguage targetLanguage,
                                 OptimizationLevel optimizationLevel = OPTIMIZATION_NONE) {
            return decompileSPIRV(compileToSPIRV(source,
                                                 entryPoint,
                                                 stage,
                                                 sourceLanguage,
                                                 optimizationLevel),
                                  entryPoint,
                                  stage,
                                  targetLanguage);
        }
    }
}

#endif //XENGINE_SHADERCOMPILER_HPP
