/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include <memory>

#include "xng/platform/glslang/glslang.hpp"

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/Public/ResourceLimits.h"
#include "SPIRV/GlslangToSpv.h"

namespace xng::glslang {
    class Includer : public ::glslang::TShader::Includer {
    public:
        std::function<std::string(const char *)> includeCallback;

        std::map<IncludeResult *, std::shared_ptr<std::string>> results;

        explicit Includer(const std::function<std::string(const char *)> &includeCallback)
                : includeCallback(includeCallback) {}

        IncludeResult *includeSystem(const char *string, const char *string1, size_t size) override {
            return includeLocal(string, string1, size);
        }

        IncludeResult *includeLocal(const char *string, const char *string1, size_t size) override {
            auto str = std::make_shared<std::string>(includeCallback(string));

            auto res = new IncludeResult(string, str->c_str(), str->length(), nullptr);

            results[res] = str;

            return res;
        }

        void releaseInclude(IncludeResult *result) override {
            auto str = results.at(result);
            results.erase(result);
            delete result;
        }
    };

    EShLanguage getLanguage(ShaderStage stage) {
        switch (stage) {
            case VERTEX:
                return EShLangVertex;
            case TESSELLATION_CONTROL:
                return EShLangTessControl;
            case TESSELLATION_EVALUATION:
                return EShLangTessEvaluation;
            case GEOMETRY:
                return EShLangGeometry;
            case FRAGMENT:
                return EShLangFragment;
            case COMPUTE:
                return EShLangCompute;
            case RAY_GENERATE:
                return EShLangRayGen;
            case RAY_HIT_ANY:
                return EShLangAnyHit;
            case RAY_HIT_CLOSEST:
                return EShLangClosestHit;
            case RAY_MISS:
                return EShLangMiss;
            case RAY_INTERSECT:
                return EShLangIntersect;
            default:
                throw std::runtime_error("Invalid stage");
        }
    }


    ::glslang::EShClient getClient(ShaderEnvironment env) {
        switch (env) {
            case ENVIRONMENT_NONE:
                return ::glslang::EShClientNone;
            case ENVIRONMENT_OPENGL:
                return ::glslang::EShClientOpenGL;
            case ENVIRONMENT_VULKAN_1_0:
            case ENVIRONMENT_VULKAN_1_1:
            case ENVIRONMENT_VULKAN_1_2:
            case ENVIRONMENT_VULKAN_1_3:
                return ::glslang::EShClientVulkan;
            default:
                throw std::runtime_error("Invalid environment");
        }
    }

    ::glslang::EShTargetClientVersion getClientVersion(ShaderEnvironment env) {
        switch (env) {
            case ENVIRONMENT_NONE:
            case ENVIRONMENT_OPENGL:
                return ::glslang::EShTargetOpenGL_450;
            case ENVIRONMENT_VULKAN_1_0:
                return ::glslang::EShTargetVulkan_1_0;
            case ENVIRONMENT_VULKAN_1_1:
                return ::glslang::EShTargetVulkan_1_1;
            case ENVIRONMENT_VULKAN_1_2:
                return ::glslang::EShTargetVulkan_1_2;
            case ENVIRONMENT_VULKAN_1_3:
                return ::glslang::EShTargetVulkan_1_3;
            default:
                throw std::runtime_error("Invalid environment");
        }
    }

    ::glslang::EShTargetLanguageVersion getLanguageVersion(ShaderEnvironment env) {
        switch (env) {
            case ENVIRONMENT_NONE:
            case ENVIRONMENT_OPENGL:
            case ENVIRONMENT_VULKAN_1_0:
                return ::glslang::EShTargetSpv_1_0;
            case ENVIRONMENT_VULKAN_1_1:
                return ::glslang::EShTargetSpv_1_3;
            case ENVIRONMENT_VULKAN_1_2:
                return ::glslang::EShTargetSpv_1_5;
            case ENVIRONMENT_VULKAN_1_3:
                return ::glslang::EShTargetSpv_1_6;
            default:
                throw std::runtime_error("Invalid environment");
        }
    }

    ::glslang::EShSource getSourceLanguage(ShaderLanguage lang) {
        switch (lang) {
            case HLSL_SHADER_MODEL_4:
                return ::glslang::EShSourceHlsl;
            case GLSL_ES_320:
            case GLSL_420:
            case GLSL_460:
                return ::glslang::EShSourceGlsl;
            default:
                throw std::runtime_error("Invalid shader language");
        }
    }

