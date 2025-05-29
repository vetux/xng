/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_VKRENDERTARGET_HPP
#define XENGINE_VKRENDERTARGET_HPP

#include "xng/gpu/rendertarget.hpp"

#include "vulkan/vulkan.h"

namespace xng::vulkan {
    class VkRenderTarget : public RenderTarget {
    public:
        RenderTargetDesc desc;

        VkInstance instance;
        VkSurfaceKHR surface;

        explicit VkRenderTarget(VkInstance instance) : instance(instance), surface() {}

        ~VkRenderTarget() override {
            if (surface) {
                vkDestroySurfaceKHR(instance, surface, nullptr);
            }
        }

        const RenderTargetDesc &getDescription() override {
            return desc;
        }

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments) override {

        }

        void setAttachments(const std::vector<RenderTargetAttachment> &colorAttachments,
                            RenderTargetAttachment depthStencilAttachment) override {

        }

        void clearAttachments() override {

        }

        bool isComplete() override {
            return false;
        }

        virtual VkSurfaceKHR getSurface() {
            return surface;
        }
    };
}
#endif //XENGINE_VKRENDERTARGET_HPP
