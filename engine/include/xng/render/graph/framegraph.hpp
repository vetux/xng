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

#ifndef XENGINE_FRAMEGRAPH_HPP
#define XENGINE_FRAMEGRAPH_HPP

#include "xng/render/graph/framegraphresource.hpp"
#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphcommand.hpp"
#include "xng/render/graph/framegraphslot.hpp"
#include "xng/render/graph/framegraphcontext.hpp"

namespace xng {
    struct FrameGraph {
        std::vector<FrameGraphContext> contexts;

        std::map<FrameGraphSlot, FrameGraphResource> slotAssignments;

        FrameGraphResource backBuffer;

        std::set<FrameGraphResource> getPersistentResources() const {
            std::set<FrameGraphResource> ret;
            for (auto &stage: contexts) {
                ret.insert(stage.persists.begin(), stage.persists.end());
            }
            return ret;
        }

        bool checkResource(FrameGraphResource resource) {
            for (auto &stage: contexts) {
                if (stage.persists.contains(resource)) {
                    return true;
                } else {
                    for (auto &c: stage.commands) {
                        if (std::find(c.resources.begin(), c.resources.end(), resource) != c.resources.end()) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    };
}

#endif //XENGINE_FRAMEGRAPH_HPP
