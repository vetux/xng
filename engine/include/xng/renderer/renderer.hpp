/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERER_HPP
#define XENGINE_RENDERER_HPP

#include "xng/rendergraph/runtime.hpp"

#include "xng/renderer/renderscene.hpp"
#include "xng/renderer/renderpass.hpp"
#include "xng/renderer/rendererstatistics.hpp"
#include "xng/io/byte.hpp"

namespace xng {
    // TODO: Frames in flight implementation
    // TODO: Fix ghosting (Might be linux compositor related or broken synchronization in gl adapter)
    class XENGINE_EXPORT Renderer {
    public:
        static rg::Shader compileSkinningShader();

        /**
         * @param runtime The runtime to use
         * @param streamingBudget The streaming budget (Default 64MB)
         */
        explicit Renderer(rg::Runtime &runtime, const size_t streamingBudget = 256 * (KB(256)))
            : Renderer(runtime, streamingBudget, compileSkinningShader()) {
        }

        Renderer(rg::Runtime &runtime,
                 size_t streamingBudget,
                 const rg::Shader &skinningShader);

        std::shared_ptr<RenderScene> createScene(size_t tileSize = 256,
                                                 size_t tileBorder = 9,
                                                 float maxAnisotropy = 16);

        void setPasses(std::vector<std::shared_ptr<RenderPass> > passes);

        void draw(const std::shared_ptr<rg::Surface> &surface, RenderScene &scene);

    private:
        static constexpr int skinningLocalSize = 64;

        rg::ComputePass recordSkinningPass(const RenderScene &scene) const;

        rg::Runtime &runtime;
        ChunkStreamer chunkStreamer;
        rg::PipelineCache::Handle skinningPipeline;

        std::vector<std::shared_ptr<RenderPass> > passes;
    };
}

#endif //XENGINE_RENDERER_HPP
