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

#ifndef XENGINE_PRESENTATIONPASS_HPP
#define XENGINE_PRESENTATIONPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

#include "xng/render/scene.hpp"

namespace xng {
    /**
     * The presentation pass presents the screen color texture (In render resolution) to the screen
     * and performs upscaling / downscaling when necessary.
     *
     * Reads SLOT_SCREEN_COLOR and writes to the backbuffer
     */
    class XENGINE_EXPORT PresentationPass : public FrameGraphPass {
    public:
        PresentationPass();

        ~PresentationPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override { return typeid(PresentationPass); };
    };
}
#endif //XENGINE_PRESENTATIONPASS_HPP
