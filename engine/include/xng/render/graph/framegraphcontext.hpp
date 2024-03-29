/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPHCONTEXT_HPP
#define XENGINE_FRAMEGRAPHCONTEXT_HPP

#include <vector>
#include <set>

#include "xng/render/graph/framegraphcommand.hpp"
#include "xng/render/graph/framegraphresource.hpp"

namespace xng {
    struct FrameGraphContext {
        std::type_index pass = typeid(FrameGraphPass);
        std::vector<FrameGraphCommand> commands;
        std::set<FrameGraphResource> persists; // The set of resources that are declared to be persistent

        FrameGraphContext() = default;
    };
}
#endif //XENGINE_FRAMEGRAPHCONTEXT_HPP
