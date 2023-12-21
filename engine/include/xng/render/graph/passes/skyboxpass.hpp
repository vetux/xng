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
#include "xng/render/graph/framegraphresource.hpp"

#include "xng/render/scene.hpp"

namespace xng {
    /**
     * Clears the background texture with the skybox color and then draws the optional skybox texture on a normalized cube without transformation.
     *
     * Writes SLOT_BACKGROUND_COLOR
     */
    class XENGINE_EXPORT SkyboxPass : public FrameGraphPass {
    public:
        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;

    private:
        Mesh cube = Mesh::normalizedCube();

        FrameGraphResource pipeline;
        FrameGraphResource vertexBuffer;
        FrameGraphResource indexBuffer;

        FrameGraphResource skyboxTexture;

        Skybox skybox;
    };
}

#endif //XENGINE_SKYBOXPASS_HPP
