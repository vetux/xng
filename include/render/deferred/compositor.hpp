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

#ifndef XENGINE_COMPOSITOR_HPP
#define XENGINE_COMPOSITOR_HPP

#include "render/deferred/passchain.hpp"

#include "platform/graphics/renderdevice.hpp"
#include "platform/graphics/rendercommand.hpp"

namespace xengine {
    /**
     * The compositor creates the final image on the screen by combining textures returned by a pass chain object.
     *
     * It has the task of relating color and depth textures created by the render passes
     * and outputting the correct pixels for the final image.
     *
     * The class can be inherited to define custom logic at the end of the pipeline for example for post processing effects.
     */
    class XENGINE_EXPORT Compositor {
    public:
        explicit Compositor(RenderDevice &device);

        virtual void setClearColor(ColorRGBA color);

        virtual void present(RenderTarget &screen, PassChain &chain);

    protected:
        virtual void drawNode(RenderTarget &screen, PassChain::Node &node);

        RenderDevice &device;
        std::unique_ptr<ShaderProgram> shader;
        std::unique_ptr<MeshBuffer> screenQuad;
        ColorRGBA clearColor{50, 50, 0, 0};
    };
}

#endif //XENGINE_COMPOSITOR_HPP
