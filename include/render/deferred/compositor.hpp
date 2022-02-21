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

#ifndef XENGINE_COMPOSITOR_HPP
#define XENGINE_COMPOSITOR_HPP

#include "render/deferred/renderpass.hpp"
#include "render/platform/renderdevice.hpp"
#include "render/platform/rendercommand.hpp"

namespace xengine {
    /**
     * The compositor creates the final image on the screen by combining textures returned by a pass chain object.
     *
     * It has the task of relating color and depth textures created by the render passes
     * and outputting the correct pixels for the final image.
     *
     * The class can be inherited to define custom logic at the end of the pipeline.
     */
    class XENGINE_EXPORT Compositor {
    public:
        explicit Compositor(RenderDevice &device);

        virtual void setClearColor(ColorRGBA color);

        /**
         * Users can override this function to access custom pass members.
         *
         * @param screen
         * @param chain
         */
        virtual void present(RenderTarget &screen, std::vector<std::unique_ptr<RenderPass>> &passes);

    protected:
        struct Layer {
            std::shared_ptr<TextureBuffer> color;
            bool enableBlending = true;
            BlendMode colorBlendModeSource = BlendMode::SRC_ALPHA;
            BlendMode colorBlendModeDest = BlendMode::ONE_MINUS_SRC_ALPHA;

            std::shared_ptr<TextureBuffer> depth;
            DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;
        };

        virtual std::vector<Layer> getLayers(std::vector<std::unique_ptr<RenderPass>> &passes);

        virtual void drawLayer(RenderTarget &screen, const Layer &node);

        RenderDevice &device;
        std::unique_ptr<ShaderProgram> shader;
        std::unique_ptr<MeshBuffer> screenQuad;
        ColorRGBA clearColor{50, 50, 0, 0};
    };
}

#endif //XENGINE_COMPOSITOR_HPP