    int getSourceVersion(ShaderLanguage lang) {
        switch (lang) {
            case HLSL_SHADER_MODEL_4:
                return 0;
            case GLSL_ES_320:
                return 320;
            case GLSL_420:
                return 420;
            case GLSL_460:
                return 460;
            default:
                throw std::runtime_error("Invalid shader language");
        }
    }

    GLSLang::GLSLang() {
        glslang_initialize_process();
    }

    GLSLang::~GLSLang() {
        glslang_finalize_process();
    }

    std::vector<uint32_t> GLSLang::compile(const std::string &source,
                                           const std::string &entryPoint,
                                           ShaderStage stage,
                                           ShaderLanguage language,
                                           ShaderCompiler::OptimizationLevel optimizationLevel,
                                           ShaderEnvironment environment) const {
        auto shaderStage = getLanguage(stage);

        ::glslang::TShader shader(shaderStage);

        const char *sources[1] = {source.data()};
        shader.setStrings(sources, 1);

        shader.setEntryPoint(entryPoint.c_str());

        ::glslang::EShTargetClientVersion targetApiVersion = getClientVersion(environment);
        ::glslang::EShTargetLanguageVersion spirvVersion = getLanguageVersion(environment);
        ::glslang::EShClient client = getClient(environment);

        shader.setEnvInput(getSourceLanguage(language),
                           shaderStage,
                           client,
                           getSourceVersion(language));
        shader.setEnvClient(client, targetApiVersion);
        shader.setEnvTarget(::glslang::EshTargetSpv, spirvVersion);

        const TBuiltInResource *resources = GetDefaultResources();
        const int defaultVersion = 100;
        const bool forwardCompatible = false;
        const EShMessages messageFlags = EShMsgDefault;
        EProfile defaultProfile = ENoProfile;

        if (!shader.parse(resources,
                          defaultVersion,
                          defaultProfile,
                          false,
                          forwardCompatible,
                          messageFlags)) {
            auto log = shader.getInfoLog();
            throw std::runtime_error("Failed to parse shader: " + std::string(log));
        }

        ::glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messageFlags)) {
            auto log = program.getInfoLog();
            throw std::runtime_error("Failed to link shader: " + std::string(log));
        }

        ::glslang::TIntermediate &intermediateRef = *(program.getIntermediate(shaderStage));

        std::vector<uint32_t> spirv;

        ::glslang::SpvOptions options{};
        options.validate = true;

        ::glslang::GlslangToSpv(intermediateRef, spirv, &options);

        return spirv;
    }

    std::string GLSLang::preprocess(const std::string &source,
                                    ShaderStage stage,
                                    ShaderLanguage language,
                                    const std::function<std::string(const char *)> &include,
                                    const std::map<std::string, std::string> &macros,
                                    ShaderCompiler::OptimizationLevel optimizationLevel,
                                    ShaderEnvironment environment) const {
        auto src = source;

        // Sandwich the extension and defines between the #version declaration and the rest of the shader because shader.setPreamble prepends the text before the #version for some reason.
        auto end = src.find('\n');

        if (end != std::string::npos){
            src = source.substr(0, end + 1);

            std::string preamble = "#extension GL_GOOGLE_include_directive : enable\n"; // Apparently #include is not standard glsl and requires this extension.
            for (auto &macro : macros){
                preamble += "#define " + macro.first + " " + macro.second + "\n";
            }

            src += preamble;

            src += source.substr(end + 1);
        }

        auto shaderStage = getLanguage(stage);

        ::glslang::TShader shader(shaderStage);

        const char *sources[1] = {src.data()};
        shader.setStrings(sources, 1);

        ::glslang::EShTargetClientVersion targetApiVersion = getClientVersion(environment);
        ::glslang::EShTargetLanguageVersion spirvVersion = getLanguageVersion(environment);
        ::glslang::EShClient client = getClient(environment);

        shader.setEnvInput(getSourceLanguage(language),
                           shaderStage,
                           client,
                           getSourceVersion(language));
        shader.setEnvClient(client, targetApiVersion);
        shader.setEnvTarget(::glslang::EshTargetSpv, spirvVersion);

        const TBuiltInResource *resources = GetDefaultResources();
        const int defaultVersion = 100;
        const bool forwardCompatible = false;
        const EShMessages messageFlags = EShMsgDefault;
        EProfile defaultProfile = ENoProfile;

        Includer includer(include);

        std::string preprocessedStr;
        if (!shader.preprocess(resources,
                               defaultVersion,
                               defaultProfile,
                               false,
                               forwardCompatible,
                               messageFlags,
                               &preprocessedStr,
                               includer)) {
            auto log = shader.getInfoLog();
            throw std::runtime_error("Failed to preprocess shader: " + std::string(log));
        }

        return preprocessedStr;
    }
}