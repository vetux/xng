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

#ifndef XENGINE_VULKAN_HPP
#define XENGINE_VULKAN_HPP

#include "xng/rendergraph/runtime.hpp"

namespace xng::vulkan {
    class XENGINE_EXPORT Runtime final : public xng::rendergraph::Runtime {
    public:
        explicit Runtime(const std::vector<std::string> &extensions);

        ~Runtime() override;

        std::shared_ptr<rendergraph::Surface> createSurface(std::shared_ptr<Window> window) override;

        void setFramesInFlight(size_t framesInFlight) override;

        rendergraph::Heap &getResourceHeap() override;

        rendergraph::PipelineCache &getPipelineCache() override;

        rendergraph::Statistics execute(const rendergraph::Graph &graph) override;

        rendergraph::Statistics execute(const std::vector<rendergraph::Graph> &graphs) override;
    };
}

#endif //XENGINE_VULKAN_HPP
