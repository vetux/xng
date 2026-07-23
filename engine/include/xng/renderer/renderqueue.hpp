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

#ifndef XENGINE_RENDERQUEUE_HPP
#define XENGINE_RENDERQUEUE_HPP

#include "xng/rendergraph/runtime.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

namespace xng {
    /**
     * The RenderQueue manages the graph submission/s for a given frame.
     */
    class RenderQueue {
    public:
        class SubmitFence {
        public:
            [[nodiscard]] bool isSignaled() const {
                return fence && fence->isSignaled();
            }

            [[nodiscard]] bool wait(const size_t timeout) const {
                return fence && fence->wait(timeout);
            }

        protected:
            std::shared_ptr<rg::Fence> fence = nullptr;

            friend class RenderQueue;
        };

        RenderQueue() = default;

        std::shared_ptr<SubmitFence> addTransfer(rg::TransferPass transfer) {
            auto fence = std::make_shared<SubmitFence>();
            transfers.emplace_back(std::move(transfer));
            transferFences.emplace_back(fence);
            return fence;
        }

        void addPreFrame(rg::Pass pass) {
            preFrame.emplace_back(std::move(pass));
        }

        std::shared_ptr<SubmitFence> addPostFrame(rg::Pass pass) {
            auto fence = std::make_shared<SubmitFence>();
            postFrame.emplace_back(std::move(pass));
            postFrameFences.emplace_back(fence);
            return fence;
        }

        void addFrame(rg::Pass pass) {
            frame.addPass(std::move(pass));
        }

        rg::GraphBuilder &getFrameBuilder() {
            return frame;
        }

        std::shared_ptr<rg::Fence> submit(rg::Runtime &runtime) {
            // Submit Transfers
            const std::shared_ptr transferFence = std::move(runtime.execute(rg::Graph(transfers, {}, {})));
            for (auto &fence: transferFences) {
                fence->fence = transferFence;
            }
            transfers.clear();
            transferFences.clear();

            // Submit Frame
            auto graph = frame.build();
            graph.passes.insert(graph.passes.begin(), preFrame.begin(), preFrame.end());
            graph.passes.insert(graph.passes.end(), postFrame.begin(), postFrame.end());
            const std::shared_ptr postFrameFence = std::move(runtime.execute(graph));
            for (auto &fence: postFrameFences) {
                fence->fence = postFrameFence;
            }
            preFrame.clear();
            frame = rg::GraphBuilder();
            postFrame.clear();
            postFrameFences.clear();
            return postFrameFence;
        }

    private:
        std::vector<rg::Pass> transfers;
        std::vector<std::shared_ptr<SubmitFence> > transferFences;

        std::vector<rg::Pass> preFrame;
        rg::GraphBuilder frame;
        std::vector<rg::Pass> postFrame;
        std::vector<std::shared_ptr<SubmitFence> > postFrameFences;
    };
}

#endif //XENGINE_RENDERQUEUE_HPP
