/**
 *  Mana - 3D Game Engine
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

#ifndef XENGINE_QTOGLMESHBUFFER_HPP
#define XENGINE_QTOGLMESHBUFFER_HPP

#include "platform/graphics/meshbuffer.hpp"
#include "math/matrixmath.hpp"

#include "qtoglcheckerror.hpp"
#include "qtopenglinclude.hpp"

#include <QOpenGLFunctions_4_5_Core>

namespace xengine {
    namespace opengl {
        class QtOGLMeshBuffer : public MeshBuffer, public QOpenGLFunctions_4_5_Core {
        public:
            GLuint VAO;
            GLuint VBO;
            GLuint EBO;

            size_t elementCount;
            GLuint elementType;

            bool indexed;

            bool instanced;
            size_t instanceCount;
            GLuint instanceVBO;

            explicit QtOGLMeshBuffer() : VAO(0),
                                         VBO(0),
                                         EBO(0),
                                         elementCount(0),
                                         elementType(GL_TRIANGLES),
                                         indexed(false),
                                         instanced(false),
                                         instanceCount(0),
                                         instanceVBO(0) {
                QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
            }

            QtOGLMeshBuffer(const QtOGLMeshBuffer &copy) = delete;

            QtOGLMeshBuffer &operator=(const QtOGLMeshBuffer &copy) = delete;

            ~QtOGLMeshBuffer() override {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                if (indexed) {
                    glDeleteBuffers(1, &EBO);
                }
                glDeleteBuffers(1, &instanceVBO);
            }
        };
    }
}

#endif //XENGINE_QTOGLMESHBUFFER_HPP
