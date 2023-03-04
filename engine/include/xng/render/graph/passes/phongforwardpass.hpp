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

#ifndef XENGINE_PHONGFORWARDPASS_HPP
#define XENGINE_PHONGFORWARDPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The forward SHADE_PHONG* shading model implementation used for transparent objects.
     *
     * No dependencies
     */
    class XENGINE_EXPORT PhongForwardPass : public FrameGraphPass {
    public:
        // FrameGraphResource to a Texture RGBA : Contains the ambient color values
        SHARED_PROPERTY(PhongForwardPass, AMBIENT)

        // FrameGraphResource to a Texture RGBA : Contains the diffuse color values
        SHARED_PROPERTY(PhongForwardPass, DIFFUSE)

        // FrameGraphResource to a Texture RGBA : Contains the specular color values
        SHARED_PROPERTY(PhongForwardPass, SPECULAR)

        // FrameGraphResource to a Texture DEPTH_STENCIL : The depth values
        SHARED_PROPERTY(PhongForwardPass, DEPTH)

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() override;
    };
}
#endif //XENGINE_PHONGFORWARDPASS_HPP
