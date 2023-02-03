/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_SHADOWMAPPINGPASS_HPP
#define XENGINE_SHADOWMAPPINGPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The shadow mapping pass creates a shadow map which contains shadowing data for a rendered frame.
     * The lighting model resolve pass applies the shadow map to the shaded image.
     *
     * No Dependencies
     */
    class XENGINE_EXPORT ShadowMappingPass : public FrameGraphPass {
    public:
        //FrameGraphResource to a Texture R : Contains values between 0 - 1 indicating if a pixel is in shadow or not.
        SHARED_PROPERTY(ShadowMappingPass, SHADOW_MAP)

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() override;
    };
}
#endif //XENGINE_SHADOWMAPPINGPASS_HPP
