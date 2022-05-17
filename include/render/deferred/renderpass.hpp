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

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include "asset/scene.hpp"

#include "render/deferred/gbuffer.hpp"

#include "render/platform/renderer.hpp"
#include "render/platform/renderdevice.hpp"

namespace xengine {
    /**
     * Users may extend this type to create custom render passes.
     */
    class XENGINE_EXPORT RenderPass {
    public:
        struct Output {
            //The textures used by the default compositor if assigned.
            std::unique_ptr<TextureBuffer> color = nullptr;
            std::unique_ptr<TextureBuffer> depth = nullptr;
        } output;

        virtual ~RenderPass() = default;

        /**
         * Run the render logic and update the output.
         */
        virtual void render(GBuffer &gBuffer, Scene &scene) = 0;

        virtual void resize(Vec2i size, int samples) {
            resizeOutputColor(size);
            resizeOutputDepth(size);
        };

    protected:
        void resizeOutputColor(Vec2i size) {
            auto &alloc = device.getAllocator();
            TextureBuffer::Attributes attrib;
            attrib.size = size;
            output.color = std::unique_ptr<TextureBuffer>(std::move(alloc.createTextureBuffer(attrib)));
        }

        void resizeOutputDepth(Vec2i size) {
            auto &alloc = device.getAllocator();
            TextureBuffer::Attributes attrib;
            attrib.size = size;
            attrib.format = TextureBuffer::DEPTH_STENCIL;
            output.depth = std::unique_ptr<TextureBuffer>(std::move(alloc.createTextureBuffer(attrib)));
        }

        explicit RenderPass(RenderDevice &device)
                : device(device) {}

        RenderDevice &device;
    };
}

#endif //XENGINE_RENDERPASS _HPP
