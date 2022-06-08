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

#ifndef XENGINE_COMPOSITEPASS_HPP
#define XENGINE_COMPOSITEPASS_HPP

#include "render/graph/renderpass.hpp"


namespace xengine {
    class XENGINE_EXPORT CompositePass : public RenderPass {
    public:
        struct XENGINE_EXPORT Layer {
            explicit Layer(TextureBuffer *color = nullptr, TextureBuffer *depth = nullptr)
                    : color(color), depth(depth) {}

            TextureBuffer *color;
            TextureBuffer *depth;
            bool enableBlending = true;
            BlendMode colorBlendModeSource = BlendMode::SRC_ALPHA;
            BlendMode colorBlendModeDest = BlendMode::ONE_MINUS_SRC_ALPHA;
            DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;
        };

        explicit CompositePass(RenderDevice &device, ColorRGBA clearColor = ColorRGBA::black());

        ~CompositePass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(RenderPassResources &resources, RenderDevice &ren, FrameGraphBlackboard &board) override;

        void setClearColor(ColorRGBA color) { clearColor = color; }

    private:
        void drawLayer(Layer layer,
                       RenderDevice &ren,
                       RenderTarget &target,
                       ShaderProgram &shaderProgram,
                       MeshBuffer &screenQuad);

        ColorRGBA clearColor = ColorRGBA::white();

        std::unique_ptr<ShaderProgram> shader;
        std::unique_ptr<MeshBuffer> quadMesh;

        FrameGraphResource backBuffer;
    };
}

#endif //XENGINE_COMPOSITEPASS_HPP
