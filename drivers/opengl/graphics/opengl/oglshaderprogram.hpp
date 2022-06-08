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

#ifndef XENGINE_OGLSHADERPROGRAM_HPP
#define XENGINE_OGLSHADERPROGRAM_HPP

#include <string>
#include <map>
#include <functional>
#include <utility>

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"

#include "graphics/shaderprogram.hpp"

#include "graphics/opengl/oglbuildmacro.hpp"

#include "graphics/shader/shadercompiler.hpp"

namespace xengine {
    namespace opengl {
        class OPENGL_TYPENAME(ShaderProgram) : public ShaderProgram OPENGL_INHERIT {
        public:
            ShaderProgramDesc desc;
            GLuint programHandle = 0;

            /**
             * @param vertexShader The preprocessed glsl vertex shader.
             * @param fragmentShader The preprocessed glsl fragment shader.
             * @param geometryShader The preprocessed glsl geometry shader, if empty no geometry shader is used.
             */
            explicit OPENGL_TYPENAME(ShaderProgram)(ShaderProgramDesc desc)
                    : desc(std::move(desc)) {
                initialize();

                char *vertexSource, *fragmentSource, *geometrySource = nullptr;

                std::string vert, frag, geo;
                auto it = desc.entries.find(VERTEX);
                if (it == desc.entries.end())
                    throw std::runtime_error("No vertex shader");

                vert = ShaderCompiler::decompileSPIRV(desc.buffers.at(it->second.bufferIndex).blob,
                                                      it->second.entryPoint,
                                                      VERTEX,
                                                      GLSL_420);
                vertexSource = vert.data();

                it = desc.entries.find(FRAGMENT);
                if (it == desc.entries.end())
                    throw std::runtime_error("No fragment shader");

                frag = ShaderCompiler::decompileSPIRV(desc.buffers.at(it->second.bufferIndex).blob,
                                                      it->second.entryPoint,
                                                      FRAGMENT,
                                                      GLSL_420);
                fragmentSource = frag.data();

                it = desc.entries.find(GEOMETRY);
                if (it != desc.entries.end()) {
                    geo = ShaderCompiler::decompileSPIRV(desc.buffers.at(it->second.bufferIndex).blob,
                                                         it->second.entryPoint,
                                                         GEOMETRY,
                                                         GLSL_420);
                    geometrySource = geo.data();
                }

                programHandle = glCreateProgram();

                GLuint vsH = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vsH, 1, &vertexSource, NULL);
                glCompileShader(vsH);
                GLint success;
                glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(vsH, 512, NULL, infoLog);
                    glDeleteShader(vsH);
                    std::string error = "Failed to compile vertex shader: ";
                    error.append(infoLog);
                    throw std::runtime_error(error);
                }

                glAttachShader(programHandle, vsH);

                GLuint gsH;

                if (geometrySource != nullptr) {
                    gsH = glCreateShader(GL_GEOMETRY_SHADER);
                    glShaderSource(gsH, 1, &geometrySource, NULL);
                    glCompileShader(gsH);
                    glGetShaderiv(gsH, GL_COMPILE_STATUS, &success);
                    if (!success) {
                        char infoLog[512];
                        glGetShaderInfoLog(gsH, 512, NULL, infoLog);
                        glDeleteShader(vsH);
                        glDeleteShader(gsH);
                        std::string error = "Failed to compile geometry shader: ";
                        error.append(infoLog);
                        throw std::runtime_error(error);
                    }
                    glAttachShader(programHandle, gsH);
                }

                GLuint fsH = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fsH, 1, &fragmentSource, NULL);
                glCompileShader(fsH);
                glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetShaderInfoLog(fsH, 512, NULL, infoLog);
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
                checkGLError("");
            }

            ~OPENGL_TYPENAME(ShaderProgram)() override {
                glDeleteProgram(programHandle);
            }

            OPENGL_TYPENAME(ShaderProgram)(const OPENGL_TYPENAME(ShaderProgram) &copy) = delete;

            OPENGL_TYPENAME(ShaderProgram) &operator=(const OPENGL_TYPENAME(ShaderProgram) &) = delete;

            const ShaderProgramDesc &getDescription() override {
                return desc;
            }

            void activate() {
                glUseProgram(programHandle);
                checkGLError("");
            }

            void checkLinkSuccess() {
                auto msg = getLinkError();
                if (!msg.empty())
                    throw std::runtime_error(msg);
            }

            std::string getLinkError() {
                GLint success;
                glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
                if (!success) {
                    GLchar infoLog[512];
                    glGetProgramInfoLog(programHandle, 512, NULL, infoLog);
                    std::string error = "Failed to link shader program: ";
                    error.append(infoLog);
                    return error;
                }
                return "";
            }

            OPENGL_MEMBERS
        };
    }
}

#endif //XENGINE_OGLSHADERPROGRAM_HPP
