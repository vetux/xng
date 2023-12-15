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

#ifndef XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP
#define XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP

#include "xng/render/graph/framegraph.hpp"
#include "xng/render/graph/framegraphruntime.hpp"

#include "xng/shader/shadercompiler.hpp"
#include "xng/shader/shaderdecompiler.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    /**
     * A frame graph runtime with a simple sequential execution model on a single queue and pooled resource allocations.
     */
    class XENGINE_EXPORT FrameGraphRuntimeSimple : public FrameGraphRuntime {
    public:
        FrameGraphRuntimeSimple(RenderTarget &backBuffer,
                                RenderDevice &device,
                                ShaderCompiler &shaderCompiler,
                                ShaderDecompiler &shaderDecompiler);

        void execute(const FrameGraph &graph) override;

        const RenderTargetDesc &getBackBufferDesc() override;

        const RenderDeviceInfo &getRenderDeviceInfo() override;

    private:
        RenderTarget &backBuffer;
        RenderDevice &device;

        ShaderCompiler &shaderCompiler;
        ShaderDecompiler &shaderDecompiler;
    };
}
#endif //XENGINE_FRAMEGRAPHRUNTIMESIMPLE_HPP
