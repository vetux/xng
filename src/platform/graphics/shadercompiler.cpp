/**
 *  XEngine - C++ game engine library
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

#include "platform/graphics/shadercompiler.hpp"

#include <stdexcept>

#include "shaderc/shaderc.hpp"

#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

namespace xengine {
    class IncludeHandler : public shaderc::CompileOptions::IncluderInterface {
    public:
        std::function<std::string(const char *)> callback;

        IncludeHandler() = default;

        explicit IncludeHandler(std::function<std::string(const char *)> callback)
                : callback(std::move(callback)) {}

        shaderc_include_result *GetInclude(const char *requested_source,
                                           shaderc_include_type type,
                                           const char *requesting_source,
                                           size_t include_depth) override {
            auto *ret = new shaderc_include_result();
            auto userData = new std::pair<std::string, std::string>(std::string(callback(requested_source)),
                                                                    std::string(requested_source));
            ret->user_data = userData;
            ret->content = userData->first.c_str();
            ret->content_length = userData->first.length();
            ret->source_name = userData->second.c_str();
            ret->source_name_length = userData->second.length();

            return ret;
        }

        void ReleaseInclude(shaderc_include_result *data) override {
            auto *pair = static_cast<std::pair<std::string, std::string> *>(data->user_data);
            delete pair;
            delete data;
        }
    };

    shaderc_optimization_level convertOptimizationLevel(ShaderCompiler::OptimizationLevel opt) {
        switch (opt) {
            case ShaderCompiler::OPTIMIZATION_NONE:
                return shaderc_optimization_level_zero;
            case ShaderCompiler::OPTIMIZATION_PERFORMANCE:
                return shaderc_optimization_level_performance;
            case ShaderCompiler::OPTIMIZATION_SIZE:
                return shaderc_optimization_level_size;
            default:
                throw std::runtime_error("Invalid optimization level " + std::to_string(opt));
        }
    }

    std::vector<uint32_t> ShaderCompiler::compileToSPIRV(const std::string &source,
                                                         const std::string &entryPoint,
                                                         ShaderStage stage,
                                                         ShaderLanguage language,
                                                         OptimizationLevel optimizationLevel) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        shaderc_shader_kind shaderStage;
        switch (stage) {
            case VERTEX:
                shaderStage = shaderc_vertex_shader;
                break;
            case GEOMETRY:
                shaderStage = shaderc_geometry_shader;
                break;
            case FRAGMENT:
                shaderStage = shaderc_fragment_shader;
                break;
        }

        shaderc_source_language shaderLang;
        switch (language) {
            case HLSL_SHADER_MODEL_4:
                shaderLang = shaderc_source_language_hlsl;
                break;
            case GLSL_460:
                options.SetTargetEnvironment(shaderc_target_env_opengl, 0);
                shaderLang = shaderc_source_language_glsl;
                break;
            case GLSL_460_VK:
                options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);
                shaderLang = shaderc_source_language_glsl;
                break;
            case GLSL_ES_320:
                options.SetTargetEnvironment(shaderc_target_env_opengl, 0);
                shaderLang = shaderc_source_language_glsl;
                break;
        }

        options.SetSourceLanguage(shaderLang);
        options.SetAutoBindUniforms(false);
        options.SetAutoSampledTextures(true);
        options.SetAutoMapLocations(true);
        options.SetOptimizationLevel(convertOptimizationLevel(optimizationLevel));

        auto compileResult = compiler.CompileGlslToSpv(source,
                                                       shaderStage,
                                                       "shader",
                                                       entryPoint.c_str(),
                                                       options);

        if (compileResult.GetCompilationStatus() != shaderc_compilation_status_success) {
            throw std::runtime_error("Failed to compile to spirv " + compileResult.GetErrorMessage());
        }

        return {compileResult.cbegin(), compileResult.cend()};
    }

    std::string ShaderCompiler::decompileSPIRV(const std::vector<uint32_t> &source,
                                               ShaderLanguage targetLanguage) {
        switch (targetLanguage) {
            case HLSL_SHADER_MODEL_4: {
                spirv_cross::CompilerHLSL sCompiler(source);

                spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                // Set the first uniform buffer name to "Globals" because shaderc
                // merges all uniform variables declared in the source into a single uniform buffer in the SPIRV.
                if (!resources.uniform_buffers.empty())
                    sCompiler.set_name(resources.uniform_buffers[0].id, "Globals");

                spirv_cross::CompilerGLSL::Options sOptions;
                spirv_cross::CompilerHLSL::Options hlslOptions;

                hlslOptions.shader_model = 40;

                sCompiler.set_hlsl_options(hlslOptions);
                sCompiler.set_common_options(sOptions);

                sCompiler.build_combined_image_samplers();

                return sCompiler.compile();
            }
            case GLSL_460_VK:
            case GLSL_460: {
                spirv_cross::CompilerGLSL sCompiler(source);

                spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                // Set the first uniform buffer name to "Globals" because shaderc
                // merges all uniform variables declared in the source into a single uniform buffer in the SPIRV.
                if (!resources.uniform_buffers.empty())
                    sCompiler.set_name(resources.uniform_buffers[0].id, "Globals");

                spirv_cross::CompilerGLSL::Options sOptions;
                sOptions.version = 460;

                //Dont generate glsl which uses the uniform buffer api.
                sOptions.emit_uniform_buffer_as_plain_uniforms = true;
                sOptions.separate_shader_objects = true;

                sCompiler.set_common_options(sOptions);

                sCompiler.build_dummy_sampler_for_combined_images();
                sCompiler.build_combined_image_samplers();

                return sCompiler.compile();
            }
            case GLSL_ES_320 : {
                spirv_cross::CompilerGLSL sCompiler(source);

                spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                // Set the first uniform buffer name to "Globals" because shaderc
                // merges all uniform variables declared in the source into a single uniform buffer in the SPIRV.
                if (!resources.uniform_buffers.empty())
                    sCompiler.set_name(resources.uniform_buffers[0].id, "Globals");

                spirv_cross::CompilerGLSL::Options sOptions;
                sOptions.version = 320;

                //Dont generate glsl which uses the uniform buffer api.
                sOptions.emit_uniform_buffer_as_plain_uniforms = true;
                sOptions.separate_shader_objects = true;

                sOptions.es = true;

                //Set medium precision because high precision is optional in OpenGL ES
                sOptions.fragment.default_float_precision = sOptions.Mediump;
                sOptions.fragment.default_int_precision = sOptions.Mediump;

                sCompiler.set_common_options(sOptions);

                return sCompiler.compile();
            }
            default:
                throw std::runtime_error("");
        }
    }

    std::string ShaderCompiler::preprocess(const std::string &source,
                                           ShaderStage stage,
                                           ShaderLanguage language,
                                           const std::function<std::string(const char *)> &include,
                                           const std::map<std::string, std::string> &macros,
                                           OptimizationLevel optimizationLevel) {
        shaderc_shader_kind shaderStage;
        switch (stage) {
            case VERTEX:
                shaderStage = shaderc_vertex_shader;
                break;
            case GEOMETRY:
                shaderStage = shaderc_geometry_shader;
                break;
            case FRAGMENT:
                shaderStage = shaderc_fragment_shader;
                break;
        }

        shaderc_source_language shaderLang;
        switch (language) {
            case HLSL_SHADER_MODEL_4:
                shaderLang = shaderc_source_language_hlsl;
                break;
            case GLSL_460:
            case GLSL_460_VK:
            case GLSL_ES_320:
                shaderLang = shaderc_source_language_glsl;
                break;
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        for (auto &p: macros)
            options.AddMacroDefinition(p.first, p.second);

        options.SetIncluder(std::make_unique<IncludeHandler>(include));
        options.SetSourceLanguage(shaderLang);
        options.SetAutoBindUniforms(false);
        options.SetAutoSampledTextures(true);
        options.SetAutoMapLocations(true);

        options.SetOptimizationLevel(convertOptimizationLevel(optimizationLevel));

        auto preProcessResult = compiler.PreprocessGlsl(source,
                                                        shaderStage,
                                                        "shader",
                                                        options);

        if (preProcessResult.GetCompilationStatus() != shaderc_compilation_status_success) {
            throw std::runtime_error("Failed to preprocess glsl: " + preProcessResult.GetErrorMessage());
        }

        return {preProcessResult.cbegin(), preProcessResult.cend()};
    }

    std::string ShaderCompiler::crossCompile(const std::string &source,
                                             const std::string &entryPoint,
                                             ShaderStage stage,
                                             ShaderLanguage sourceLanguage,
                                             ShaderLanguage targetLanguage,
                             OptimizationLevel optimizationLevel) {
        return decompileSPIRV(compileToSPIRV(source,
                                             entryPoint,
                                             stage,
                                             sourceLanguage,
                                             optimizationLevel),
                              targetLanguage);
    }
}