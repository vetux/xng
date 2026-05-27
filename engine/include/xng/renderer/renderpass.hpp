/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include "xng/renderer/renderscene.hpp"
#include "xng/renderer/renderpassregistry.hpp"

#include "xng/rendergraph/builder/graphbuilder.hpp"

namespace xng {
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        /**
         * The passes can assume that the arguments are referenceable until their callback has run and stay stable
         * across callback invocations.
         *
         * @param builder
         * @param surface
         * @param registry
         * @param scene
         */
        virtual void record(rg::GraphBuilder &builder,
                            std::shared_ptr<rg::Surface> surface,
                            RenderPassRegistry &registry,
                            const RenderScene &scene) = 0;
    };
}

#endif //XENGINE_RENDERPASS_HPP
