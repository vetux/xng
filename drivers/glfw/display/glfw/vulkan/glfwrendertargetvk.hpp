/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_GLFWRENDERTARGETVK_HPP
#define XENGINE_GLFWRENDERTARGETVK_HPP

#include "gpu/vulkan/vkrendertarget.hpp"

#include "display/glfw/glfwinclude.hpp"

namespace xng::glfw {
    class GLFWRenderTargetVk : public vulkan::VkRenderTarget {
    public:
        GLFWwindow *wndH;

        explicit GLFWRenderTargetVk(GLFWwindow &wnd, VkInstance instance)
                : vulkan::VkRenderTarget(instance),
                  wndH(&wnd) {
            desc.samples = glfwGetWindowAttrib(wndH, GLFW_SAMPLES);
            desc.multisample = desc.samples != 0;
            glfwCreateWindowSurface(instance, wndH, nullptr, &surface);
        }

        const RenderTargetDesc &getDescription() override {
            glfwGetFramebufferSize(wndH, &desc.size.x, &desc.size.y);
            return desc;
        }
    };
}

#endif //XENGINE_GLFWRENDERTARGETVK_HPP
