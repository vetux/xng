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

#include "compat/imguicompat.hpp"

#include "display/window.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "display/glfw/opengl/windowglfwgl.hpp"
#include "graphics/opengl/oglrendertarget.hpp"

namespace xengine {
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

        void DrawData(Window &window, RenderTarget &target) {
            return DrawData(window, target, RenderOptions({}, target.getSize()));
        }

        void DrawData(Window &window, RenderTarget &target, RenderOptions options) {
            auto displayDriver = window.getDisplayDriver();
            auto graphicsDriver = window.getGraphicsDriver();

            if (graphicsDriver == "opengl") {
                auto &t = dynamic_cast<opengl::OGLRenderTarget &>(target);
                glBindFramebuffer(GL_FRAMEBUFFER, t.getFBO());
                glViewport(options.viewportOffset.x,
                           options.viewportOffset.y,
                           options.viewportSize.x,
                           options.viewportSize.y);
                glClearColor(options.clearColorValue.r(),
                             options.clearColorValue.g(),
                             options.clearColorValue.b(),
                             options.clearColorValue.a());
                GLenum clearFlags = 0;
                if (options.clearColor)
                    clearFlags |= GL_COLOR_BUFFER_BIT;
                if (options.clearDepth)
                    clearFlags |= GL_DEPTH_BUFFER_BIT;
                if (options.clearStencil)
                    clearFlags |= GL_STENCIL_BUFFER_BIT;
                glClear(clearFlags);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                checkGLError("ImGuiCompat");
            } else {
                throw std::runtime_error("Unsupported graphics driver");
            }
        }
    }
}