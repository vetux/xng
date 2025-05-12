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

#ifndef XENGINE_FG2RUNTIME_HPP
#define XENGINE_FG2RUNTIME_HPP

#include "xng/render/graph2/fggraph.hpp"

namespace xng {
    /**
     * The runtime / context represents the platform dependent implementation of the renderer.
     */
    class FGRuntime {
    public:
        virtual void compile(const FGGraph &graph) = 0;

        virtual void execute(const FGGraph &graph) = 0;
    };
}

#endif //XENGINE_FG2RUNTIME_HPP
