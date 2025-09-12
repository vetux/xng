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

#include "spirvcross.hpp"

#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>

namespace xng::spirv_cross {
    static spv::ExecutionModel convertShaderStage(ShaderStageType stage) {
        switch (stage) {
            case VERTEX:
                return spv::ExecutionModel::ExecutionModelVertex;
            case GEOMETRY:
                return spv::ExecutionModel::ExecutionModelGeometry;
            case FRAGMENT:
                return spv::ExecutionModel::ExecutionModelFragment;
            case TESSELLATION_CONTROL:
                return spv::ExecutionModelTessellationControl;
            case TESSELLATION_EVALUATION:
                return spv::ExecutionModelTessellationEvaluation;
            case COMPUTE:
                return spv::ExecutionModelGLCompute;
            case RAY_GENERATE:
                return spv::ExecutionModelRayGenerationKHR;
            case RAY_HIT_ANY:
                return spv::ExecutionModelAnyHitKHR;
            case RAY_HIT_CLOSEST:
                return spv::ExecutionModelClosestHitKHR;
            case RAY_MISS:
                return spv::ExecutionModelMissKHR;
            case RAY_INTERSECT:
                return spv::ExecutionModelIntersectionKHR;
            default:
                throw std::runtime_error("Unsupported shader stage");
        }
    }

    std::string SpirvCross::decompile(const std::vector<uint32_t> &source,
                                      const std::string &entryPoint,
                                      ShaderStageType stage,
                                      ShaderLanguage targetLanguage) const {
        switch (targetLanguage) {
            case HLSL_SHADER_MODEL_4: {
                ::spirv_cross::CompilerHLSL sCompiler(source);
                sCompiler.set_entry_point(entryPoint, convertShaderStage(stage));

                ::spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                ::spirv_cross::CompilerGLSL::Options sOptions;
                ::spirv_cross::CompilerHLSL::Options hlslOptions;

                hlslOptions.shader_model = 40;
                sCompiler.set_hlsl_options(hlslOptions);
                sCompiler.set_common_options(sOptions);

                sCompiler.build_combined_image_samplers();

                return sCompiler.compile();
            }
            case GLSL_460: {
                ::spirv_cross::CompilerGLSL sCompiler(source);
                sCompiler.set_entry_point(entryPoint, convertShaderStage(stage));

                ::spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                ::spirv_cross::CompilerGLSL::Options sOptions;
                sOptions.version = 460;

                sCompiler.set_common_options(sOptions);

                sCompiler.build_dummy_sampler_for_combined_images();
                sCompiler.build_combined_image_samplers();

                return sCompiler.compile();
            }
            case GLSL_420: {
                ::spirv_cross::CompilerGLSL sCompiler(source);
                sCompiler.set_entry_point(entryPoint, convertShaderStage(stage));

                ::spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                ::spirv_cross::CompilerGLSL::Options sOptions;
                sOptions.version = 420;

                sCompiler.set_common_options(sOptions);

                sCompiler.build_dummy_sampler_for_combined_images();
                sCompiler.build_combined_image_samplers();

                return sCompiler.compile();
            }
            case GLSL_ES_320 : {
                ::spirv_cross::CompilerGLSL sCompiler(source);
                sCompiler.set_entry_point(entryPoint, convertShaderStage(stage));

                ::spirv_cross::ShaderResources resources = sCompiler.get_shader_resources();

                ::spirv_cross::CompilerGLSL::Options sOptions;
                sOptions.version = 320;

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
}