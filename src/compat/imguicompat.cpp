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

#include <limits>

#include "display/window.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "display/glfw/windowglfw.hpp"
#include "graphics/opengl/oglrendertarget.hpp"

namespace xengine {
    namespace ImGuiCompat {
        void Init(Window &window, GraphicsBackend graphicsBackend) {
            switch (window.getDisplayBackend()) {
                case GLFW:
                    ImGui_ImplGlfw_InitForOpenGL(dynamic_cast<glfw::WindowGLFW &>(window).windowHandle(), true);
                    break;
                default:
                    throw std::runtime_error("Not supported");
            }
            switch (graphicsBackend) {
                case OPENGL_4_6:
                    ImGui_ImplOpenGL3_Init("#version 460");
                    break;
                case DIRECTX_11:
                case VULKAN:
                default:
                    throw std::runtime_error("Not supported");
            }
        }

        void Shutdown(Window &window, GraphicsBackend graphicsBackend) {
            switch (graphicsBackend) {
                case OPENGL_4_6:
                    ImGui_ImplOpenGL3_Shutdown();
                    break;
                case DIRECTX_11:
                case VULKAN:
                default:
                    throw std::runtime_error("Not supported");
            }
            switch (window.getDisplayBackend()) {
                case GLFW:
                    ImGui_ImplGlfw_Shutdown();
                    break;
                default:
                    throw std::runtime_error("Not supported");
            }
        }

        void NewFrame(Window &window, GraphicsBackend graphicsBackend) {
            switch (graphicsBackend) {
                case OPENGL_4_6:
                    ImGui_ImplOpenGL3_NewFrame();
                    break;
                case DIRECTX_11:
                case VULKAN:
                default:
                    throw std::runtime_error("Not supported");
            }
            switch (window.getDisplayBackend()) {
                case GLFW:
                    ImGui_ImplGlfw_NewFrame();
                    break;
                default:
                    throw std::runtime_error("Not supported");
            }
        }

        void DrawData(Window &window, RenderTarget &target, GraphicsBackend graphicsBackend) {
            return DrawData(window, target, RenderOptions({}, target.getSize()), graphicsBackend);
        }

        void DrawData(Window &window, RenderTarget &target, RenderOptions options, GraphicsBackend graphicsBackend) {
            switch (graphicsBackend) {
                case OPENGL_4_6: {
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
                    break;
                }
                case DIRECTX_11:
                case VULKAN:
                default:
                    throw std::runtime_error("Not supported");
            }
        }
    }
}