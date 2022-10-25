/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "compat/imguicompat.hpp"

#include "display/window.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef DRIVER_OPENGL

#include "display/glfw/opengl/windowglfwgl.hpp"
#include "gpu/opengl/oglrendertarget.hpp"

namespace xng {
    namespace ImGuiCompat {
        void Init(Window &window) {
            auto displayDriver = window.getDisplayDriver();
            auto graphicsDriver = window.getGraphicsDriver();
            if (displayDriver == "glfw" && graphicsDriver == "opengl") {
                ImGui_ImplGlfw_InitForOpenGL(dynamic_cast<glfw::WindowGLFWGL &>(window).windowHandle(), true);
                ImGui_ImplOpenGL3_Init("#version 460");
            } else {
                throw std::runtime_error("Unsupported display or graphics driver");
            }
        }

        void Shutdown(Window &window) {
            auto displayDriver = window.getDisplayDriver();
            auto graphicsDriver = window.getGraphicsDriver();

            if (displayDriver == "glfw" && graphicsDriver == "opengl") {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
            } else {
                throw std::runtime_error("Unsupported display or graphics driver");
            }
        }

        void NewFrame(Window &window) {
            auto displayDriver = window.getDisplayDriver();
            auto graphicsDriver = window.getGraphicsDriver();

            if (graphicsDriver == "opengl") {
                ImGui_ImplOpenGL3_NewFrame();
            } else {
                throw std::runtime_error("Unsupported graphics driver");
            }

            if (displayDriver == "glfw") {
                ImGui_ImplGlfw_NewFrame();
            } else {
                throw std::runtime_error("Unsupported display driver");
            }
        }

        void DrawData(Window &window, RenderTarget &target, bool clear) {
            auto displayDriver = window.getDisplayDriver();
            auto graphicsDriver = window.getGraphicsDriver();

            if (graphicsDriver == "opengl") {
                auto &t = dynamic_cast<opengl::OGLRenderTarget &>(target);
                glBindFramebuffer(GL_FRAMEBUFFER, t.getFBO());
                glViewport(0,
                           0,
                           t.desc.size.x,
                           t.desc.size.y);
                const auto clearColor = ColorRGBA::black();
                glClearColor(clearColor.r(),
                             clearColor.g(),
                             clearColor.b(),
                             clearColor.a());
                GLenum clearFlags = 0;
                if (clear) {
                    clearFlags |= GL_COLOR_BUFFER_BIT;
                    clearFlags |= GL_DEPTH_BUFFER_BIT;
                    clearFlags |= GL_STENCIL_BUFFER_BIT;
                }
                glClear(clearFlags);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            } else {
                throw std::runtime_error("Unsupported graphics driver");
            }
        }
    }
}

#endif