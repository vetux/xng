/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_COMPOSITEPASS_HPP
#define XENGINE_COMPOSITEPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The composite pass combines the texture layers produced by previous passes
     * and outputs the final composited image to the backbuffer.
     *
     * No Dependencies
     */
    class XENGINE_EXPORT CompositePass : public FrameGraphPass {
    public:
        // std::vector<CompositePass::Layer> : The composite layers that are appended to by preceding passes and read by the composite pass in the setup phase.
        SHARED_PROPERTY(CompositePass, LAYERS)

        struct XENGINE_EXPORT Layer {
            explicit Layer(FrameGraphResource color, FrameGraphResource *depth = nullptr)
                    : color(color), depth(depth) {}

            FrameGraphResource color;
            FrameGraphResource *depth;
            bool enableBlending = true;
            BlendMode colorBlendModeSource = BlendMode::SRC_ALPHA;
            BlendMode colorBlendModeDest = BlendMode::ONE_MINUS_SRC_ALPHA;
            DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;
        };

        CompositePass();

        ~CompositePass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() const override;

    private:
        FrameGraphResource shader;
        FrameGraphResource quadMesh;
        FrameGraphResource backBuffer;
        std::vector<Layer> layers;
    };
}

#endif //XENGINE_COMPOSITEPASS_HPP
