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

#ifndef XENGINE_SHADERSOURCE_HPP
#define XENGINE_SHADERSOURCE_HPP

#include "shader/spirvbundle.hpp"
#include "shader/shaderlanguage.hpp"
#include "shader/spirvcompiler.hpp"
#include "shader/spirvdecompiler.hpp"

namespace xng {
    class XENGINE_EXPORT ShaderSource : public Messageable {
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

        ShaderSource preprocess(const SPIRVCompiler &compiler,
                                const std::function<std::string(const char *)> &include = {},
                                const std::map<std::string, std::string> &macros = {},
                                SPIRVCompiler::OptimizationLevel optimizationLevel = SPIRVCompiler::OPTIMIZATION_NONE) const {
            if (preprocessed)
                throw std::runtime_error("Source already preprocessed");
            ShaderSource ret(*this);
            ret.src = compiler.preprocess(src, stage, language, include, macros, optimizationLevel);
            ret.preprocessed = true;
            return ret;
        }

        ShaderSource crossCompile(const SPIRVCompiler &compiler,
                                  const SPIRVDecompiler &decompiler,
                                  ShaderLanguage targetLanguage,
                                  SPIRVCompiler::OptimizationLevel optimizationLevel) const {
            ShaderSource ret(*this);
            if (!ret.preprocessed)
                ret = ret.preprocess(compiler);
            ret.src = decompiler.decompile(compiler.compile(src, entryPoint, stage, language, optimizationLevel),
                                           entryPoint, stage, targetLanguage);
            ret.language = targetLanguage;
            return ret;
        }

        SPIRVBundle compile(const SPIRVCompiler &compiler,
                            SPIRVCompiler::OptimizationLevel optimizationLevel = SPIRVCompiler::OPTIMIZATION_NONE) const {
            ShaderSource shader(*this);
            if (!shader.preprocessed)
                shader = preprocess(compiler, {}, {}, optimizationLevel);
            return SPIRVBundle({{.stage = shader.stage,
                                        .entryPoint = shader.entryPoint,
                                        .blobIndex = 0}},
                               {compiler.compile(src, entryPoint, stage, language, optimizationLevel)});
        }

        const std::string &getSrc() const { return src; }

        const std::string &getEntryPoint() const { return entryPoint; }

        ShaderStage getStage() const { return stage; }

        ShaderLanguage getLanguage() const { return language; }

        bool isPreProcessed() const { return preprocessed; }

        bool isEmpty() const { return src.empty(); }

        Messageable &operator<<(const Message &message) override {
            src = message.value("src", std::string());
            entryPoint = message.value("entryPoint", std::string());
            stage = (ShaderStage) message.value("stage", (int) VERTEX);
            language = (ShaderLanguage) message.value("language", (int) GLSL_420);
            preprocessed = message.value("preprocessed", false);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["src"] = src;
            message["entryPoint"] = entryPoint;
            message["stage"] = (int) stage;
            message["language"] = (int) language;
            message["preprocessed"] = preprocessed;
            return message;
        }

    private:
        std::string src{};
        std::string entryPoint{};
        ShaderStage stage{};
        ShaderLanguage language{};
        bool preprocessed{};
    };
}

#endif //XENGINE_SHADERSOURCE_HPP
