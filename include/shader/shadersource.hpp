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

#ifndef XENGINE_SHADERSOURCE_HPP
#define XENGINE_SHADERSOURCE_HPP

#include "shader/shadercompiler.hpp"
#include "shader/spirvsource.hpp"

namespace xengine {
    class XENGINE_EXPORT ShaderSource {
    public:
        ShaderSource() = default;

        ShaderSource(std::string src,
                     std::string entryPoint,
                     ShaderStage stage,
                     ShaderLanguage language,
                     bool preprocessed = false);

        void preprocess(const std::function<std::string(const char *)> &include = {},
                        const std::map<std::string, std::string> &macros = {},
                        ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE);

        void crossCompile(ShaderLanguage targetLanguage,
                          ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE);

        SPIRVSource compile(ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE);

        const std::string &getSrc() const;

        const std::string &getEntryPoint() const;

        ShaderStage getStage() const;

        ShaderLanguage getLanguage() const;

        bool isPreProcessed() const;

        bool isEmpty() const { return src.empty(); }

    private:
        std::string src{};

        std::string entryPoint{};
        ShaderStage stage{};
        ShaderLanguage language{};
        bool preprocessed{};
    };
}

#endif //XENGINE_SHADERSOURCE_HPP
