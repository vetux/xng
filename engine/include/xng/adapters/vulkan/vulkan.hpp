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

#include "xng/rendergraph/rendergraphruntime.hpp"

#include "xng/display/displayenvironment.hpp"

namespace xng::vulkan {
    class XENGINE_EXPORT RenderGraphRuntime final : public xng::RenderGraphRuntime {
    public:
        explicit RenderGraphRuntime(DisplayEnvironment &displayDriver);

        ~RenderGraphRuntime() override;

        void setWindow(std::shared_ptr<Window> window) override;

        Window &getWindow() override;

        Vec2i updateBackBuffer() override;

        Vec2i getBackBufferSize() override;

        RenderGraphHandle compile(const RenderGraph &graph) override;

        void recompile(RenderGraphHandle handle, const RenderGraph &graph) override;

        RenderGraphStatistics execute(RenderGraphHandle graph) override;

        RenderGraphStatistics execute(const std::vector<RenderGraphHandle> &graphs) override;

        void destroy(RenderGraphHandle graph) override;

        void saveCache(RenderGraphHandle graph, std::ostream &stream) override;

        void loadCache(RenderGraphHandle graph, std::istream &stream) override;

        GraphicsAPI getGraphicsAPI() override;
    };
}

#endif //XENGINE_VULKAN_HPP
