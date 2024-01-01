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

#ifndef XENGINE_CLEARPASS_HPP
#define XENGINE_CLEARPASS_HPP

#include "xng/resource/uri.hpp"

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphtextureatlas.hpp"
#include "xng/render/scene/scene.hpp"
#include "xng/render/graph/meshallocator.hpp"

namespace xng {
    /**
     * The ClearPass creates and clears the SLOT_SCREEN_COLOR-SLOT_BACKGROUND_COLOR slot textures.
     */
    class XENGINE_EXPORT ClearPass : public FrameGraphPass {
    public:
        ~ClearPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;
    };
}
#endif //XENGINE_CLEARPASS_HPP
