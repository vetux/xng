/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_OGLSHADERPROGRAM_HPP
#define XENGINE_OGLSHADERPROGRAM_HPP

#include "glad/glad.h"

#include "compiledpipeline.hpp"

#include "ogldebug.hpp"

struct OGLShaderProgram {
    GLuint programHandle{};

    explicit OGLShaderProgram(const CompiledPipeline &pipeline) {
        auto it = pipeline.sourceCode.find(Shader::VERTEX);
        if (it == pipeline.sourceCode.end())
            throw std::runtime_error("No vertex shader");

        const std::string vert = it->second;

        it = pipeline.sourceCode.find(Shader::FRAGMENT);
        if (it == pipeline.sourceCode.end())
            throw std::runtime_error("No fragment shader");

        const std::string frag = it->second;

        std::string geo;
        it = pipeline.sourceCode.find(Shader::GEOMETRY);
        if (it != pipeline.sourceCode.end()) {
            geo = it->second;
        }

        if (geo.size() > 0) {
            buildShader(vert, frag, geo);
        } else {
            buildShader(vert, frag);
        }
    }

    ~OGLShaderProgram() {
        glDeleteProgram(programHandle);
    }

    void buildShader(const std::string &vertexSource, const std::string &fragmentSource) {
        const auto vertSrc = vertexSource.c_str();
        const auto fragSrc = fragmentSource.c_str();

        programHandle = glCreateProgram();

        const GLuint vsH = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsH, 1, &vertSrc, nullptr);
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

        const GLuint fsH = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsH, 1, &fragSrc, nullptr);
        glCompileShader(fsH);
        glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(fsH, 512, nullptr, infoLog);
            glDeleteShader(vsH);
            glDeleteShader(fsH);
            std::string error = "Failed to compile fragment shader: ";
            error.append(infoLog);
            throw std::runtime_error(error);
        }
        glAttachShader(programHandle, fsH);

        glLinkProgram(programHandle);

        glDeleteShader(vsH);
        glDeleteShader(fsH);

        glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
            glDeleteProgram(programHandle);
            std::string error = "Failed to link shader program: ";
            error.append(infoLog);
            throw std::runtime_error(error);
        }

        oglCheckError();
    }

    void buildShader(const std::string &vertexSource,
                     const std::string &fragmentSource,
                     const std::string &geometrySource) {
        const auto vertSrc = vertexSource.c_str();
        const auto fragSrc = fragmentSource.c_str();
        const auto geoSrc = geometrySource.c_str();

        programHandle = glCreateProgram();

        const GLuint vsH = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsH, 1, &vertSrc, nullptr);
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

        const GLuint gsH = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gsH, 1, &geoSrc, nullptr);
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

        const GLuint fsH = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsH, 1, &fragSrc, nullptr);
        glCompileShader(fsH);
        glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(fsH, 512, nullptr, infoLog);
            glDeleteShader(vsH);
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
        glDeleteShader(gsH);

        glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
            glDeleteProgram(programHandle);
            std::string error = "Failed to link shader program: ";
            error.append(infoLog);
            throw std::runtime_error(error);
        }

        oglCheckError();
    }
};

#endif //XENGINE_OGLSHADERPROGRAM_HPP
