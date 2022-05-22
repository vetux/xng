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

#include <stdexcept>

#include "cast/numeric_cast.hpp"

#include "math/rotation.hpp"
#include "math/matrixmath.hpp"

#include "render/platform/opengl/oglshaderprogram.hpp"
#include "render/platform/opengl/oglcheckerror.hpp"

namespace xengine {
    namespace opengl {
        OGLShaderProgram::OGLShaderProgram() : programHandle(0) {}

        OGLShaderProgram::OGLShaderProgram(const std::string &vertexShader,
                                           const std::string &fragmentShader,
                                           const std::string &geometryShader)
                : programHandle(0) {
            const char *vertexSource = vertexShader.c_str();
            const char *fragmentSource = fragmentShader.c_str();
            const char *geometrySource = geometryShader.c_str();

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

            if (!geometryShader.empty()) {
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
                if (!geometryShader.empty())
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

        OGLShaderProgram::OGLShaderProgram(const ShaderBinary &binary)
                : programHandle(0) {
            programHandle = glCreateProgram();

            glProgramBinary(programHandle,
                            binary.token,
                            binary.buffer.data(),
                            numeric_cast<GLsizei>(binary.buffer.size()));

            checkLinkSuccess();
            checkGLError("");
        }

        OGLShaderProgram::~OGLShaderProgram() {
            glDeleteProgram(programHandle);
        }

        void OGLShaderProgram::activate() {
            glUseProgram(programHandle);
            checkGLError("");
        }

        bool OGLShaderProgram::setTexture(const std::string &name, int slot) {
            //Samplers do not appear to get merged into the global struct when cross compiling
            auto it = locations.find(name);
            if (it != locations.end())
                return setTexture(it->second, slot);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setTexture(location, slot);
        }

        bool OGLShaderProgram::setBool(const std::string &name, bool value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setBool(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setBool(location, value);
        }

        bool OGLShaderProgram::setInt(const std::string &name, int value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setInt(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setInt(location, value);
        }

        bool OGLShaderProgram::setFloat(const std::string &name, float value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setFloat(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setFloat(location, value);
        }

        bool OGLShaderProgram::setVec2(const std::string &name, const Vec2b &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool OGLShaderProgram::setVec2(const std::string &name, const Vec2i &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool OGLShaderProgram::setVec2(const std::string &name, const Vec2f &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool OGLShaderProgram::setVec3(const std::string &name, const Vec3b &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool OGLShaderProgram::setVec3(const std::string &name, const Vec3i &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool OGLShaderProgram::setVec3(const std::string &name, const Vec3f &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool OGLShaderProgram::setVec4(const std::string &name, const Vec4b &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool OGLShaderProgram::setVec4(const std::string &name, const Vec4i &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool OGLShaderProgram::setVec4(const std::string &name, const Vec4f &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool OGLShaderProgram::setMat2(const std::string &name, const Mat2f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool OGLShaderProgram::setMat3(const std::string &name, const Mat3f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool OGLShaderProgram::setMat4(const std::string &name, const Mat4f &value) {
            auto it = locations.find(name);
            if (it != locations.end())
                return setMat4(it->second, value);
            GLint location = glGetUniformLocation(programHandle, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setMat4(location, value);
        }

        bool OGLShaderProgram::setTexture(int location, int slot) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, (int) slot);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setBool(int location, bool value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, static_cast<int>(value));
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setInt(int location, int value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, value);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setFloat(int location, float value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1f(location, value);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec2(int location, const Vec2b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2i(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec2(int location, const Vec2i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2i(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec2(int location, const Vec2f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2f(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec3(int location, const Vec3b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3i(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec3(int location, const Vec3i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3i(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec3(int location, const Vec3f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3f(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec4(int location, const Vec4b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4i(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec4(int location, const Vec4i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4i(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setVec4(int location, const Vec4f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4f(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool OGLShaderProgram::setMat2(int location, const Mat2f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool OGLShaderProgram::setMat3(int location, const Mat3f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool OGLShaderProgram::setMat4(int location, const Mat4f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat *) &value);
                checkGLError();
                return true;
            }
        }

        ShaderBinary OGLShaderProgram::getBinary() {
            ShaderBinary ret;

            GLint size;
            glGetProgramiv(programHandle, GL_PROGRAM_BINARY_LENGTH, &size);
            ret.buffer.resize(size);

            GLsizei length;
            GLenum format;
            glGetProgramBinary(programHandle, numeric_cast<GLsizei>(ret.buffer.size()), &length, &format,
                               ret.buffer.data());

            ret.buffer.resize(length);
            ret.token = format;

            return ret;
        }
    }

    void opengl::OGLShaderProgram::checkLinkSuccess() const {
        GLint success;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(programHandle, 512, NULL, infoLog);
            std::string error = "Failed to link shader program: ";
            error.append(infoLog);
            throw std::runtime_error(error);
        }
    }
}