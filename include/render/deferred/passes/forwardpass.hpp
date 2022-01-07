/**
 *  XEngine - C++ game engine library
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

#ifndef XENGINE_FORWARDPASS_HPP
#define XENGINE_FORWARDPASS_HPP

#include "render/deferred/renderpass.hpp"

namespace xengine {
    class XENGINE_EXPORT ForwardPass : public RenderPass {
    public:
        static const char *COLOR;
        static const char *DEPTH;

        explicit ForwardPass(RenderDevice &device);

        ~ForwardPass() override;

        void prepareBuffer(GeometryBuffer &gBuffer) override;

        void render(GeometryBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) override;

    private:
        RenderDevice &device;
    };
}

#endif //XENGINE_FORWARDPASS_HPP
