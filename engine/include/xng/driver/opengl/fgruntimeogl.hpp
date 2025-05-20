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

#ifndef XENGINE_FGRUNTIMEOGL_HPP
#define XENGINE_FGRUNTIMEOGL_HPP

#include "xng/render/graph2/fgruntime.hpp"

namespace xng {
    class FGRuntimeOGL : public FGRuntime {
    public:
        void setWindow(const Window &window) override;

        GraphHandle compile(const FGGraph &graph) override;

        void execute(GraphHandle graph) override;

        void saveCache(GraphHandle graph, std::ostream &stream) override;

        void loadCache(GraphHandle graph, std::istream &stream) override;
    };
}

#endif //XENGINE_FGRUNTIMEOGL_HPP
