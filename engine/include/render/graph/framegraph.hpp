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

#ifndef XENGINE_FRAMEGRAPH_HPP
#define XENGINE_FRAMEGRAPH_HPP

#include "resource/resourcehandle.hpp"

#include "graphics/renderobject.hpp"
#include "graphics/renderer.hpp"

#include "render/graph/objectpool.hpp"
#include "render/graph/framegraphresource.hpp"
#include "render/graph/renderpassresources.hpp"
#include "render/graph/framegraphblackboard.hpp"

namespace xengine {
    class RenderPass;

    class XENGINE_EXPORT FrameGraph {
    public:
        FrameGraph(std::vector<std::shared_ptr<RenderPass>> passes,
                   std::vector<std::set<FrameGraphResource>> passResources,
                   std::vector<std::function<RenderObject &()>> resources);

        void render(Renderer &ren);

    private:
        void compile();

        void execute(Renderer &ren);

        std::vector<std::shared_ptr<RenderPass>> passes;
        std::vector<std::set<FrameGraphResource>> passResources;
        std::vector<std::function<RenderObject &()>> resources;

        std::vector<RenderPassResources> passData;

        FrameGraphBlackboard blackboard;
    };
}

#endif //XENGINE_FRAMEGRAPH_HPP
