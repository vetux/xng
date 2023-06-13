/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_OGLCOMPUTEPIPELINE_HPP
#define XENGINE_OGLCOMPUTEPIPELINE_HPP

#include "xng/gpu/computepipeline.hpp"

#include <utility>

namespace xng::opengl {
    class OGLComputePipeline : public ComputePipeline {
    public:
        std::function<void(RenderObject * )> destructor;
        ComputePipelineDesc desc;

        GLuint programHandle;

        void buildGLSL(ShaderDecompiler &decompiler) {
            char *shaderSource = nullptr;

            std::string shader;
            auto it = desc.shaders.find(COMPUTE);
            if (it == desc.shaders.end())
                throw std::runtime_error("No vertex shader");

            shader = decompiler.decompile(desc.shaders.at(COMPUTE).getBlob(),
                                        it->second.getEntryPoint(),
                                        COMPUTE,
                                        GLSL_460);
            shaderSource = shader.data();

            programHandle = glCreateProgram();

            GLuint vsH = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(vsH, 1, &shaderSource, nullptr);
            glCompileShader(vsH);
            GLint success;
            glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(vsH, 512, nullptr, infoLog);
                glDeleteShader(vsH);
                std::string error = "Failed to compile compute shader: ";
                error.append(infoLog);
                throw std::runtime_error(error);
            }

            glAttachShader(programHandle, vsH);

            glLinkProgram(programHandle);

            glDeleteShader(vsH);
        }

        void buildSPIRV() {
            programHandle = glCreateProgram();

            GLuint vsH = glCreateShader(GL_COMPUTE_SHADER);
            glShaderBinary(1,
                           &vsH,
                           GL_SHADER_BINARY_FORMAT_SPIR_V,
                           desc.shaders.at(VERTEX).getBlob().data(),
                           static_cast<GLsizei>(desc.shaders.at(COMPUTE).getBlob().size() * sizeof(uint32_t)));
            glSpecializeShader(vsH, desc.shaders.at(COMPUTE).getEntryPoint().c_str(), 0, nullptr, nullptr);

            GLint success;
            glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(vsH, 512, nullptr, infoLog);
                glDeleteShader(vsH);
                std::string error = "Failed to compile compute shader: ";
                error.append(infoLog);
                throw std::runtime_error(error);
            }

            glAttachShader(programHandle, vsH);

            glLinkProgram(programHandle);

            glDeleteShader(vsH);
        }

        OGLComputePipeline(std::function<void(RenderObject * )> destructor,
                           const ComputePipelineDesc& desc,
                           ShaderDecompiler &decompiler) : destructor(std::move(destructor)),
                                                       desc(desc) {
            if (!desc.shaders.empty()) {
                // if (!GLAD_GL_ARB_gl_spirv) {
                buildGLSL(decompiler);
                /* } else {
                      buildSPIRV(); // The OpenGL implementation for windows (AMD) appears to not support spirv shaders despite using 4.6.
                  }*/
                checkLinkSuccess();
            }
        }

        void checkLinkSuccess() const {
            auto msg = getLinkError();
            if (!msg.empty())
                throw std::runtime_error(msg);
        }

        std::string getLinkError() const {
            GLint success;
            glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
                std::string error = "Failed to link shader program: ";
                error.append(infoLog);
                return error;
            }
            return "";
        }

        const ComputePipelineDesc &getDescription() override {
            return desc;
        }
    };
}
#endif //XENGINE_OGLCOMPUTEPIPELINE_HPP
