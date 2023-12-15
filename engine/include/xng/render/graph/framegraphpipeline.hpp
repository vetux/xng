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

#ifndef XENGINE_FRAMEGRAPHPIPELINE_HPP
#define XENGINE_FRAMEGRAPHPIPELINE_HPP

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * A pipeline consist of a vector of passes which are setup in the defined order.
     *
     * The side effects (Slots) of the passes determine which operations of the passes
     * the renderer can multi thread / spread over multiple queues etc.
     */
    class FrameGraphPipeline {
    public:
        FrameGraphPipeline() = default;

        explicit FrameGraphPipeline(std::vector<std::shared_ptr<FrameGraphPass>> passes) : passes(std::move(passes)) {}

        FrameGraphPipeline &addPass(const std::shared_ptr<FrameGraphPass>& pass) {
            passes.emplace_back(pass);
            return *this;
        }

        const std::vector<std::shared_ptr<FrameGraphPass>> &getPasses() {
            return passes;
        }

    private:
        std::vector<std::shared_ptr<FrameGraphPass>> passes;
    };
}

#endif //XENGINE_FRAMEGRAPHPIPELINE_HPP
