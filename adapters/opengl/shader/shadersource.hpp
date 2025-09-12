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

#ifndef XENGINE_SHADERSOURCE_HPP
#define XENGINE_SHADERSOURCE_HPP

#include "xng/io/messageable.hpp"

#include "shader/spirvshader.hpp"
#include "shader/shaderlanguage.hpp"
#include "shader/shadercompiler.hpp"
#include "shader/shaderdecompiler.hpp"

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

        ShaderSource preprocess(const ShaderCompiler &compiler,
                                const std::function<std::string(const char *)> &include = {},
                                const std::map<std::string, std::string> &macros = {},
                                ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE,
                                ShaderEnvironment environment = ENVIRONMENT_OPENGL) const {
            if (preprocessed)
                throw std::runtime_error("Source already preprocessed");
            ShaderSource ret(*this);
            ret.src = compiler.preprocess(src, stage, language, include, macros, optimizationLevel, environment);
            ret.preprocessed = true;
            return ret;
        }

        ShaderSource crossCompile(const ShaderCompiler &compiler,
                                  const ShaderDecompiler &decompiler,
                                  ShaderLanguage targetLanguage,
                                  ShaderCompiler::OptimizationLevel optimizationLevel,
                                  ShaderEnvironment environment) const {
            ShaderSource ret(*this);
            if (!ret.preprocessed)
                ret = ret.preprocess(compiler);
            ret.src = decompiler.decompile(
                    compiler.compile(src, entryPoint, stage, language, optimizationLevel, environment),
                    entryPoint,
                    stage,
                    targetLanguage);
            ret.language = targetLanguage;
            return ret;
        }

        SPIRVShader compile(const ShaderCompiler &compiler,
                            ShaderCompiler::OptimizationLevel optimizationLevel = ShaderCompiler::OPTIMIZATION_NONE,
                            ShaderEnvironment environment = ENVIRONMENT_OPENGL) const {
            ShaderSource shader = *this;
            if (!shader.preprocessed)
                shader = preprocess(compiler, {}, {}, optimizationLevel);
            return {environment,
                    shader.stage,
                    shader.entryPoint,
                    compiler.compile(shader.src,
                                     shader.entryPoint,
                                     shader.stage,
                                     shader.language,
                                     optimizationLevel,
                                     environment)};
        }

        const std::string &getSrc() const { return src; }

        const std::string &getEntryPoint() const { return entryPoint; }

        ShaderStage getStage() const { return stage; }

        ShaderLanguage getLanguage() const { return language; }

        bool isPreProcessed() const { return preprocessed; }

        bool isEmpty() const { return src.empty(); }

        Messageable &operator<<(const Message &message) override {
            message.value("src", src);
            message.value("entryPoint", entryPoint);
            stage = (ShaderStage) message.getMessage("stage", Message((int) VERTEX)).asInt();
            language = (ShaderLanguage) message.getMessage("language", Message((int) GLSL_420)).asInt();
            message.value("preprocessed", preprocessed);
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
