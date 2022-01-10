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

#include "platform/graphics/shadersource.hpp"

namespace xengine {
    ShaderSource::ShaderSource(std::string src,
                               std::string entryPoint,
                               ShaderStage stage,
                               ShaderLanguage language,
                               bool preprocessed)
            : src(std::move(src)),
              entryPoint(std::move(entryPoint)),
              stage(stage),
              language(language),
              preprocessed(preprocessed) {}

    void ShaderSource::preprocess(const std::function<std::string(const char *)> &include,
                                  const std::map <std::string, std::string> &macros,
                                  ShaderCompiler::OptimizationLevel optimizationLevel) {
        if (preprocessed)
            throw std::runtime_error("Source already preprocessed");
        src = ShaderCompiler::preprocess(src, stage, language, include, macros, optimizationLevel);
        preprocessed = true;
    }

    void ShaderSource::crossCompile(ShaderLanguage targetLanguage,ShaderCompiler::OptimizationLevel optimizationLevel) {
        if (!preprocessed)
            preprocess();
        src = ShaderCompiler::crossCompile(src, entryPoint, stage, language, targetLanguage, optimizationLevel);
        language = targetLanguage;
    }

    std::vector <uint32_t> ShaderSource::compile(ShaderCompiler::OptimizationLevel optimizationLevel) {
        if (!preprocessed)
            this->preprocess({}, {}, optimizationLevel);
        return compileToSPIRV(src, entryPoint, stage, language, optimizationLevel);
    }

    const std::string &ShaderSource::getSrc() const { return src; }

    const std::string &ShaderSource::getEntryPoint() const { return entryPoint; }

    ShaderStage ShaderSource::getStage() const { return stage; }

    ShaderLanguage ShaderSource::getLanguage() const { return language; }

    bool ShaderSource::isPreProcessed() const { return preprocessed; }
}