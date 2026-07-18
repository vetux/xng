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

#ifndef XENGINE_STREAMERQUEUE_HPP
#define XENGINE_STREAMERQUEUE_HPP

#include <utility>

#include "xng/rendergraph/runtime.hpp"

namespace xng {
    /**
     * All streaming passes execute on the streamer queue.
     *
     * The streamer queue executes on a transfer queue if the render graph implementation supports it,
     * which means they run in parallel with RenderPasses.
     *
     * All time critical per frame operations should only be run inside RenderPasses which allows true asynchronous streaming to run
     * concurrently with the per frame operations.
     */
    class StreamerQueue {
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

            friend class StreamerQueue;
        };

        explicit StreamerQueue(rg::Runtime &runtime) : runtime(runtime) {
        }

        std::shared_ptr<SubmitFence> addPass(rg::TransferPass pass) {
            auto sem = std::make_shared<SubmitFence>();
            passes.emplace_back(std::move(pass), sem);
            return sem;
        }

        void submit() {
            rg::Graph graph;
            for (auto &submit: passes) {
                graph.passes.emplace_back(std::move(submit.pass));
            }
            const std::shared_ptr sem = std::move(runtime.execute(graph));
            for (const auto &submit: passes) {
                submit.fence->fence = sem;
            }
            passes.clear();
        }

    private:
        struct PassSubmit {
            rg::TransferPass pass;
            std::shared_ptr<SubmitFence> fence;

            PassSubmit(rg::TransferPass pass, const std::shared_ptr<SubmitFence> &fence)
                : pass(std::move(pass)), fence(fence) {
            }
        };

        rg::Runtime &runtime;
        std::vector<PassSubmit> passes;
    };
}

#endif //XENGINE_STREAMERQUEUE_HPP
