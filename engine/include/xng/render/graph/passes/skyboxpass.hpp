/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_SKYBOXPASS_HPP
#define XENGINE_SKYBOXPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

#include "xng/render/scene.hpp"

namespace xng {
    /**
     * Draws the skybox texture on a normalized cube without transformation.
     *
     * Writes SLOT_DEFERRED_COLOR
     */
    class XENGINE_EXPORT SkyboxPass : public FrameGraphPass {
    public:
        SkyboxPass();

        ~SkyboxPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() const override;

    private:
        FrameGraphResource shader;
        FrameGraphResource skyboxCube;
        FrameGraphResource defaultTexture;

        FrameGraphResource renderTarget;
        FrameGraphResource outColor;
        FrameGraphResource outDepth;

        FrameGraphResource skyboxTexture;
    };
}
#endif //XENGINE_SKYBOXPASS_HPP
