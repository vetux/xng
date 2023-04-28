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
     * The composite pass blits the SLOT_DEFERRED_COLOR texture and blends the SLOT_FORWARD_COLOR texture ontop and stores the result in the SLOT_SCREEN_COLOR and SLOT_SCREEN_DEPTH slots.
     */
    class XENGINE_EXPORT CompositePass : public FrameGraphPass {
    public:
        CompositePass();

        ~CompositePass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() const override;

    private:
        Mesh mesh = Mesh::normalizedQuad();

        bool quadAllocated = false;

        FrameGraphResource target;
        FrameGraphResource blitTarget;

        FrameGraphResource pipeline;
        FrameGraphResource pass;

        FrameGraphResource vertexBuffer;
        FrameGraphResource vertexArrayObject;

        FrameGraphResource screenColor;
        FrameGraphResource screenDepth;

        FrameGraphResource deferredColor;
        FrameGraphResource deferredDepth;

        FrameGraphResource forwardColor;
        FrameGraphResource forwardDepth;
    };
}

#endif //XENGINE_COMPOSITEPASS_HPP
