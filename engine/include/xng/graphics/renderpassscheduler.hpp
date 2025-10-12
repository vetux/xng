/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_RENDERPASSSCHEDULER_HPP
#define XENGINE_RENDERPASSSCHEDULER_HPP

#include <memory>
#include <vector>

#include "renderpass.hpp"
#include "xng/rendergraph/rendergraphruntime.hpp"

namespace xng {
    /**
     * Handles recompilation of RenderPass objects
     */
    class RenderPassScheduler {
    public:
        explicit RenderPassScheduler(std::shared_ptr<RenderGraphRuntime> rt)
            : runtime(std::move(rt)) {
            updateBackBuffer();
        }

        Vec2i updateBackBuffer() {
            backBufferSize = runtime->updateBackBuffer();
            return backBufferSize;
        }

        RenderGraphHandle addGraph(const std::shared_ptr<RenderPass>& renderPass) {
            std::vector<std::shared_ptr<RenderPass> > renderPasses;
            renderPasses.emplace_back(renderPass);
            return addGraph(renderPasses);
        }

        RenderGraphHandle addGraph(std::vector<std::shared_ptr<RenderPass> > renderPasses) {
            RenderGraphBuilder builder(backBufferSize);
            for (auto &pass: renderPasses) {
                pass->create(builder);
            }
            auto graphHandle = runtime->compile(builder.build());
            graphPasses.emplace(graphHandle, renderPasses);
            return graphHandle;
        }

        RenderGraphStatistics execute(const RenderGraphHandle graphHandle) const {
            bool recompile = false;
            for (const auto &pass: graphPasses.at(graphHandle)) {
                if (pass->shouldRebuild(backBufferSize)) {
                    recompile = true;
                }
            }
            if (recompile) {
                RenderGraphBuilder builder(backBufferSize);
                for (const auto &pass: graphPasses.at(graphHandle)) {
                    pass->recreate(builder);
                }
                runtime->recompile(graphHandle, builder.build());
            }
            return runtime->execute(graphHandle);
        }

        RenderGraphStatistics execute(const std::vector<RenderGraphHandle> &graphHandles) const {
            for (auto &graphHandle: graphHandles) {
                bool recompile = false;
                for (const auto &pass: graphPasses.at(graphHandle)) {
                    if (pass->shouldRebuild(backBufferSize)) {
                        recompile = true;
                    }
                }
                if (recompile) {
                    RenderGraphBuilder builder(backBufferSize);
                    for (const auto &pass: graphPasses.at(graphHandle)) {
                        pass->recreate(builder);
                    }
                    runtime->recompile(graphHandle, builder.build());
                }
            }
            return runtime->execute(graphHandles);
        }

        void destroy(const RenderGraphHandle graphHandle) {
            graphPasses.erase(graphHandle);
            runtime->destroy(graphHandle);
        }

    private:
        Vec2i backBufferSize;

        std::shared_ptr<RenderGraphRuntime> runtime;
        std::map<RenderGraphHandle, std::vector<std::shared_ptr<RenderPass> > > graphPasses;
    };
}

#endif //XENGINE_RENDERPASSSCHEDULER_HPP
