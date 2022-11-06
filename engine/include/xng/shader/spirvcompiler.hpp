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

#ifndef XENGINE_SPIRVCOMPILER_HPP
#define XENGINE_SPIRVCOMPILER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include "xng/driver/driver.hpp"

#include "shaderlanguage.hpp"
#include "shaderstage.hpp"

namespace xng {
    enum SPIRVCompilerBackend {
        SHADERC
    };

    /**
     * A SPIRVCompiler preprocesses and compiles shader source from the languages defined in ShaderLanguage to SPIRV
     */
    class XENGINE_EXPORT SPIRVCompiler : public Driver {
    public:
        static std::unique_ptr<SPIRVCompiler> load(SPIRVCompilerBackend backend) {
            switch (backend) {
                case SHADERC:
                    return std::unique_ptr<SPIRVCompiler>(
                            dynamic_cast<SPIRVCompiler *>(Driver::load("shaderc").release()));
            }
            throw std::runtime_error("Invalid backend");
        }

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
                                              ShaderStage stage,
                                              ShaderLanguage language,
                                              OptimizationLevel optimizationLevel) const = 0;

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
                                       ShaderStage stage,
                                       ShaderLanguage language,
                                       const std::function<std::string(const char *)> &include,
                                       const std::map<std::string, std::string> &macros,
                                       OptimizationLevel optimizationLevel) const = 0;

        std::type_index getBaseType() override {
            return typeid(SPIRVCompiler);
        }
    };
}

#endif //XENGINE_SPIRVCOMPILER_HPP
