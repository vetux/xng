/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "gpu/renderobject.hpp"
#include "gpu/renderpipelinedesc.hpp"
#include "gpu/rendercommand.hpp"
#include "gpu/rendertarget.hpp"
#include "gpu/gpufence.hpp"

namespace xng {
    class RenderPipeline : public RenderObject {
    public:
        Type getType() override {
            return RENDER_PIPELINE;
        }

        virtual void setViewport(Vec2i viewportOffset, Vec2i viewportSize) = 0;

        virtual std::unique_ptr<GpuFence> render(RenderTarget &target, const std::vector<RenderCommand> &commands) = 0;

        virtual std::vector<uint8_t> cache() = 0;

        virtual const RenderPipelineDesc &getDescription() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
