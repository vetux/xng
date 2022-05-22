/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_FRAMEGRAPHPIPELINE_HPP
#define XENGINE_FRAMEGRAPHPIPELINE_HPP

#include "render/pipeline.hpp"

#include "render/graph/objectpool.hpp"
#include "render/graph/renderpass.hpp"

#include "render/platform/renderdevice.hpp"

namespace xengine {
    class XENGINE_EXPORT FrameGraphPipeline : public Pipeline {
    public:
        explicit FrameGraphPipeline(RenderDevice &device);

        void render(RenderTarget &target, const Scene &scene) override;

        void setRenderResolution(Vec2i res) override { renderResolution = res; }

        void setRenderSamples(int samples) override { renderSamples = samples; }

        void setPasses(std::vector<std::shared_ptr<RenderPass>> passes);

    private:
        RenderDevice &device;
        std::vector<std::shared_ptr<RenderPass>> passes;
        ObjectPool pool;
        Vec2i renderResolution = {640, 480};
        int renderSamples{};
    };
}
#endif //XENGINE_FRAMEGRAPHPIPELINE_HPP
