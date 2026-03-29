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

#ifndef XENGINE_OPENGL_HPP
#define XENGINE_OPENGL_HPP

#include "xng/rendergraph/runtime.hpp"

#include "xng/display/displayenvironment.hpp"

namespace xng::opengl {
    class XENGINE_EXPORT Runtime final : public rendergraph::Runtime {
    public:
        /**
         * To be able to implement the heap, the runtime must create a hidden window
         * because of tight context/window coupling in gl.
         *
         * @param env The env used to create the heap hidden window.
         */
        Runtime(DisplayEnvironment &env);

        ~Runtime() override;

        std::shared_ptr<rendergraph::Surface> createSurface(std::shared_ptr<Window> window) override;

        void setFramesInFlight(size_t framesInFlight) override;

        rendergraph::Heap &getResourceHeap() override;

        rendergraph::PipelineCache &getPipelineCache() override;

        rendergraph::Statistics execute(const rendergraph::Graph &graph) override;

        rendergraph::Statistics execute(const std::vector<rendergraph::Graph> &graphs) override;

    private:
        struct MemberData;
        std::unique_ptr<MemberData> data;
    };
}

#endif //XENGINE_OPENGL_HPP
