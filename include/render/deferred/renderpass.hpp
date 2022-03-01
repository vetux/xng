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

#include "render/scene.hpp"

#include "render/deferred/gbuffer.hpp"

#include "render/platform/renderer.hpp"
#include "render/platform/renderdevice.hpp"

namespace xengine {
    /**
     * Users may extend this type to create custom render passes.
     */
    class XENGINE_EXPORT RenderPass {
    public:
        virtual ~RenderPass() = default;

        /**
         * Run the render logic.
         *
         * @param gBuffer The gbuffer to sample from.
         * @param scene The scene data.
         */
        virtual void render(GBuffer &gBuffer, Scene &scene) = 0;

        /**
         * Return the default color texture which the render pass has written to in the last render call.
         * Pass inheritors can expose more color textures via members if needed.
         *
         * Returned texture is required to be TEXTURE_2D because a conventional render pass rasterizes a screenquad
         * and does the multisample resolve while sampling from the GBuffer.
         *
         * Returned texture is required to be of the size of the last render Gbuffer size, and may change
         * when the GBuffer size has changed from a previous render() invocation.
         *
         * @return pointer to TextureBuffer.
         */
        std::shared_ptr<TextureBuffer> getColorBuffer() { return colorBuffer; }

        /**
         * Return the default depth texture.
         *
         * Returned texture is required to be TEXTURE_2D.
         *
         * Returned texture is required to be of the size of the last render Gbuffer size, and may change
         * when the GBuffer size has changed from a previous render() invocation.
         *
         * @return pointer to TextureBuffer or nullptr if no depth texture was created.
         */
        std::shared_ptr<TextureBuffer> getDepthBuffer() { return depthBuffer; }

    protected:
        void resizeTextureBuffers(Vec2i size, RenderAllocator &alloc, bool createDepthBuffer) {
            TextureBuffer::Attributes attrib;
            attrib.size = size;

            colorBuffer = std::shared_ptr<TextureBuffer>(std::move(alloc.createTextureBuffer(attrib)));

            attrib.format = TextureBuffer::DEPTH_STENCIL;
            if (createDepthBuffer)
                depthBuffer = std::shared_ptr<TextureBuffer>(std::move(alloc.createTextureBuffer(attrib)));
            else
                depthBuffer = nullptr;
        }

        std::shared_ptr<TextureBuffer> colorBuffer;
        std::shared_ptr<TextureBuffer> depthBuffer;
    };
}

#endif //XENGINE_RENDERPASS _HPP
