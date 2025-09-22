/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SHADERCOMPILER_HPP
#define XENGINE_SHADERCOMPILER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include "shader/shaderlanguage.hpp"
#include "shader/shaderstage.hpp"
#include "shader/shaderenvironment.hpp"

namespace xng {
    /**
     * A ShaderCompiler preprocesses and compiles shader source from the languages defined in ShaderLanguage to SPIRV
     */
    class XENGINE_EXPORT ShaderCompiler {
    public:
        enum OptimizationLevel {
            OPTIMIZATION_NONE,
            OPTIMIZATION_PERFORMANCE,
            OPTIMIZATION_SIZE
        };

        /**
         * Compile the given source to spirv.
         *
         * shaderc notes:
         *  Beware that when compiling HLSL TO SPIRV the shaderc implementation merges
         *  all global variables into a single uniform buffer.
         *
         * @param source
         * @param entryPoint
         * @param stage
         * @param language
         * @return
         */
        virtual std::vector<uint32_t> compile(const std::string &source,
                                              const std::string &entryPoint,
                                              ShaderStageType stage,
                                              ShaderLanguage language,
                                              OptimizationLevel optimizationLevel,
                                              ShaderEnvironment environment) const = 0;

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
        virtual std::string preprocess(const std::string &source,
                                       ShaderStageType stage,
                                       ShaderLanguage language,
                                       const std::function<std::string(const char *)> &include,
                                       const std::map<std::string, std::string> &macros,
                                       OptimizationLevel optimizationLevel,
                                       ShaderEnvironment environment) const = 0;
    };
}

#endif //XENGINE_SHADERCOMPILER_HPP
