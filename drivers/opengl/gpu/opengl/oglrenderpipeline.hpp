/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_OGLRENDERPIPELINE_HPP
#define XENGINE_OGLRENDERPIPELINE_HPP

#include "xng/gpu/renderpipeline.hpp"

#include <utility>

#include "opengl_include.hpp"

#include "gpu/opengl/oglrendertarget.hpp"
#include "gpu/opengl/oglshaderuniformbuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"
#include "gpu/opengl/oglfence.hpp"
#include "gpu/opengl/oglvertexarrayobject.hpp"
#include "gpu/opengl/ogltexturearraybuffer.hpp"

namespace xng::opengl {
    class OGLRenderPipeline : public RenderPipeline {
    public:
        std::function<void(RenderObject *)> destructor;
        RenderPipelineDesc desc;

        GLuint programHandle = 0;

        explicit OGLRenderPipeline(std::function<void(RenderObject *)> destructor,
                                   RenderPipelineDesc descArg,
                                   ShaderDecompiler &decompiler)
                : destructor(std::move(destructor)),
                  desc(std::move(descArg)) {
            if (!desc.shaders.empty()) {
                char *vertexSource, *fragmentSource, *geometrySource = nullptr;

                std::string vert, frag, geo;
                auto it = desc.shaders.find(VERTEX);
                if (it == desc.shaders.end())
                    throw std::runtime_error("No vertex shader");

                vert = decompiler.decompile(desc.shaders.at(VERTEX).getBlob(),
                                            it->second.getEntryPoint(),
                                            VERTEX,
                                            GLSL_460);
                vertexSource = vert.data();

                it = desc.shaders.find(FRAGMENT);
                if (it == desc.shaders.end())
                    throw std::runtime_error("No fragment shader");

                frag = decompiler.decompile(desc.shaders.at(FRAGMENT).getBlob(),
                                            it->second.getEntryPoint(),
                                            FRAGMENT,
                                            GLSL_460);
                fragmentSource = frag.data();

                it = desc.shaders.find(GEOMETRY);
                if (it != desc.shaders.end()) {
                    geo = decompiler.decompile(desc.shaders.at(GEOMETRY).getBlob(),
                                               it->second.getEntryPoint(),
                                               GEOMETRY,
                                               GLSL_460);
                    geometrySource = geo.data();
                }

                programHandle = glCreateProgram();

                GLuint vsH = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vsH, 1, &vertexSource, nullptr);
                glCompileShader(vsH);
                GLint success;
                glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(vsH, 512, nullptr, infoLog);
                    glDeleteShader(vsH);
                    std::string error = "Failed to compile vertex shader: ";
                    error.append(infoLog);
                    throw std::runtime_error(error);
                }

                glAttachShader(programHandle, vsH);

                GLuint gsH;

                if (geometrySource != nullptr) {
                    gsH = glCreateShader(GL_GEOMETRY_SHADER);
                    glShaderSource(gsH, 1, &geometrySource, nullptr);
                    glCompileShader(gsH);
                    glGetShaderiv(gsH, GL_COMPILE_STATUS, &success);
                    if (!success) {
                        char infoLog[512];
                        glGetShaderInfoLog(gsH, 512, nullptr, infoLog);
                        glDeleteShader(vsH);
                        glDeleteShader(gsH);
                        std::string error = "Failed to compile geometry shader: ";
                        error.append(infoLog);
                        throw std::runtime_error(error);
                    }
                    glAttachShader(programHandle, gsH);
                }

                GLuint fsH = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fsH, 1, &fragmentSource, nullptr);
                glCompileShader(fsH);
                glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(fsH, 512, nullptr, infoLog);
                    glDeleteShader(vsH);
                    if (geometrySource != nullptr)
                        glDeleteShader(gsH);
                    glDeleteShader(fsH);
                    std::string error = "Failed to compile fragment shader: ";
                    error.append(infoLog);
                    throw std::runtime_error(error);
                }
                glAttachShader(programHandle, fsH);

                glLinkProgram(programHandle);

                glDeleteShader(vsH);
                glDeleteShader(fsH);

                checkLinkSuccess();
            }

            checkGLError();
        }

        ~OGLRenderPipeline() override {
            glDeleteProgram(programHandle);
            destructor(this);
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

        const RenderPipelineDesc &getDescription() override {
            return desc;
        }

        std::vector<uint8_t> cache() override {
            throw std::runtime_error("Not Implemented");
        }
    };
}

#endif //XENGINE_OGLRENDERPIPELINE_HPP
