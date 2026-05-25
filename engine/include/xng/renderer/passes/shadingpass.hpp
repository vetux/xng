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

#ifndef XENGINE_SHADINGPASS_HPP
#define XENGINE_SHADINGPASS_HPP

#include "xng/renderer/renderpass.hpp"

namespace xng {
    /**
     * Performs deferred and forward shading of the default shading models using the outputs from the
     * Construction / Shadow, and IBL passes and stores the result in a compositing layer.
     */
    class ShadingPass final : public RenderPass {
    public:
        void record(rg::GraphBuilder &builder,
                    rg::Surface &surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override;
    };
}

#endif //XENGINE_SHADINGPASS_HPP
