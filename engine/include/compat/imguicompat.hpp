/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_IMGUICOMPAT_HPP
#define XENGINE_IMGUICOMPAT_HPP

#include "graphics/rendertarget.hpp"
#include "display/window.hpp"

namespace xng {
    namespace ImGuiCompat {
        /**
         * Calls the Impl*Init* methods and ImGui::CreateContext if it is the first call.
         * @param window
         */
        XENGINE_EXPORT void Init(Window &window);

        /**
         * Calls the Impl*Shutdown methods and ImGui::DestroyContext if it is the last call.
         * @param window
         */
        XENGINE_EXPORT void Shutdown(Window &window);

        /**
         * Calls Impl*NewFrame methods, the user still has to call ImGui::NewFrame afterwards.
         * @param window
         */
        XENGINE_EXPORT void NewFrame(Window &window);

        /**
         * Calls Impl*RenderDrawData and renders the imgui data into the target.
         * The user still has to call ImGui::Render beforehand.
         *
         * Users can attach a texture buffer to the window render target to access the result.
         *
         * @param window
         * @param target
         */
        XENGINE_EXPORT void DrawData(Window &window, RenderTarget &target, bool clear);
    }
}
#endif //XENGINE_IMGUICOMPAT_HPP
