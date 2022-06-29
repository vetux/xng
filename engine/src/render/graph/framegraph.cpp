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

#include "render/graph/framegraph.hpp"
#include "render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraph::FrameGraph(std::vector<std::shared_ptr<FrameGraphPass>> passes,
                           std::vector<std::set<FrameGraphResource>> passResources,
                           std::vector<std::function<RenderObject &()>> resources)
            : passes(std::move(passes)), passResources(std::move(passResources)), resources(std::move(resources)) {}

    void FrameGraph::render(RenderDevice &ren) {
        compile();
        execute(ren);
    }

    void FrameGraph::compile() {
        for (auto &s: passResources) {
            std::map<FrameGraphResource, RenderObject *> objects;
            for (auto &res: s) {
                objects[res] = &resources.at(res.index)();
            }
            passData.emplace_back(FrameGraphPassResources(objects));
        }
    }

    void FrameGraph::execute(RenderDevice &ren) {
        blackboard.clear();
        int i = 0;
        for (auto &p: passes) {
            p->execute(passData.at(i++), ren, blackboard);
        }
    }
}