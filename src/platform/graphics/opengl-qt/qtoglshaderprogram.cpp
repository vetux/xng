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

#include "math/rotation.hpp"
#include "math/matrixmath.hpp"

#include "qtoglshaderprogram.hpp"
#include "qtoglcheckerror.hpp"

namespace xengine {
    namespace opengl {
        QtOGLShaderProgram::QtOGLShaderProgram() : programID(0) {}

        QtOGLShaderProgram::QtOGLShaderProgram(const std::string &vertexShader,
                                           const std::string &geometryShader,
                                           const std::string &fragmentShader,
                                           const std::string &prefix)
                : programID(0), prefix(prefix) {
            QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

            const char *vertexSource = vertexShader.c_str();
            const char *geometrySource = geometryShader.c_str();
            const char *fragmentSource = fragmentShader.c_str();

            programID = glCreateProgram();


            unsigned int vsH = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vsH, 1, &vertexSource, NULL);
            glCompileShader(vsH);
            int success;
            glGetShaderiv(vsH, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(vsH, 512, NULL, infoLog);
                glDeleteShader(vsH);
                std::string error = "Failed to compile vertex shader: ";
                error.append(infoLog);
                throw std::runtime_error(error);
            }

            glAttachShader(programID, vsH);

            unsigned int gsH;

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
                glAttachShader(programID, gsH);
            }

            unsigned int fsH = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fsH, 1, &fragmentSource, NULL);
            glCompileShader(fsH);
            glGetShaderiv(fsH, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(fsH, 512, NULL, infoLog);
                glDeleteShader(vsH);
                if (!geometryShader.empty())
                    glDeleteShader(gsH);
                glDeleteShader(fsH);
                std::string error = "Failed to compile fragment shader: ";
                error.append(infoLog);
                throw std::runtime_error(error);
            }
            glAttachShader(programID, fsH);

            glLinkProgram(programID);

            glDeleteShader(vsH);
            glDeleteShader(fsH);

            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(programID, 512, NULL, infoLog);
                std::string error = "Failed to link shader program: ";
                error.append(infoLog);
                throw std::runtime_error(error);
            }

            checkGLError("");
        }

        QtOGLShaderProgram::~QtOGLShaderProgram() {
            glDeleteProgram(programID);
        }

        void QtOGLShaderProgram::activate() {
            glUseProgram(programID);
            checkGLError("");
        }

        bool QtOGLShaderProgram::setTexture(const std::string &name, int slot) {
            //Samplers do not appear to get merged into the global struct when cross compiling
            auto it = locations.find(name);
            if (it != locations.end())
                return setTexture(it->second, slot);
            GLint location = glGetUniformLocation(programID, name.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setTexture(location, slot);
        }

        bool QtOGLShaderProgram::setBool(const std::string &name, bool value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setBool(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setBool(location, value);
        }

        bool QtOGLShaderProgram::setInt(const std::string &name, int value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setInt(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setInt(location, value);
        }

        bool QtOGLShaderProgram::setFloat(const std::string &name, float value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setFloat(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setFloat(location, value);
        }

        bool QtOGLShaderProgram::setVec2(const std::string &name, const Vec2b &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool QtOGLShaderProgram::setVec2(const std::string &name, const Vec2i &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool QtOGLShaderProgram::setVec2(const std::string &name, const Vec2f &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec2(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec2(location, value);
        }

        bool QtOGLShaderProgram::setVec3(const std::string &name, const Vec3b &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool QtOGLShaderProgram::setVec3(const std::string &name, const Vec3i &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool QtOGLShaderProgram::setVec3(const std::string &name, const Vec3f &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec3(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec3(location, value);
        }

        bool QtOGLShaderProgram::setVec4(const std::string &name, const Vec4b &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool QtOGLShaderProgram::setVec4(const std::string &name, const Vec4i &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool QtOGLShaderProgram::setVec4(const std::string &name, const Vec4f &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setVec4(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setVec4(location, value);
        }

        bool QtOGLShaderProgram::setMat2(const std::string &name, const Mat2f &value) {
            std::string prefixName = prefix + name;
            throw std::runtime_error("Not Implemented");
        }

        bool QtOGLShaderProgram::setMat3(const std::string &name, const Mat3f &value) {
            std::string prefixName = prefix + name;
            throw std::runtime_error("Not Implemented");
        }

        bool QtOGLShaderProgram::setMat4(const std::string &name, const Mat4f &value) {
            std::string prefixName = prefix + name;
            auto it = locations.find(prefixName);
            if (it != locations.end())
                return setMat4(it->second, value);
            GLint location = glGetUniformLocation(programID, prefixName.c_str());
            checkGLError();
            if (location >= 0)
                locations[name] = location;
            return setMat4(location, value);
        }

        bool QtOGLShaderProgram::setTexture(int location, int slot) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, (int) slot);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setBool(int location, bool value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, static_cast<int>(value));
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setInt(int location, int value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1i(location, value);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setFloat(int location, float value) {
            if (location < 0) {
                return false;
            } else {
                glUniform1f(location, value);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec2(int location, const Vec2b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2i(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec2(int location, const Vec2i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2i(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec2(int location, const Vec2f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform2f(location, value.x, value.y);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec3(int location, const Vec3b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3i(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec3(int location, const Vec3i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3i(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec3(int location, const Vec3f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform3f(location, value.x, value.y, value.z);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec4(int location, const Vec4b &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4i(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec4(int location, const Vec4i &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4i(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setVec4(int location, const Vec4f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniform4f(location, value.x, value.y, value.z, value.w);
                checkGLError();
                return true;
            }
        }

        bool QtOGLShaderProgram::setMat2(int location, const Mat2f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool QtOGLShaderProgram::setMat3(int location, const Mat3f &value) {
            throw std::runtime_error("Not Implemented");
        }

        bool QtOGLShaderProgram::setMat4(int location, const Mat4f &value) {
            if (location < 0) {
                return false;
            } else {
                glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat *) &value);
                checkGLError();
                return true;
            }
        }
    }
}