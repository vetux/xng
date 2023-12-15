/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPHRUNTIME_HPP
#define XENGINE_FRAMEGRAPHRUNTIME_HPP

#include "xng/render/graph/framegraph.hpp"

#include "xng/shader/shadercompiler.hpp"
#include "xng/shader/shaderdecompiler.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    /**
     * A frame graph runtime executes frame graphs.
     */
    class FrameGraphRuntime {
    public:
        virtual ~FrameGraphRuntime() = default;

        virtual void execute(const FrameGraph &graph) = 0;

        virtual const RenderTargetDesc &getBackBufferDesc() = 0;

        virtual const RenderDeviceInfo &getRenderDeviceInfo() = 0;
    };
}
#endif //XENGINE_FRAMEGRAPHRUNTIME_HPP
