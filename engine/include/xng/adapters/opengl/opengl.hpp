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

#ifndef XENGINE_OPENGL_HPP
#define XENGINE_OPENGL_HPP

#include "xng/rendergraph/rendergraphruntime.hpp"

namespace xng::opengl {
    class XENGINE_EXPORT OpenGL final : public RenderGraphRuntime {
    public:
        OpenGL();

        ~OpenGL() override = default;

        void setWindow(std::shared_ptr<Window> window) override;

        Window &getWindow() override;

        RenderGraphHandle compile(const RenderGraph &graph) override;

        void recompile(RenderGraphHandle handle, const RenderGraph &graph) override;

        void execute(RenderGraphHandle graph) override;

        void execute(const std::vector<RenderGraphHandle> &graphs) override;

        void destroy(RenderGraphHandle graph) override;

        void saveCache(RenderGraphHandle graph, std::ostream &stream) override;

        void loadCache(RenderGraphHandle graph, std::istream &stream) override;

        GraphicsAPI getGraphicsAPI() override { return OPENGL_4_6; }

    private:
        std::unique_ptr<RenderGraphRuntime> runtime;
    };
}

#endif //XENGINE_OPENGL_HPP
