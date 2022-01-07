/**
 *  XEngine - C++ game engine library
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

#ifndef XENGINE_IMGUICOMPAT_HPP
#define XENGINE_IMGUICOMPAT_HPP

#include "platform/graphics/graphicsbackend.hpp"
#include "platform/graphics/rendertarget.hpp"
#include "platform/display/displaybackend.hpp"
#include "platform/display/window.hpp"

namespace xengine {
    namespace ImGuiCompat {
        /**
         * Calls the Impl*Init* methods and ImGui::CreateContext if it is the first call.
         * @param window
         */
        XENGINE_EXPORT void Init(Window &window, GraphicsBackend graphicsBackend);

        /**
         * Calls the Impl*Shutdown methods and ImGui::DestroyContext if it is the last call.
         * @param window
         */
        XENGINE_EXPORT void Shutdown(Window &window, GraphicsBackend graphicsBackend);

        /**
         * Calls Impl*NewFrame methods, the user still has to call ImGui::NewFrame afterwards.
         * @param window
         */
        XENGINE_EXPORT void NewFrame(Window &window, GraphicsBackend graphicsBackend);

        /**
         * Calls Impl*RenderDrawData and renders the imgui data into the target.
         * The user still has to call ImGui::Render beforehand.
         *
         * Users can attach a texture buffer to the window render target to access the result.
         *
         * @param window
         * @param target
         */
        XENGINE_EXPORT void DrawData(Window &window, RenderTarget &target, GraphicsBackend graphicsBackend);

        /**
         * Calls Impl*RenderDrawData and renders the imgui data into the target.
         * The user still has to call ImGui::Render beforehand.
         *
         * Users can attach a texture buffer to the window render target to access the result.
         *
         * @param window
         * @param target
         */
        XENGINE_EXPORT  void DrawData(Window &window,
                                   RenderTarget &target,
                                   RenderOptions options,
                                   GraphicsBackend graphicsBackend);
    }
}
#endif //XENGINE_IMGUICOMPAT_HPP
