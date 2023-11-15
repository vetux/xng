/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_GLFWRENDERTARGETGL_HPP
#define XENGINE_GLFWRENDERTARGETGL_HPP

#include "gpu/opengl/oglrendertarget.hpp"

#include <GLFW/glfw3.h>

namespace xng::glfw {
    class GLFWRenderTargetGL : public opengl::OGLRenderTarget {
    public:
        GLFWwindow *wndH;

        explicit GLFWRenderTargetGL(GLFWwindow &wnd)
                : opengl::OGLRenderTarget(),
                  wndH(&wnd) {
            desc.samples = glfwGetWindowAttrib(wndH, GLFW_SAMPLES);
            desc.multisample = desc.samples != 0;
        }

        const RenderTargetDesc &getDescription() override {
            glfwGetFramebufferSize(wndH, &desc.size.x, &desc.size.y);
            return desc;
        }

        GLuint getFBO() override {
            return 0;
        };
    };
}

#endif //XENGINE_GLFWRENDERTARGETGL_HPP
