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
#include "shader/spirvbundle.hpp"

namespace xng {
    /**
     * An object oriented wrapper around the shader compiler.
     */
    class XENGINE_EXPORT ShaderSource {
    public:
        ShaderSource() = default;

        ShaderSource(std::string src,
                     std::string entryPoint,
                     ShaderStage stage,
                     ShaderLanguage language,
                     bool preprocessed)
                : src(std::move(src)),
                  entryPoint(std::move(entryPoint)),
                  stage(stage),
                  language(language),
                  preprocessed(preprocessed) {}

        ShaderSource preprocess(const std::function<std::string(const char *)> &include = {},
                                const std::map<std::string, std::string> &macros = {},
                                ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE) const {
            if (preprocessed)
                throw std::runtime_error("Source already preprocessed");
            ShaderSource ret(*this);
            ret.src = ShaderCompiler::preprocess(src, stage, language, include, macros, optimizationLevel);
            ret.preprocessed = true;
            return ret;
        }

        ShaderSource crossCompile(ShaderLanguage targetLanguage,
                                  ShaderCompiler::OptimizationLevel optimizationLevel) const {
            ShaderSource ret(*this);
            if (!ret.preprocessed)
                ret = ret.preprocess();
            ret.src = ShaderCompiler::crossCompile(src, entryPoint, stage, language, targetLanguage, optimizationLevel);
            ret.language = targetLanguage;
            return ret;
        }

        SPIRVBundle compile(ShaderCompiler::OptimizationLevel optimizationLevel
        = ShaderCompiler::OPTIMIZATION_NONE) const {
            ShaderSource shader(*this);
            if (!shader.preprocessed)
                shader = preprocess({}, {}, optimizationLevel);
            return SPIRVBundle({{.stage = shader.stage,
                                        .entryPoint = shader.entryPoint,
                                        .blobIndex = 0}},
                               {ShaderCompiler::compileToSPIRV(src, entryPoint, stage, language, optimizationLevel)});
        }

        const std::string &getSrc() const { return src; }

        const std::string &getEntryPoint() const { return entryPoint; }

        ShaderStage getStage() const { return stage; }

        ShaderLanguage getLanguage() const { return language; }

        bool isPreProcessed() const { return preprocessed; }

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
